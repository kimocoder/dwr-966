
/*
 * AMAZON_S internal switch ethernet driver.
 *
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <common.h>


#include <config.h>
#include <malloc.h>
#include <net.h>
#include <asm/vr9.h>
#include <asm/addrspace.h>
#include <asm/pinstrap.h>

#ifdef CONFIG_BOOT_FROM_NAND
#include <nand.h>
#endif

#ifdef CONFIG_BOOT_FROM_SPI
#include <spi_flash.h>
#endif


#include <lzma/LzmaTypes.h>
#include <lzma/LzmaDec.h>
#include <lzma/LzmaTools.h>


#define TX_CHAN_NO   1
#define RX_CHAN_NO   0

#define NUM_RX_DESC	PKTBUFSRX
#define NUM_TX_DESC	8
#define MAX_PACKET_SIZE 	1536
#define TOUT_LOOP	100


#define RGMII 0
#define GMII  1
#define TMII  2
#define MII   3
#define RMII  4
				
#define CONFIG_VR9_SW_PORT(NUM)   CONFIG_VR9_SW_PORT_##NUM

#define MDIO_WRITE_CMD  ((0 << 11)| (1 <<10))
#define MDIO_READ_CMD  ((1 << 11) | (0<<10))

extern void flush_dcache_range(unsigned long start, unsigned long stop);
extern void clean_dcache_range(unsigned long start, unsigned long stop);
extern void invalidate_dcache_range(unsigned long start, unsigned long stop);
extern void flush_dcache(void);
extern void invalidate_dcache(void);
extern void invalidate_icache(void);


typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN                 :1;
			volatile u32 C	                 :1;
			volatile u32 Sop                 :1;
			volatile u32 Eop	         :1;
			volatile u32 reserved		 :3;
			volatile u32 Byteoffset		 :2;
			volatile u32 reserve             :7;
			volatile u32 DataLen             :16;
		}field;

		volatile u32 word;
	}status;

	volatile u32 DataPtr;
} dma_rx_descriptor_t;

typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN                 :1;
			volatile u32 C	                 :1;
			volatile u32 Sop                 :1;
			volatile u32 Eop	         :1;
			volatile u32 Byteoffset		 :5;
			volatile u32 reserved            :7;
			volatile u32 DataLen             :16;
		}field;

		volatile u32 word;
	}status;

	volatile u32 DataPtr;
} dma_tx_descriptor_t;

typedef struct
{
    int on;
	int miimode;
	int miirate;
}vr9_sw_port;


static dma_rx_descriptor_t rx_des_ring[NUM_RX_DESC] __attribute__ ((aligned(8)));
static dma_tx_descriptor_t tx_des_ring[NUM_TX_DESC] __attribute__ ((aligned(8)));
static vr9_sw_port vr9_port[7];
static int tx_num, rx_num;
#if CONFIG_VR9_SW_PORT_2 | CONFIG_VR9_SW_PORT_3 | CONFIG_VR9_SW_PORT_4 | CONFIG_VR9_SW_PORT_5b 
static u8 gphy_fw[65536] __attribute__ ((aligned(32)));
#endif

int vr9_switch_init(struct eth_device *dev, bd_t * bis);
int vr9_switch_send(struct eth_device *dev, volatile void *packet,int length);
int vr9_switch_recv(struct eth_device *dev);
void vr9_switch_halt(struct eth_device *dev);
static void vr9_sw_chip_init();
static void vr9_dma_init(void);

u16 vr9_mdio_init()
{
    *BSP_GPIO_P2_ALTSEL0 = *BSP_GPIO_P2_ALTSEL0 | (0xc00);
    *BSP_GPIO_P2_ALTSEL1 = *BSP_GPIO_P2_ALTSEL1 & ~(0xc00);
    *BSP_GPIO_P2_DIR = *BSP_GPIO_P2_DIR  | 0x800;
    *BSP_GPIO_P2_OD = *BSP_GPIO_P2_OD | 0xc00;
}
	
u16 vr9_mdio_read(u8 phyaddr, u8 phyreg)
{
        u16 i=0;
        while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000);
        REG32(VR9_ETHSW_MDIO_CTRL) = MDIO_READ_CMD | (((u32)phyaddr)<<5) | ((u32) phyreg)|0x1000 ;
        while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000)
        {
        	i++;
        	if(i>0x7fff)
        	{
        		  printf("MDIO access time out!\n");
        		  break;
        	}
        }
        return((u16) (REG32(VR9_ETHSW_MDIO_READ)));
}

void vr9_mdio_write(u8 phyaddr, u8 phyreg, u16 data)
{
        u16 i=0;
        
        REG32(VR9_ETHSW_MDIO_WRITE)= data;
        while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000);
        REG32(VR9_ETHSW_MDIO_CTRL) =  MDIO_WRITE_CMD | (((u32)phyaddr)<<5) | ((u32) phyreg)|0x1000 ;
        while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000)
        {
        	i++;
        	if(i>0x7fff)
        	{
        		  printf("MDIO access time out!\n");
        		  break;
        	}
        }
}
					


int vr9_eth_initialize(bd_t * bis)
{
	struct eth_device *dev;

	if (!(dev = (struct eth_device *) malloc (sizeof *dev)))
        {
                printf("Failed to allocate memory\n");
                return 0;
        }
        memset(dev, 0, sizeof(*dev));

        asm("sync");
        vr9_sw_chip_init();
        asm("sync");
	/***************/
	sprintf(dev->name, "vr9 Switch");
	dev->init = vr9_switch_init;
	dev->halt = vr9_switch_halt;
	dev->send = vr9_switch_send;
	dev->recv = vr9_switch_recv;

	eth_register(dev);


	return 1;
}

