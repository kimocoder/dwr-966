/******************************************************************************
**
** FILE NAME    : ethsw.c
** AUTHOR       : Kamal Eradath
** DESCRIPTION  : Driver for LAN port seperation
** COPYRIGHT    :      Copyright (c) 2013
**              Lantiq Deutschland GmbH 
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date         $Author                $Comment
** 23 MAR 2013   Kamal               Initiate Version
** 22 APR 2013   Kalyan              Add skb mark handling
** 25 MARCH 2014   Mahipati          Fix: Overflow of portdev when port number
**                                   is greater than 4.
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/if_link.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/if_arp.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/rculist.h>
#include <linux/notifier.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <net/rtnetlink.h>
#include <net/lantiq_ethsw.h>

#if defined(CONFIG_LTQ_PPA_DATAPATH) || defined(CONFIG_LTQ_PPA_DATAPATH_MODULE)
extern int32_t  (*ppa_hook_set_lan_seperate_flag_fn)( uint32_t flag);
extern int32_t  (*ppa_hook_get_lan_seperate_flag_fn)( uint32_t flag);
#endif

//#define DEBUG_ETHSW printk
#define DEBUG_ETHSW 

int newlink = 0;
#define MAX_PORT_NUMBER 6 /* Maximum valid lan switch port is 6 */

static const struct nla_policy ethsw_policy[IFLA_ETHSW_MAX + 1] = {
        [IFLA_ETHSW_PORTS]          = { .type = NLA_STRING },
};

struct ethsw_pcpu_stats {
  u64      rx_packets;
  u64      rx_bytes;
  u64      rx_multicast;
  u64      tx_packets;
  u64      tx_bytes;
  struct u64_stats_sync  syncp;
  u32      rx_errors;
  u32      rx_dropped;
  u32      tx_dropped;
};
struct ethswitch_dev {
  struct net_device *dev;
  struct net_device *basedev;
  struct ethsw_pcpu_stats __percpu *pcpu_stats;
};

/*
 * Global array for lan port separated interfaces. 
 * Array index directly maps to switch port number 
 */
struct ethswitch_dev *portdev[MAX_PORT_NUMBER];

#define ETHSW_FEATURES \
        (NETIF_F_SG | NETIF_F_ALL_CSUM | NETIF_F_HIGHDMA | NETIF_F_FRAGLIST | \
         NETIF_F_GSO | NETIF_F_TSO | NETIF_F_UFO | NETIF_F_GSO_ROBUST | \
         NETIF_F_TSO_ECN | NETIF_F_TSO6)

#define ETHSW_STATE_MASK \
        ((1<<__LINK_STATE_NOCARRIER) | (1<<__LINK_STATE_DORMANT))


/* called under rcu_read_lock() from netif_receive_skb */
static rx_handler_result_t ethsw_handle_frame(struct sk_buff **pskb)
{
  struct net_device *dev;
  unsigned int mark = 0;
  struct sk_buff *skb = *pskb;
  const struct ethhdr *eth = eth_hdr(skb);
  struct ethsw_pcpu_stats *pcpu_stats;
  struct ethswitch_dev *ethswdev;
  
  mark = skb->mark; 
#ifdef CONFIG_NETWORK_EXTMARK
  GET_DATA_FROM_MARK_OPT(skb->mark,LAN_PORT_SEP_MASK,LAN_PORT_SEP_START_BIT_POS,mark);
#else
  mark = mark >> LAN_PORT_SEP_SHIFT;
#endif
  
  DEBUG_ETHSW(KERN_INFO "handling frame with incoming mark %X %s \n",mark,skb->dev->name);

  if ( mark ) {
    mark = mark - 1; /* Decrement by 1 as PPA datapath driver increments by 1. Logic used to avoid confusion for port 0. */
  } else {
    return RX_HANDLER_PASS; /* Not interested in packet coming from non-lan-switched interfaces */
  }
   
  if ( portdev[mark] == NULL )
    return RX_HANDLER_PASS; /* No virtual interface for this switch port */

  dev = portdev[mark]->dev;

  if (unlikely(!(dev->flags & IFF_UP))) {
    kfree_skb(skb);
    return RX_HANDLER_CONSUMED;
  }

  ethswdev  = netdev_priv(dev);
  pcpu_stats = this_cpu_ptr(ethswdev->pcpu_stats);
  skb = skb_share_check(skb, GFP_ATOMIC);
  u64_stats_update_begin(&pcpu_stats->syncp);
  
  if (skb == NULL) {
    pcpu_stats->rx_errors++;
    pcpu_stats->rx_dropped++;
    u64_stats_update_end(&pcpu_stats->syncp);
                
    return RX_HANDLER_CONSUMED;
  } else {
    pcpu_stats->rx_packets++;
    pcpu_stats->rx_bytes += skb->len + ETH_HLEN;
    if (is_multicast_ether_addr(eth->h_dest))
      pcpu_stats->rx_multicast++;
    u64_stats_update_end(&pcpu_stats->syncp);
  }


  skb->dev = dev;
  skb->pkt_type = PACKET_HOST;

  DEBUG_ETHSW(KERN_INFO "sending frame up via %s with mark %X\n",dev->name,mark);

  netif_rx(skb);

  return RX_HANDLER_CONSUMED;
}