int vr9_switch_init(struct eth_device *dev, bd_t * bis)
{
	int i;
	tx_num=0;
	rx_num=0;

		/* Reset DMA
		 */

    *BSP_DMA_CS=RX_CHAN_NO;
    *BSP_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
    *BSP_DMA_CPOLL= 0x80000040;
    /*set descriptor base*/
    *BSP_DMA_CDBA=(u32)rx_des_ring;
    *BSP_DMA_CDLEN=NUM_RX_DESC;
    *BSP_DMA_CIE = 0;
    *BSP_DMA_CCTRL=0x30000;

    *BSP_DMA_CS=TX_CHAN_NO;
    *BSP_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
    *BSP_DMA_CPOLL= 0x80000040;
    *BSP_DMA_CDBA=(u32)tx_des_ring;
    *BSP_DMA_CDLEN=NUM_TX_DESC;
    *BSP_DMA_CIE = 0;
    *BSP_DMA_CCTRL=0x30100;

	for(i=0;i < NUM_RX_DESC; i++)
	{
		dma_rx_descriptor_t * rx_desc = KSEG1ADDR(&rx_des_ring[i]);
		rx_desc->status.word=0;
		rx_desc->status.field.OWN=1;
		rx_desc->status.field.DataLen=PKTSIZE_ALIGN;   /* 1536  */
		rx_desc->DataPtr=(u32)NetRxPackets[i] & 0x3ffffff0;
		invalidate_dcache_range((u32)&rx_des_ring[i],(u32)&rx_des_ring[i]+8);
	}

	
	for(i=0;i < NUM_TX_DESC; i++)
	{
		dma_tx_descriptor_t * tx_desc = KSEG1ADDR(&tx_des_ring[i]);
		memset(tx_desc, 0, sizeof(tx_des_ring[0]));
	}
		/* turn on DMA rx & tx channel
		 */
    *BSP_DMA_CS=RX_CHAN_NO;
    *BSP_DMA_CCTRL|=1;/*reset and turn on the channel*/
	return 0;
}

void vr9_switch_halt(struct eth_device *dev)
{
    int i;
        for(i=0;i<8;i++)
	{
	   *BSP_DMA_CS=i;
	   *BSP_DMA_CCTRL&=~1;/*stop the dma channel*/
	}
}

int vr9_switch_send(struct eth_device *dev, volatile void *packet,int length)
{

	int                 	i;
	int 		 	res = -1;
	dma_tx_descriptor_t * tx_desc= KSEG1ADDR(&tx_des_ring[tx_num]);

	if (length <= 0)
	{
		printf ("%s: bad packet size: %d\n", dev->name, length);
		goto Done;
	}

	for(i=0; tx_desc->status.field.OWN==1; i++)
	{
		if(i>=TOUT_LOOP)
		{
			printf("NO Tx Descriptor...");
			goto Done;
		}
	}

	//serial_putc('s');

	tx_desc->status.field.Sop=1;
	tx_desc->status.field.Eop=1;
	tx_desc->status.field.C=0;
	//tx_desc->DataPtr = (u32)KSEG1ADDR(packet);
	tx_desc->DataPtr = (u32)packet & 0x0ffffff0;
        if(length<60)
		tx_desc->status.field.DataLen = 60;
	else
		tx_desc->status.field.DataLen = (u32)length;
        
	    flush_dcache_range((u32)packet,(u32)packet+(u32)tx_desc->status.field.DataLen);	
	asm("SYNC");

	tx_desc->status.field.OWN=1;

	res=length;
	tx_num++;
        if(tx_num==NUM_TX_DESC) tx_num=0;
	*BSP_DMA_CS=TX_CHAN_NO;

	if(!(*BSP_DMA_CCTRL & 1))
	*BSP_DMA_CCTRL|=1;

Done:
	return res;
}

int vr9_switch_recv(struct eth_device *dev)
{

	int                    length  = 0;
	dma_rx_descriptor_t * rx_desc;
	for (;;)
	{
	    rx_desc = KSEG1ADDR(&rx_des_ring[rx_num]);
               
		if ((rx_desc->status.field.C == 0) || (rx_desc->status.field.OWN == 1))
		{
          break;
		}

		length = rx_desc->status.field.DataLen;
		if (length)
		{
            
			invalidate_dcache_range((u32)NetRxPackets[rx_num], (u32)NetRxPackets[rx_num]+length);
			NetReceive((void*)KSEG1ADDR(NetRxPackets[rx_num]), length - 4);
		}
		else
		{
			printf("Zero length!!!\n");
		}
		rx_desc->status.field.Sop=0;
		rx_desc->status.field.Eop=0;
		rx_desc->status.field.C=0;
		rx_desc->status.field.DataLen=PKTSIZE_ALIGN;
		rx_desc->status.field.OWN=1;
		rx_num++;
		if(rx_num==NUM_RX_DESC) rx_num=0;
	}
	return length;
}


static void vr9_dma_init(void)
{
    *BSP_PMU_PWDCR &=~(BSP_PMU_DMA);/*enable DMA from PMU*/

    /* Reset DMA
		 */
    *BSP_DMA_CTRL|=1;
    *BSP_DMA_IRNEN=0;/*disable all the interrupts first*/

    /* Clear Interrupt Status Register
	*/
    *BSP_DMA_IRNCR=0xfffff;
    /*disable all the dma interrupts*/
    *BSP_DMA_IRNEN=0;
	/*disable channel 0 and channel 1 interrupts*/
    
    *BSP_DMA_CS=RX_CHAN_NO;
    *BSP_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
    *BSP_DMA_CPOLL= 0x80000040;
     /*set descriptor base*/
    *BSP_DMA_CDBA=(u32)rx_des_ring;
    *BSP_DMA_CDLEN=NUM_RX_DESC;
    *BSP_DMA_CIE = 0;
    *BSP_DMA_CCTRL=0x30000;

    *BSP_DMA_CS=TX_CHAN_NO;
    *BSP_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
    *BSP_DMA_CPOLL= 0x80000040;
    *BSP_DMA_CDBA=(u32)tx_des_ring;
    *BSP_DMA_CDLEN=NUM_TX_DESC;
    *BSP_DMA_CIE = 0;
    *BSP_DMA_CCTRL=0x30100;
     /*enable the poll function and set the poll counter*/
     //*AMAZON_S_DMA_CPOLL=AMAZON_S_DMA_POLL_EN | (AMAZON_S_DMA_POLL_COUNT<<4);
     /*set port properties, enable endian conversion for switch*/
     __asm__("sync");
    *BSP_DMA_PS=0;
    //*BSP_DMA_PCTRL = 0x1f28;
    *BSP_DMA_PCTRL = 0x1f68;/*fix me, should I enable dma drop?*/
	//*BSP_DMA_PCTRL|=0xf<<8;/*enable 32 bit endian conversion*/
    //*BSP_DMA_PCTRL=0x1f14;
    __asm__("sync");
    return;
}

void vr9_sw_gpio_configure(u8 port, u8 mode)
{
    switch(port)
	{
      case 0:
         if ((mode  == MII) || (mode == RMII))
		 {
		    *BSP_GPIO_P2_ALTSEL0 = *BSP_GPIO_P2_ALTSEL0 | (0x0303);
		    *BSP_GPIO_P2_ALTSEL1 = *BSP_GPIO_P2_ALTSEL1 & ~(0x0303);
		    *BSP_GPIO_P2_DIR     = (*BSP_GPIO_P2_DIR & ~(0x0303)) | 0x0100;
		    *BSP_GPIO_P2_OD      = *BSP_GPIO_P2_OD | 0x0100;
		 }
	     break;

      case 1:
         if ((mode  == MII) || (mode == RMII))
         {
             *BSP_GPIO_P2_ALTSEL0 = *BSP_GPIO_P2_ALTSEL0 | (0xf000);
             *BSP_GPIO_P2_ALTSEL1 = *BSP_GPIO_P2_ALTSEL1 & ~(0xf000);
             *BSP_GPIO_P2_DIR     = (*BSP_GPIO_P2_DIR & ~(0xf000))|0x2000;
             *BSP_GPIO_P2_OD      = *BSP_GPIO_P2_OD | 0x2000;
         }
													 
	     break;
    
      default:
	     break;
	}
}

void config_mdio_gpio()
{
	  *BSP_GPIO_P2_ALTSEL0 = *BSP_GPIO_P2_ALTSEL0 | (0xc00);
    *BSP_GPIO_P2_ALTSEL1 = *BSP_GPIO_P2_ALTSEL1 & ~(0xc00);
    *BSP_GPIO_P2_DIR     |= 0xc00;
    *BSP_GPIO_P2_OD      |= 0xc00;
	
	
}

void config_port()
{
      int i;
	  for(i=0;i<7;i++)
	    vr9_port[i].on=0;

#ifdef CONFIG_VR9_SW_PORT_0
        vr9_port[0].on=1;
        vr9_port[0].miimode=CONFIG_VR9_SW_PORT0_MIIMODE;
        vr9_port[0].miirate=CONFIG_VR9_SW_PORT0_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_1
        vr9_port[1].on=1;
		vr9_port[1].miimode=CONFIG_VR9_SW_PORT1_MIIMODE;
		vr9_port[1].miirate=CONFIG_VR9_SW_PORT1_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_2
	    vr9_port[2].on=1;
		vr9_port[2].miimode=CONFIG_VR9_SW_PORT2_MIIMODE;
		vr9_port[2].miirate=CONFIG_VR9_SW_PORT2_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_3
        vr9_port[3].on=1;
		vr9_port[3].miimode=CONFIG_VR9_SW_PORT3_MIIMODE;
		vr9_port[3].miirate=CONFIG_VR9_SW_PORT3_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_4
	    vr9_port[4].on=1;
		vr9_port[4].miimode=CONFIG_VR9_SW_PORT4_MIIMODE;
		vr9_port[4].miirate=CONFIG_VR9_SW_PORT4_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_5a
        vr9_port[5].on=1;
		vr9_port[5].miimode=CONFIG_VR9_SW_PORT5a_MIIMODE;
		vr9_port[5].miirate=CONFIG_VR9_SW_PORT5a_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_5b
	    vr9_port[5].on=1;
		vr9_port[5].miimode=CONFIG_VR9_SW_PORT5b_MIIMODE;
		vr9_port[5].miirate=CONFIG_VR9_SW_PORT5b_MIIRATE;
#endif
											  

    return;
}