static int ethswitch_init(struct net_device *dev)
{
  
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);
  struct net_device *basedev = ethswdev->basedev;

  dev->state  = (dev->state & ~ETHSW_STATE_MASK) |
                                  (basedev->state & ETHSW_STATE_MASK);
  dev->features    = basedev->features & ETHSW_FEATURES;
  dev->features    |= NETIF_F_LLTX;
  dev->gso_max_size       = basedev->gso_max_size;
  dev->iflink             = basedev->ifindex;
  dev->hard_header_len    = basedev->hard_header_len;
  dev->dev_id       = basedev->dev_id;

  if (is_zero_ether_addr(dev->dev_addr))
     memcpy(dev->dev_addr, basedev->dev_addr, dev->addr_len);
  if (is_zero_ether_addr(dev->broadcast))
     memcpy(dev->broadcast, basedev->broadcast, dev->addr_len);
  ethswdev->pcpu_stats = alloc_percpu(struct ethsw_pcpu_stats);
  if (!ethswdev->pcpu_stats)
    return -ENOMEM;

        
  return 0;
}
static void ethswitch_uninit(struct net_device *dev)
{
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);

  free_percpu(ethswdev->pcpu_stats);
}

static struct rtnl_link_stats64 *ethswitch_dev_get_stats64(struct net_device *dev,
               struct rtnl_link_stats64 *stats)
{
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);

  if (ethswdev->pcpu_stats) {
    struct ethsw_pcpu_stats *p;
    u64 rx_packets, rx_bytes, rx_multicast, tx_packets, tx_bytes;
    u32 rx_errors = 0, tx_dropped = 0;
    unsigned int start;
    int i;

    for_each_possible_cpu(i) {
      p = per_cpu_ptr(ethswdev->pcpu_stats, i);
      do {
        start = u64_stats_fetch_begin_bh(&p->syncp);
        rx_packets  = p->rx_packets;
        rx_bytes  = p->rx_bytes;
        rx_multicast  = p->rx_multicast;
        tx_packets  = p->tx_packets;
        tx_bytes  = p->tx_bytes;
      } while (u64_stats_fetch_retry_bh(&p->syncp, start));

      stats->rx_packets  += rx_packets;
      stats->rx_bytes    += rx_bytes;
      stats->multicast  += rx_multicast;
      stats->tx_packets  += tx_packets;
      stats->tx_bytes    += tx_bytes;
      /* rx_errors & tx_dropped are u32, updated
       * without syncp protection.
       */
      rx_errors  += p->rx_errors;
      tx_dropped  += p->tx_dropped;
    }
    stats->rx_errors  = rx_errors;
    stats->rx_dropped  = rx_errors;
    stats->tx_dropped  = tx_dropped;
  }
  return stats;
}


static int ethswitch_open(struct net_device *dev)
{
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);
  struct net_device *basedev = ethswdev->basedev;
  int err;

  if (!(basedev->flags & IFF_UP))
          return -ENETDOWN;

  if (compare_ether_addr(dev->dev_addr, basedev->dev_addr)) {
          err = dev_uc_add(basedev, dev->dev_addr);
          if (err < 0)
                  goto out;
  }

  if (dev->flags & IFF_ALLMULTI) {
          err = dev_set_allmulti(basedev, 1);
          if (err < 0)
                  goto del_unicast;
  }
  if (dev->flags & IFF_PROMISC) {
          err = dev_set_promiscuity(basedev, 1);
          if (err < 0)
                  goto clear_allmulti;
  }
  
  netif_carrier_on(dev);
  return 0;

clear_allmulti:
  if (dev->flags & IFF_ALLMULTI)
      dev_set_allmulti(basedev, -1);
del_unicast:
  if (compare_ether_addr(dev->dev_addr, basedev->dev_addr))
     dev_uc_del(basedev, dev->dev_addr);
out:
  netif_carrier_off(dev);
  return err;
}

static int ethswitch_stop(struct net_device *dev)
{
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);

  struct net_device *real_dev = ethswdev->basedev;
        
  dev_mc_unsync(real_dev, dev);
  if (dev->flags & IFF_ALLMULTI)
     dev_set_allmulti(real_dev, -1);
  dev_uc_del(real_dev, dev->dev_addr);

  return 0;
}

static netdev_tx_t ethswitch_start_xmit(struct sk_buff *skb,
                                            struct net_device *dev)
{
  int i;
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);
  unsigned int len;
  int ret = -1;
  for( i=0; i<MAX_PORT_NUMBER; i++) {
    if ( portdev[i] != NULL ) {
      if (skb->skb_iif == portdev[i]->dev->ifindex ) {
        DEBUG_ETHSW(KERN_INFO "Dropping the looped packet:basedev:%s and dev:%s\n",ethswdev->basedev->name,ethswdev->dev->name);
        kfree_skb(skb);
        goto end;
      }
    }
  }
  skb->dev = ethswdev->basedev;
  len = skb->len;
  // set skb mark
  for( i=0; i<MAX_PORT_NUMBER; i++) {
    if ( portdev[i] != NULL ) {
      if ( memcmp(portdev[i]->dev->name,dev->name,strlen(dev->name)) == 0) {
#ifdef CONFIG_NETWORK_EXTMARK
          SET_DATA_FROM_MARK_OPT(skb->mark,LAN_PORT_SEP_MASK,LAN_PORT_SEP_START_BIT_POS,(1 << i));
#else
          skb->mark = ( i + 1 ) << LAN_PORT_SEP_SHIFT; /* Increment i by 1 to avoid confusion for port 0 */
#endif
          DEBUG_ETHSW(KERN_INFO "queuing packet with mark  %X from interface %s\n",skb->mark,ethswdev->dev->name);
          break;
      }
    }
  }
        
  ret = dev_queue_xmit(skb);
        
  if (likely(ret == NET_XMIT_SUCCESS)) {
    struct ethsw_pcpu_stats *pcpu_stats;
    pcpu_stats = this_cpu_ptr(ethswdev->pcpu_stats);
    u64_stats_update_begin(&pcpu_stats->syncp);
    pcpu_stats->tx_packets++;
    pcpu_stats->tx_bytes += len;
    u64_stats_update_end(&pcpu_stats->syncp);
        } else {
    this_cpu_inc(ethswdev->pcpu_stats->tx_dropped);
  }

end:
        return NETDEV_TX_OK;
}

static int ethswitch_set_mac_address(struct net_device *dev, void *p)
{
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);
  struct net_device *basedev = ethswdev->basedev;
  struct sockaddr *addr = p;
  int err;

  if (!is_valid_ether_addr(addr->sa_data))
          return -EADDRNOTAVAIL;

  if (!(dev->flags & IFF_UP))
          goto out;

  if (compare_ether_addr(addr->sa_data, basedev->dev_addr)) {
      
    err = dev_uc_add(basedev, addr->sa_data);
    if (err < 0)
    return err;
  }

  if (compare_ether_addr(dev->dev_addr, basedev->dev_addr))
     dev_uc_del(basedev, dev->dev_addr);