/***obtain lzma compressed gphy firmware from src address***/
/** return value: 0 success, 1 fail*************************/
int get_gphy_firmware(u32 dst, u32 src)
{
     int result = 1;
     int srcLen;
	 u8* load = (unsigned char*)0xa0140000;
	 ulong unc_len = 0;
	 u8* image_start = NULL;
	 ulong image_len = 0;
     image_header_t *pimg_header = NULL;
	 int fw_version = 1; /*1->version 1; 2->version 2*/
	 int fw_type = 0; /*0 FE, 1 GE*/
     int chip_version = 0;
	 char* ep;
    
	 if (((ep = getenv("raw_gphy_fw")) != NULL) && (strcmp(ep, "yes") == 0)) {
         memcpy(dst,src,65536);
         result = 0;
		 goto exit;
	 }

     pimg_header = (image_header_t *)src;	
     while(1){ 
	    if(pimg_header->ih_magic != IH_MAGIC){
              printf("bad image magic number\n");
              result = 1;
              break;

	     }
	 switch(pimg_header->ih_type){
	   case IH_TYPE_MULTI:
         image_start = (u8*)pimg_header+sizeof(image_header_t)+8;
	     image_len = pimg_header->ih_size; 

	     if(lzmaBuffToBuffDecompress(load, &unc_len,image_start, image_len)){
			 printf("gphy image name incorrect...\n");
		     result = 1;
		     goto exit;
		     }
		  pimg_header = (image_header_t *)load;  
	    break;
      
	   case IH_TYPE_FIRMWARE:
          if(strncmp(pimg_header->ih_name, "VR9 V1.1 GPHY GE", sizeof(pimg_header->ih_name)) == 0){
                  fw_version = 1;
				  fw_type = 1;
		  }else if(strncmp(pimg_header->ih_name, "VR9 V1.1 GPHY FE", sizeof(pimg_header->ih_name)) == 0){
                  fw_version = 1;
				  fw_type = 0;
		  }else if(strncmp(pimg_header->ih_name, "VR9 V1.2 GPHY GE", sizeof(pimg_header->ih_name)) == 0){
		          fw_version = 2;
		          fw_type = 1;
		  }else if(strncmp(pimg_header->ih_name, "VR9 V1.2 GPHY FE", sizeof(pimg_header->ih_name)) == 0){
		          fw_version = 2;
		          fw_type = 0;
		  }
 
#ifdef CONFIG_VRX220
          chip_version = 2;
#else
          chip_version= (REG32(BSP_MPS_CHIPID)>>28) & 0x7;
#endif
		  if(chip_version == fw_version && fw_type==CONFIG_VR9_INTERNAL_PHY_TYPE){ 
                    image_start = (u8*)pimg_header+sizeof(image_header_t);
					image_len = pimg_header->ih_size;
					memcpy((u8*)dst, image_start, image_len);
					result = 0;
					goto exit;
          }                 
		  pimg_header = (u8*)pimg_header+sizeof(image_header_t)+(pimg_header->ih_size);
	      break;
		 
	  }
	 }
    	
exit:	 
     return result;
}