out:
  memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
  return 0;
}

static int ethswitch_change_mtu(struct net_device *dev, int new_mtu)
{
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);
  struct net_device *basedev = ethswdev->basedev;

  if (basedev->mtu < new_mtu)
          return -ERANGE;

  dev->mtu = new_mtu;

  return 0;
}

static void ethswitch_change_rx_flags(struct net_device *dev, int change)
{
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);
  struct net_device *basedev = ethswdev->basedev;

  if (change & IFF_ALLMULTI)
     dev_set_allmulti(basedev, dev->flags & IFF_ALLMULTI ? 1 : -1);
  if (change & IFF_PROMISC)
     dev_set_promiscuity(basedev, dev->flags & IFF_PROMISC ? 1 : -1);
}

static int ethswitch_ethtool_get_settings(struct net_device *dev,
                                     struct ethtool_cmd *cmd)
{
  struct ethswitch_dev *ethswdev  = netdev_priv(dev);

  return __ethtool_get_settings(ethswdev->basedev, cmd);
}

static void ethswitch_ethtool_get_drvinfo(struct net_device *dev,
                                     struct ethtool_drvinfo *info)
{
  strcpy(info->driver, "ethsw" );
  strcpy(info->version, "1.1" );
  strcpy(info->fw_version, "N/A");
}

static const struct ethtool_ops ethswitch_ethtool_ops = {
  .get_link               = ethtool_op_get_link,
  .get_settings           = ethswitch_ethtool_get_settings,
  .get_drvinfo            = ethswitch_ethtool_get_drvinfo,
};

static const struct net_device_ops ethswitch_netdev_ops = {
  .ndo_init               = ethswitch_init,
  .ndo_uninit    = ethswitch_uninit,
  .ndo_open               = ethswitch_open,
  .ndo_stop               = ethswitch_stop,
  .ndo_start_xmit         = ethswitch_start_xmit,
  .ndo_change_mtu         = ethswitch_change_mtu,
  .ndo_change_rx_flags    = ethswitch_change_rx_flags,
  .ndo_set_mac_address    = ethswitch_set_mac_address,
  .ndo_get_stats64  = ethswitch_dev_get_stats64,
  .ndo_validate_addr      = eth_validate_addr,
};

static void ethswitch_setup(struct net_device *dev)
{
  ether_setup(dev);

  dev->netdev_ops         = &ethswitch_netdev_ops;
  dev->ethtool_ops        = &ethswitch_ethtool_ops;
  dev->destructor         = free_netdev;
  dev->tx_queue_len       = 0;
  dev->priv_flags         &= ~IFF_XMIT_DST_RELEASE;
      
  memset(dev->broadcast, 0, ETH_ALEN);

}

static int ethsw_validate(struct nlattr *tb[], struct nlattr *data[])
{
  unsigned long port = 1;
  char *e;
  char ports[16]="";

  if (tb[IFLA_ADDRESS]) {
     if (nla_len(tb[IFLA_ADDRESS]) != ETH_ALEN)
         return -EINVAL;
     if (!is_valid_ether_addr(nla_data(tb[IFLA_ADDRESS])))
         return -EADDRNOTAVAIL;
  }

  if (!data)
     return -EINVAL;
        
  if (data[IFLA_ETHSW_PORTS]) {
      nla_strlcpy(ports,data[IFLA_ETHSW_PORTS],16);
      port = simple_strtoul(ports,&e,10);
      if (port > MAX_PORT_NUMBER )
        return -ERANGE;
      if ( portdev[port] != NULL )
        return -EINVAL;
  }
  return 0;
}

struct ethsw_port {
  struct net_device  *dev;
  int      count;
};

static int ethsw_port_create(struct net_device *dev)
{
  int err;
  if (dev->type != ARPHRD_ETHER || dev->flags & IFF_LOOPBACK)
    return -EINVAL;

  err = netdev_rx_handler_register(dev, ethsw_handle_frame, NULL);
  return err;
}

static int ethswitch_newlink(struct net *src_net, struct net_device *dev,
                           struct nlattr *tb[], struct nlattr *data[])
{
  struct ethswitch_dev *ethswdev = netdev_priv(dev);
  struct net_device *basedev;
  int err;
  int port = -1;
  char *e;
  char ports[16]="";