static void vr9_sw_chip_init()
{
      int i,num;
	  u32 fw_addr,gphy_reset_value,fw_src_addr; 
	  u8 interface_mode;
	 
	
#if (CONFIG_VR9_SW_PORT_2 | CONFIG_VR9_SW_PORT_3 | CONFIG_VR9_SW_PORT_4 | CONFIG_VR9_SW_PORT_5b)
#ifdef CONFIG_VR9_GPHY_DEBUG

    /*GPHY0_UTX*/
    REG32(BSP_GPIO_P0_ALTSEL0)|=1<<13;
    REG32(BSP_GPIO_P0_ALTSEL1)|=1<<13;
    REG32(BSP_GPIO_P0_DIR)|=1<<13;
    

      /*GPHY1_UTX, p1.14*/
	  REG32(BSP_GPIO_P1_ALTSEL0)|=1<<14;
	  REG32(BSP_GPIO_P1_ALTSEL1)|=1<<14;
	  REG32(BSP_GPIO_P1_DIR)|=1<<14;
	  REG32(BSP_GPIO_P1_OD)|=1<<14;
				  
	  /*GJTAG_sel*/
	  REG32(BSP_GPIO_P1_ALTSEL0)|=1;
	  REG32(BSP_GPIO_P1_ALTSEL1)|=1;
	  REG32(BSP_GPIO_P1_DIR)&=~1;

      /*TDI p2.1*/
	  REG32(BSP_GPIO_P2_ALTSEL0) &=~1;
	  REG32(BSP_GPIO_P2_ALTSEL1)|=1;
	  REG32(BSP_GPIO_P2_DIR)&=~1;

	  /*TDO, gpio33, P2.1*/
	  REG32(BSP_GPIO_P2_ALTSEL0) &=~(1<<1);
	  REG32(BSP_GPIO_P2_ALTSEL1)|=1<<1;
	  REG32(BSP_GPIO_P2_DIR) |=1<<1;
	  REG32(BSP_GPIO_P2_OD) &=~(1<<1); /*to be checked, affecting debugging speed*/

	  /*TMS, p2.9*/
    REG32(BSP_GPIO_P2_ALTSEL0) |=1<<9;
	  REG32(BSP_GPIO_P2_ALTSEL1)|=1<<9;
	  REG32(BSP_GPIO_P2_DIR) &=~(1<<9);
				  
	  /*TCK, p2.8*/
	  REG32(BSP_GPIO_P2_ALTSEL0) |=1<<8;
	  REG32(BSP_GPIO_P2_ALTSEL1)|=1<<8;
	  REG32(BSP_GPIO_P2_DIR) &=~(1<<8);
	  
#if 0
	  /*Configure LED for GPHY*/
	  /*gpio2, p0.2, gphy led 2*/
	  REG32(BSP_GPIO_P0_ALTSEL0)|=1<<2;
    REG32(BSP_GPIO_P0_ALTSEL1)|=1<<2;
    REG32(BSP_GPIO_P0_DIR)|=1<<2;
    REG32(BSP_GPIO_P0_DIR)|=1<<2;
	  
	  /*gpio5, p0.5, gph0 led0*/
	  REG32(BSP_GPIO_P0_ALTSEL0)&=~(1<<5);
    REG32(BSP_GPIO_P0_ALTSEL1)|=1<<5;
    REG32(BSP_GPIO_P0_DIR)|=1<<5;
    REG32(BSP_GPIO_P0_DIR)|=1<<5;
    
    /*gpio7, p0.7, gph0 led1*/
	  REG32(BSP_GPIO_P0_ALTSEL0)|=1<<7;
    REG32(BSP_GPIO_P0_ALTSEL1)|=1<<7;
    REG32(BSP_GPIO_P0_DIR)|=1<<7;
    REG32(BSP_GPIO_P0_DIR)|=1<<7;
    
    /*gpio44, p2.12, gph1 led0*/
	  REG32(BSP_GPIO_P2_ALTSEL0)|=1<<12;
    REG32(BSP_GPIO_P2_ALTSEL1)|=1<<12;
    REG32(BSP_GPIO_P2_DIR)|=1<<12;
    REG32(BSP_GPIO_P2_DIR)|=1<<12;
    
    /*gpio 45, p2.13,gph1 led1*/
    REG32(BSP_GPIO_P2_ALTSEL0)&=~(1<<13);
    REG32(BSP_GPIO_P2_ALTSEL1)|=1<<13;
    REG32(BSP_GPIO_P2_DIR)|=1<<13;
    REG32(BSP_GPIO_P2_DIR)|=1<<13;
    
    /*gpio 47, p2.15, gphy1 led2*/
    REG32(BSP_GPIO_P2_ALTSEL0)&=~(1<<15);
    REG32(BSP_GPIO_P2_ALTSEL1)|=1<<15;
    REG32(BSP_GPIO_P2_DIR)|=1<<15;
    REG32(BSP_GPIO_P2_DIR)|=1<<15;
#endif
#endif

    
#ifdef CONFIG_GPHY_LED_GPIO
    /*GPIO 5 , 7 , 44 , 45 config as GPHY LED , and GPIO7 is CFRAME pin*/ 
    *BSP_GPIO_P0_ALTSEL0 &= (~0x20);
    *BSP_GPIO_P0_ALTSEL1 |= (0x20);
    *BSP_GPIO_P0_DIR |= (0x20);
    *BSP_GPIO_P0_OD |= (0x20);
    *BSP_GPIO_P0_ALTSEL0 |=0x80;
    *BSP_GPIO_P0_ALTSEL1 |=0x80;
    *BSP_GPIO_P0_DIR |= (0x80);
    *BSP_GPIO_P0_DIR |= (0x80);

    *BSP_GPIO_P2_ALTSEL0 |= (0x1000);
    *BSP_GPIO_P2_ALTSEL1 |= (0x1000);
    *BSP_GPIO_P2_DIR |= (0x1000);
    *BSP_GPIO_P2_OD |= (0x1000);
    *BSP_GPIO_P2_ALTSEL0 &=(~0x2000);
    *BSP_GPIO_P2_ALTSEL1 |=0x2000;
    *BSP_GPIO_P2_DIR |= (0x2000);
    *BSP_GPIO_P2_DIR |= (0x2000);  
#else	 /*CONFIG_GPHY_SHIFT_REGISTER*/											
	  /*enable led for gphy link status*/
	  *(BSP_GPIO_P0_DIR) = (*BSP_GPIO_P0_DIR)|(0x070);
	  *(BSP_GPIO_P0_ALTSEL0) = ((*BSP_GPIO_P0_ALTSEL0)|(0x70));
	  *(BSP_GPIO_P0_ALTSEL1) = (*BSP_GPIO_P0_ALTSEL1)&(~0x70);
	  *BSP_GPIO_P0_OD = (*BSP_GPIO_P0_OD)|0x70;
							  
      *BSP_LED_CON1 = 0x81a38007; // all LEDs enable, software update mode, FPI clock for shifting, no delay
	  *BSP_LED_CON0 = 0x3f000000; // software update
#endif					 

	  asm("sync");
    /*turn on ephy, switch and dma in PMU*/
    *BSP_PMU_PWDCR = 0x061592be;
     mdelay(100);

    *BSP_RCU_RST_REQ = 0xa0200300;

	  mdelay(100);
	  
	  /*Turn on switch macro*/  
    REG32(VR9_ETHSW_GLOB_CTRL) |= (1<<15);
    
#ifdef CONFIG_VR9_CRYSTAL_25M
    REG32(BSP_IF_CLK)=0x16e00006; /*GRX mode, Xtal is 25Mhz, so directly use*/
#else /*36Mhz crystal clock*/
    REG32(BSP_IF_CLK)=0x16e00008; /*25Mhz from PLL0 divider*/
  #ifdef CONFIG_VR9_PHY_USE_CLOCK_GPIO3
    /* Configure GPIO3 for CLK25_OUT */
    *BSP_IF_CLK =0x16e000010;
    *BSP_GPIO_P0_ALTSEL0 = *BSP_GPIO_P0_ALTSEL0 | (1<<3);
    *BSP_GPIO_P0_ALTSEL1 = *BSP_GPIO_P0_ALTSEL1 & ~(1<<3);
    #ifdef CONFIG_VR9_PHY_USE_CLOCK_GPIO3_INTERNAL
      *BSP_GPIO_P0_DIR |= (1<<3);  /*take clock from internal CGU, gpio3 set to clk output mode*/
	#else
	  *BSP_GPIO_P0_DIR &=~(1<<3);  /*take clock from external source, gpio3 set to input clock*/
      *BSP_GPIO_P0_OD = *BSP_GPIO_P0_OD | (1<<3);
	#endif  
  #endif
#endif
    
      REG32(BSP_GPHY1_Cfg)=0x1fe70000;
      REG32(BSP_GPHY0_Cfg)=0x1fe70000;/*gphy0 config for 25Mhz input clock only*/
	  mdelay(1);
	  asm("sync");
	  fw_addr=0xa0200000;
#ifdef CONFIG_VR9_GPHY_FW_EMBEDDED	  
      fw_src_addr = CONFIG_VR9_GPHY_FW_ADDR;

	  //memcpy(fw_addr, CONFIG_VR9_GPHY_FW_ADDR, 65536); 
#endif	 
#ifdef CONFIG_VR9_EXTERN_GPHY_FW_ADDR
 #ifdef CONFIG_BOOT_FROM_NOR
       fw_src_addr = getenv("gphy_fw_addr")?simple_strtoul((char *)getenv("gphy_fw_addr"),NULL,16):CONFIG_VR9_EXTERN_GPHY_FW_ADDR;
 #elif defined(CONFIG_BOOT_FROM_NAND)
      {
	   nand_info_t *nand = &nand_info[0];
	   u64 size=131072;
	   fw_src_addr = 0xa0120000;
       nand_read_skip_bad(nand, (getenv("gphy_fw_addr")?simple_strtoul((char *)getenv("gphy_fw_addr"),NULL,16):CONFIG_VR9_EXTERN_GPHY_FW_ADDR), &size,(u_char *)fw_src_addr);
      }
 #elif defined(CONFIG_BOOT_FROM_SPI)
      {
        static struct spi_flash *flash;
		fw_src_addr = 0xa0110000;
	    flash = spi_flash_probe(0, CONFIG_ENV_SPI_CS,
			             CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
	    spi_flash_read(flash, (getenv("gphy_fw_addr")?simple_strtoul((char *)getenv("gphy_fw_addr"),NULL,16):CONFIG_VR9_EXTERN_GPHY_FW_ADDR), 131072, (void*)fw_src_addr);					 

	  }
 #endif 
#endif

      if(get_gphy_firmware(fw_addr,fw_src_addr)){
	           printf("cannnot find gphy firmware!\n");
	    }
					 
	  mdelay(100);
	  /* step - 1 */
	  /* program fw base address */
	  REG32(0xBF203020) = 0x200000;  // address inside DDR1/2
	  REG32(0xBF203068) = 0x200000;  // address inside DDR1/2
      REG32(0xBF203010) = 0xa0000021;
	  mdelay(100);
	  /* step - 2 */

	  // Bring GPHY-0/GFS-0 GPHY-1/GFS-1 out of reset
	  //REG32(0xBF203010) = 0x0;
#ifdef CONFIG_POWER_DOWN_REGULATOR	 
      *BSP_RCU_RST_REQ = 0x04000020;
#else
      *BSP_RCU_RST_REQ = 0x20;
#endif	  
	  
	  mdelay(100); /*wait 200ms*/
      REG32(BSP_SWITCH_PMAC_RX_IPG) = 0x8b;
#endif

	  /*Enable PHY polling for each port*/		
	  REG32(VR9_ETHSW_MDC_CFG_0)=0x3f;
	  /*2.5 MHz MDIO clock*/
#if defined(CONFIG_VR9_CRYSTAL_6M) || defined(CONFIG_VR9_CRYSTAL_CPLD)
      REG32(VR9_ETHSW_MDC_CFG_1)=0x00008102;
#else
      REG32(VR9_ETHSW_MDC_CFG_1)=0x00008105;
#endif
	  udelay(100);
/**************************************************/	  
/***********configure phy now**********************/
/**************************************************/
	 asm("sync");
	 /*config mdio gpio for external gphy only*/ 
	  config_mdio_gpio();
	 /*Configure port 2,4 phy address and auto-polling mode now*/
	  REG32(VR9_ETHSW_PHY_ADDR_2) = 0x1811;
      REG32(VR9_ETHSW_PHY_ADDR_3) = 0x1812;
      REG32(VR9_ETHSW_PHY_ADDR_4) = 0x1813;
#ifdef CONFIG_VR9_SW_PORT_5b    
      REG32(VR9_ETHSW_PHY_ADDR_5) = 0x1814;  
#elif CONFIG_VR9_SW_PORT_5a
      REG32(VR9_ETHSW_PHY_ADDR_5) = 0x1805;  
#endif  
     
	if(((REG32(BSP_MPS_CHIPID)>>28) & 0x7) ==0x1){
/*****disable "clear on read" ability for link status bits**/
    vr9_mdio_write(0x11,0xd,0x1F);
    vr9_mdio_write(0x11,0xe,0x1FF);
    vr9_mdio_write(0x11,0xd,0x401F);
    vr9_mdio_write(0x11,0xe,0x1);
    vr9_mdio_write(0x11,0x14,0x8106);
    vr9_mdio_write(0x11,0x0,0x8000);

    vr9_mdio_write(0x13,0xd,0x1F);
    vr9_mdio_write(0x13,0xe,0x1FF);
    vr9_mdio_write(0x13,0xd,0x401F);
    vr9_mdio_write(0x13,0xe,0x1);
    vr9_mdio_write(0x13,0x14,0x8106);
    vr9_mdio_write(0x13,0x0,0x8000);

    vr9_mdio_write(0x0,0xd,0x1F);
    vr9_mdio_write(0x0,0xe,0x1FF);
    vr9_mdio_write(0x0,0xd,0x401F);
    vr9_mdio_write(0x0,0xe,0x1);
    vr9_mdio_write(0x0,0x14,0x8106);
    vr9_mdio_write(0x0,0x0,0x8000);

    vr9_mdio_write(0x1,0xd,0x1F);
    vr9_mdio_write(0x1,0xe,0x1FF);
    vr9_mdio_write(0x1,0xd,0x401F);
    vr9_mdio_write(0x1,0xe,0x1);
    vr9_mdio_write(0x1,0x14,0x8106);
    vr9_mdio_write(0x1,0x0,0x8000);

    vr9_mdio_write(0x5,0xd,0x1F);
	vr9_mdio_write(0x5,0xe,0x1FF);
	vr9_mdio_write(0x5,0xd,0x401F);
	vr9_mdio_write(0x5,0xe,0x1);
	vr9_mdio_write(0x5,0x14,0x8106);
	vr9_mdio_write(0x5,0x0,0x8000);
}

/***enable sticky bit handling for external phy*********/
    vr9_mdio_write(0x0,0x14,0x8102);
    vr9_mdio_write(0x1,0x14,0x8102);
    vr9_mdio_write(0x5,0x14,0x8102);

/**advertise on half and full duplex mode, multi device*/
    vr9_mdio_write(0x0,0x9,0x700);
    vr9_mdio_write(0x1,0x9,0x700);
    vr9_mdio_write(0x5,0x9,0x700);
    vr9_mdio_write(0x11,0x9,0x700);
    vr9_mdio_write(0x13,0x9,0x700);

/*configuration for LED status*/
   {
    u8 phyaddr[5]={0x0,0x1,0x5,0x11,0x13};

	for(i=0;i<5;i++)
	{
     /*For LED0    (SPEED/LINK INDICATION ONLY)*/
     REG32(VR9_ETHSW_MDC_CFG_0)=0x0;
     vr9_mdio_write(phyaddr[i],0xd,0x1F);
	 vr9_mdio_write(phyaddr[i],0xe,0x1e2);
     vr9_mdio_write(phyaddr[i],0xd,0x401f);
	 vr9_mdio_write(phyaddr[i],0xe,0x42);
	 REG32(VR9_ETHSW_MDC_CFG_0)=0x3f;

     REG32(VR9_ETHSW_MDC_CFG_0)=0x0;
     vr9_mdio_write(phyaddr[i],0xd,0x1F);
     vr9_mdio_write(phyaddr[i],0xe,0x1e3);
     vr9_mdio_write(phyaddr[i],0xd,0x401f);
     vr9_mdio_write(phyaddr[i],0xe,0x10);
     REG32(VR9_ETHSW_MDC_CFG_0)=0x3f;
	
	 /*For LED1 (DATA TRAFFIC INDICATION ONLY)*/
     REG32(VR9_ETHSW_MDC_CFG_0)=0x0;
	 vr9_mdio_write(phyaddr[i],0xd,0x1f);
	 vr9_mdio_write(phyaddr[i],0xe,0x1e4);
	 vr9_mdio_write(phyaddr[i],0xd,0x401f);
	 vr9_mdio_write(phyaddr[i],0xe,0x70);
	 REG32(VR9_ETHSW_MDC_CFG_0)=0x3f;

	 REG32(VR9_ETHSW_MDC_CFG_0)=0x0;
	 vr9_mdio_write(phyaddr[i],0xd,0x1f);
	 vr9_mdio_write(phyaddr[i],0xe,0x1e5);
	 vr9_mdio_write(phyaddr[i],0xd,0x401f);
	 vr9_mdio_write(phyaddr[i],0xe,0x03); 
	 REG32(VR9_ETHSW_MDC_CFG_0)=0x3f;
	}
   }	

/**************************************/
/*end of phy configuration*************/
/**************************************/

/*************************************/
/***mac configuration now*************/
/*************************************/
	  config_port();
  
	  for(num=0;num<7;num++){
        if(vr9_port[num].on){
        //printf("port%d init!\n",num);
		    REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) |= (1<<VR9_ETHSW_MII_CFG_RES);  /*Hardware reset */
        for (i=0; i<100;i++)
          udelay(10);

        /* set xMII port interface clock rate */
        REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) &= ~(0x7<<VR9_ETHSW_MII_CFG_MIIRATE);
        REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) |= ((vr9_port[num].miirate & 0x7)<<VR9_ETHSW_MII_CFG_MIIRATE);
															 
        /*xMII  interface mode */
        //interface_mode = 4;
	    REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) &= ~(0xf<<VR9_ETHSW_MII_CFG_MIIMODE);
	    REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) |= ((vr9_port[num].miimode & 0xf)<<VR9_ETHSW_MII_CFG_MIIMODE);
						  
        /*xMII  interface enable*/
	    REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) |= (1<<VR9_ETHSW_MII_CFG_EN);
	    
		    
        REG16(VR9_ETHSW_MAC_CTRL_0 + ( num * 0x30))= 0x180; /*auto MII/GMII interface selection, enable padding*/
        
        REG16(VR9_ETHSW_MAC_CTRL_2 + ( num * 0x30))= 0x0d; /*JUMBO configured value, including any number of VLAN tags*/
       }
     }
    asm("sync");  


	   /*Force clk stop capability*/
     
	   REG32(VR9_ETHSW_ANEG_EEE_0) = 0x0C;
	   REG32(VR9_ETHSW_ANEG_EEE_1) = 0x0C;
	   REG32(VR9_ETHSW_ANEG_EEE_2) = 0x0C;
	   REG32(VR9_ETHSW_ANEG_EEE_3) = 0x0C;
	   REG32(VR9_ETHSW_ANEG_EEE_4) = 0x0C;
	   REG32(VR9_ETHSW_ANEG_EEE_5) = 0x0C;
	 
	 // Apply workaround for buffer congestion
		REG32(VR9_ETHSW_MAC_CTRL_1 + ( 6* 0x30)) |= ((1<< 0x8)); // Short Preamble
		REG32(VR9_ETHSW_MAC_CTRL_1 + ( 6* 0x30)) &= ~((0xF)); // Setting TX IPG
		REG32(VR9_ETHSW_MAC_CTRL_1 + ( 6* 0x30)) |= ((0xC));  // to 7 bytes
    	REG32(VR9_ETHSW_MAC_CTRL_6 + ( 6* 0x30)) |= (1 << 6); // Setting RX Buffer to bypass

		// Limiting global threshholds to 254 to avoid systematical concept weakness ACM bug
		REG32(VR9_ETHSW_BM_FSQM_GCTRL) &= (0xFD);
		REG32(VR9_ETHSW_BM_GCTRL) |= (0x01);
		REG32(VR9_ETHSW_BM_GCTRL) &= (0xFFFE);

        /*Fix switch MAC drop bug*/ 
        REG32(ADR_ETHSW_PCE_PDI_PCE_TBL_KEY_0) = 0xf;
		REG32(ADR_ETHSW_PCE_PDI_PCE_TBL_VAL_0) = 0x40;
        REG32(ADR_ETHSW_PCE_PDI_PCE_TBL_ADDR)  = 0x3;
		REG32(ADR_ETHSW_PCE_PDI_PCE_TBL_CTRL)  = 0x902f;
   
		REG32(0xBE10a530) = 0x107;
		REG32(0xBE10a544) |= 0x40;
		
		REG32(VR9_ETHSW_PMAC_HD_CTL) |= (0x1 << 2);	

      /*To avoid the packet drop issue, please set the TXDLY bits of PCDU0, 1 , 5 to 1.*/
      REG32(VR9_ETHSW_PCDU_0)=(REG32(VR9_ETHSW_PCDU_0) & (~0x7)) | 0x1;
      REG32(VR9_ETHSW_PCDU_1)=(REG32(VR9_ETHSW_PCDU_1) & (~0x7)) | 0x1;
	  REG32(VR9_ETHSW_PCDU_5)=(REG32(VR9_ETHSW_PCDU_5) & (~0x7)) | 0x1;
	  
	  /*Switch MACBAT WorkAround*/
	  /*Set PCE_TBL_KEY_0 (0x447) to 0xF*/
      REG32(0xBE10911C) = 0xF;

      /*Set PCE_TBL_VAL_0 (0x44D) to 0x40*/
      REG32(0xBE109134) = 0x40;

      /*Set PCE_TBL_ADDR (0x44E) to 0x3*/
      REG32(0xBE109138) = 0x3;

      /*Set PCE_TBL_CTRL (0x44F) to 0x902F*/
      REG32(0xBE10913C) = 0x902F;

   
	  REG32(0xbf203048) = 0xc0;
	  mdelay(200);
	  REG32(0xbf203048) = 0x0;
	  mdelay(200);
      asm("sync");  

#if (CONFIG_VR9_SW_PORT_2 | CONFIG_VR9_SW_PORT_3 | CONFIG_VR9_SW_PORT_4 | CONFIG_VR9_SW_PORT_5b)
  #ifdef CONFIG_VR9_SW_PORT2_MII
     printf("Internal phy(FE) firmware version: 0x%04x\n",vr9_mdio_read(0x11, 0x1e));
  #else
     printf("Internal phy(GE) firmware version: 0x%04x\n",vr9_mdio_read(0x11, 0x1e));
  #endif
#endif	
	  vr9_dma_init();
}