  if (!tb[IFLA_LINK])
     return -EINVAL;

  basedev = __dev_get_by_index(src_net, nla_get_u32(tb[IFLA_LINK]));
  if (basedev == NULL)
     return -ENODEV;

  if (!data) 
     return -EINVAL;

  if (data[IFLA_ETHSW_PORTS]) {
    nla_strlcpy(ports,data[IFLA_ETHSW_PORTS],16);
    port = simple_strtoul(ports,&e,10);
  }
    
  if (!tb[IFLA_MTU])
     dev->mtu = basedev->mtu;
  else if (dev->mtu > basedev->mtu)
     return -EINVAL;

  if (data[IFLA_ETHSW_PORTS]) {
    portdev[port] = ethswdev;
  }
    
  ethswdev->basedev = basedev;
  ethswdev->dev     = dev;
    
  if ( tb[IFLA_IFNAME] )
     nla_strlcpy(dev->name, tb[IFLA_IFNAME], IFNAMSIZ);

  if (newlink == 0) {
    err = ethsw_port_create(basedev);
    if (err < 0)
      return err;
  }
  err = netdev_upper_dev_link(basedev, dev);
  if (err)
    goto destroy_port;
        err = register_netdevice(dev);
        if ( err < 0 ) {
    goto upper_dev_unlink;
  }
  newlink++;
#if defined(CONFIG_LTQ_PPA_DATAPATH) || defined(CONFIG_LTQ_PPA_DATAPATH_MODULE)
  ppa_hook_set_lan_seperate_flag_fn(1);
#endif
        
  netif_stacked_transfer_operstate(basedev, dev);

  return 0;

destroy_port:
  newlink--;
  if (newlink == 0)
    netdev_rx_handler_unregister(dev);
upper_dev_unlink:
  netdev_upper_dev_unlink(basedev, dev);

  return err;
}


static void ethswitch_dellink(struct net_device *dev, struct list_head *head)
{
  struct ethswitch_dev *ethswdev = netdev_priv(dev);
  struct net_device *basedev = ethswdev->basedev;
  int i = 0;
  for( i=0; i<MAX_PORT_NUMBER; i++) {
    if ( portdev[i] != NULL) {
      if ( memcmp(portdev[i]->dev->name,dev->name,strlen(dev->name)) == 0) {
        portdev[i] = NULL;
        break;
      }
    }
  }
    
  unregister_netdevice(dev);
  netdev_upper_dev_unlink(basedev, dev);
    
  newlink--;
  if ( newlink == 0 ) {
    netdev_rx_handler_unregister(basedev);
#if defined(CONFIG_LTQ_PPA_DATAPATH) || defined(CONFIG_LTQ_PPA_DATAPATH_MODULE)
    ppa_hook_set_lan_seperate_flag_fn(0);
#endif
  }
}

static struct rtnl_link_ops ethsw_link_ops = {
  .kind           = "ethsw",
  .maxtype        = IFLA_ETHSW_MAX,
  .policy         = ethsw_policy,
  .priv_size      = sizeof(struct ethswitch_dev),
  .setup          = ethswitch_setup,
  .validate       = ethsw_validate,
  .newlink        = ethswitch_newlink,
  .dellink        = ethswitch_dellink,
};

/*
 * init/fini
 */

static __init int ethswitch_init_module(void)
{
    int err;
    int port=0;
    
    while( port < MAX_PORT_NUMBER) {
      portdev[port] = NULL;
      ++port;
    }
    
    err = rtnl_link_register(&ethsw_link_ops);
    if ( err < 0 )
      return err;
    return 0;
    
}

static __exit void ethswitch_cleanup_module(void)
{
        
    rtnl_link_unregister(&ethsw_link_ops);
    
}

module_init(ethswitch_init_module);
module_exit(ethswitch_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_ALIAS_RTNL_LINK("ethsw");
MODULE_DESCRIPTION("Ethernet Switch Port Virtual Interface");
MODULE_AUTHOR("Kamal Eradath"); 
