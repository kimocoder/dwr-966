/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 *  Copyright (C) 2013 Lei Chuanhua <chuanhua.lei@lantiq.com>
 */

#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/pm.h>
#include <linux/export.h>
#include <linux/delay.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/reset-controller.h>

#include <linux/kernel.h>

#include <asm/reboot.h>

#include <lantiq_soc.h>

#include "../prom.h"

/* reset request register */
#define RCU_RST_REQ		0x0010
/* reset status register */
#define RCU_RST_STAT		0x0014
/* vr9 gphy registers */
#define RCU_GFS_ADD0_XRX200	0x0020
#define RCU_GFS_ADD1_XRX200	0x0068

#define RCU_AHB_ENDIAN		0x004C
#define RCU_VR9_BE_AHB1S	0x00000008

/* reboot bit */
#define RCU_RD_GPHY0_XRX200	BIT(31)
#define RCU_RD_SRST		BIT(30)
#define RCU_RD_GPHY1_XRX200	BIT(29)

#define RCU_RST_REQ_MIPS0 BIT(1)

/* xRX300 bits */
#define RCU_RD_GPHY0_XRX300	BIT(31)
#define RCU_RD_GPHY1_XRX300	BIT(29)
#define RCU_RD_GPHY2_XRX300	BIT(28)

/* xRX330 bits */
#define RCU_RD_GPHY0_XRX330	BIT(31)
#define RCU_RD_GPHY1_XRX330	BIT(29)
#define RCU_RD_GPHY2_XRX330	BIT(28)
#define RCU_RD_GPHY3_XRX330	BIT(10)

/* xRX300 gphy registers */
#define RCU_GFS_ADD0_XRX300	0x0020
#define RCU_GFS_ADD1_XRX300	0x0058
#define RCU_GFS_ADD2_XRX300	0x00AC

/* xRX330 gphy registers */
#define RCU_GFS_ADD0_XRX330	0x0020
#define RCU_GFS_ADD1_XRX330	0x0058
#define RCU_GFS_ADD2_XRX330	0x00AC
#define RCU_GFS_ADD3_XRX330	0x0264

/* reset cause */
#define RCU_STAT_SHIFT		26
/* boot selection */
#define RCU_BOOT_SEL(x)		((x >> 18) & 0x7)
#define RCU_BOOT_SEL_XRX200(x)	(((x >> 17) & 0xf) | ((x >> 8) & 0x10))

/* remapped base addr of the reset control unit */
static void __iomem *ltq_rcu_membase;
static struct device_node *ltq_rcu_np;
static DEFINE_SPINLOCK(ltq_rcu_lock);

/* This function is used by the watchdog driver */
int ltq_reset_cause(void)
{
	u32 val = ltq_rcu_r32(RCU_RST_STAT);
	return val >> RCU_STAT_SHIFT;
}
EXPORT_SYMBOL_GPL(ltq_reset_cause);

/* This function is used by the watchdog driver */
int ltq_reset_get_status (void)
{
	u32 val = ltq_rcu_r32(RCU_RST_REQ);
	return val;
}
EXPORT_SYMBOL_GPL(ltq_reset_get_status);

/* allow platform code to find out what source we booted from */
unsigned char ltq_boot_select(void)
{
	u32 val = ltq_rcu_r32(RCU_RST_STAT);

	if (of_device_is_compatible(ltq_rcu_np, "lantiq,rcu-xrx200"))
		return RCU_BOOT_SEL_XRX200(val);

	return RCU_BOOT_SEL(val);
}

/* reset / boot a gphy */
static struct ltq_xrx200_gphy_reset {
	u32 rd;
	u32 addr;
} xrx200_gphy[] = {
	{RCU_RD_GPHY0_XRX200, RCU_GFS_ADD0_XRX200},
	{RCU_RD_GPHY1_XRX200, RCU_GFS_ADD1_XRX200},
};

/* reset / boot a gphy */
static struct ltq_xrx300_gphy_reset {
	u32 rd;
	u32 addr;
} xrx300_gphy[] = {
	{RCU_RD_GPHY0_XRX300, RCU_GFS_ADD0_XRX300},
	{RCU_RD_GPHY1_XRX300, RCU_GFS_ADD1_XRX300},
	{RCU_RD_GPHY2_XRX300, RCU_GFS_ADD2_XRX300},
};

/* reset / boot a gphy */
static struct ltq_xrx330_gphy_reset {
	u32 rd;
	u32 addr;
} xrx330_gphy[] = {
	{RCU_RD_GPHY0_XRX330, RCU_GFS_ADD0_XRX330},
	{RCU_RD_GPHY1_XRX330, RCU_GFS_ADD1_XRX330},
	{RCU_RD_GPHY2_XRX330, RCU_GFS_ADD2_XRX330},
	{RCU_RD_GPHY3_XRX330, RCU_GFS_ADD3_XRX330},
};

/* reset and boot a gphy. these phys only exist on xrx200 SoC */
int xrx200_gphy_boot(struct device *dev, unsigned int id, dma_addr_t dev_addr)
{

	if (!of_device_is_compatible(ltq_rcu_np, "lantiq,rcu-xrx200")) {
		dev_err(dev, "this SoC has no GPHY\n");
		return -EINVAL;
	}

	if (of_machine_is_compatible("lantiq,vr9")) {
		struct clk *clk;
		clk = clk_get_sys("1f203000.rcu", "gphy");
		if (IS_ERR(clk))
			return PTR_ERR(clk);
		clk_enable(clk);
	}

	dev_info(dev, "booting GPHY%u firmware at %X\n", id, dev_addr);

	if (of_machine_is_compatible("lantiq,vr9")) {
		if (id > 1) {
			dev_err(dev, "%u is an invalid gphy id\n", id);
			return -EINVAL;
		}
		ltq_rcu_w32_mask(0, xrx200_gphy[id].rd, RCU_RST_REQ);
		ltq_rcu_w32(dev_addr, xrx200_gphy[id].addr);
		ltq_rcu_w32_mask(xrx200_gphy[id].rd, 0,  RCU_RST_REQ);
	} else if (of_machine_is_compatible("lantiq,ar10")) {
		if (id > 2) {
			dev_err(dev, "%u is an invalid gphy id\n", id);
			return -EINVAL;
		}
		ltq_rcu_w32_mask(0, xrx300_gphy[id].rd, RCU_RST_REQ);
		ltq_rcu_w32(dev_addr, xrx300_gphy[id].addr);
		ltq_rcu_w32_mask(xrx300_gphy[id].rd, 0,  RCU_RST_REQ);
	} else if (of_machine_is_compatible("lantiq,grx390")) {
		if (id > 3) {
			dev_err(dev, "%u is an invalid gphy id\n", id);
			return -EINVAL;
		}
		ltq_rcu_w32_mask(0, xrx330_gphy[id].rd, RCU_RST_REQ);
		ltq_rcu_w32(dev_addr, xrx330_gphy[id].addr);
		ltq_rcu_w32_mask(xrx330_gphy[id].rd, 0,  RCU_RST_REQ);
	}
	return 0;
}

/* reset a io domain for u micro seconds */
void ltq_reset_once(unsigned int module, ulong u)
{
	ltq_rcu_w32_mask(0, module, RCU_RST_REQ);
	udelay(u);
	ltq_rcu_w32_mask(module, 0, RCU_RST_REQ);
}
EXPORT_SYMBOL_GPL(ltq_reset_once);

/* reset a io domain by hardware control instead of software control */
void ltq_hw_reset(unsigned int module)
{
	ltq_rcu_w32(module, RCU_RST_REQ);
	while (!(ltq_rcu_r32(RCU_RST_STAT) & module))
		;
}
EXPORT_SYMBOL_GPL(ltq_hw_reset);

void ltq_rcu_w32(uint32_t val, uint32_t reg_off)
{
	ltq_w32(val, ltq_rcu_membase + reg_off);
}
EXPORT_SYMBOL_GPL(ltq_rcu_w32);

uint32_t ltq_rcu_r32(uint32_t reg_off)
{
	return ltq_r32(ltq_rcu_membase + reg_off);
}
EXPORT_SYMBOL_GPL(ltq_rcu_r32);

void ltq_rcu_w32_mask(uint32_t clr, uint32_t set, uint32_t reg_off)
{
	unsigned long flags;

	spin_lock_irqsave(&ltq_rcu_lock, flags);
	ltq_rcu_w32((ltq_rcu_r32(reg_off) & ~(clr)) | (set), reg_off);
	spin_unlock_irqrestore(&ltq_rcu_lock, flags);
}
EXPORT_SYMBOL_GPL(ltq_rcu_w32_mask);

static void ltq_xbar_ahb1s_big_endian(void)
{
	ltq_rcu_w32_mask(0, RCU_VR9_BE_AHB1S, RCU_AHB_ENDIAN);
}

static int ltq_assert_device(struct reset_controller_dev *rcdev,
				unsigned long id)
{
	u32 val;

	if (id < 8)
		return -1;

	val = ltq_rcu_r32(RCU_RST_REQ);
	val |= BIT(id);
	ltq_rcu_w32(val, RCU_RST_REQ);

	return 0;
}

static int ltq_deassert_device(struct reset_controller_dev *rcdev,
				  unsigned long id)
{
	u32 val;

	if (id < 8)
		return -1;

	val = ltq_rcu_r32(RCU_RST_REQ);
	val &= ~BIT(id);
	ltq_rcu_w32(val, RCU_RST_REQ);

	return 0;
}

static int ltq_reset_device(struct reset_controller_dev *rcdev,
			       unsigned long id)
{
	ltq_assert_device(rcdev, id);
	return ltq_deassert_device(rcdev, id);
}

static struct reset_control_ops reset_ops = {
	.reset = ltq_reset_device,
	.assert = ltq_assert_device,
	.deassert = ltq_deassert_device,
};

static struct reset_controller_dev reset_dev = {
	.ops			= &reset_ops,
	.owner			= THIS_MODULE,
	.nr_resets		= 32,
	.of_reset_n_cells	= 1,
};

void ltq_rst_init(void)
{
	reset_dev.of_node = of_find_compatible_node(NULL, NULL,
						"lantiq,xway-reset");
	if (!reset_dev.of_node)
		pr_err("Failed to find reset controller node");
	else
		reset_controller_register(&reset_dev);
}

static void ltq_machine_restart(char *command)
{
	local_irq_disable();
	 if (of_machine_is_compatible("lantiq,vr9")){
        ltq_rcu_w32(RCU_RD_SRST | RCU_RD_GPHY0_XRX200 | RCU_RD_GPHY1_XRX200 | RCU_RST_REQ_MIPS0 ,RCU_RST_REQ);

	}
	 else
	ltq_rcu_w32(ltq_rcu_r32(RCU_RST_REQ) | RCU_RD_SRST, RCU_RST_REQ);
	unreachable();
}

static void ltq_machine_halt(void)
{
	local_irq_disable();
	unreachable();
}

static void ltq_machine_power_off(void)
{
	local_irq_disable();
	unreachable();
}

static int __init mips_reboot_setup(void)
{
	struct resource res;

	ltq_rcu_np = of_find_compatible_node(NULL, NULL, "lantiq,rcu-xway");
	if (!ltq_rcu_np)
		ltq_rcu_np = of_find_compatible_node(NULL, NULL,
							"lantiq,rcu-xrx200");

	/* check if all the reset register range is available */
	if (!ltq_rcu_np)
		panic("Failed to load reset resources from devicetree");

	if (of_address_to_resource(ltq_rcu_np, 0, &res))
		panic("Failed to get rcu memory range");

	if (request_mem_region(res.start, resource_size(&res), res.name) < 0)
		pr_err("Failed to request rcu memory");

	ltq_rcu_membase = ioremap_nocache(res.start, resource_size(&res));
	if (!ltq_rcu_membase)
		panic("Failed to remap core memory");

	_machine_restart = ltq_machine_restart;
	_machine_halt = ltq_machine_halt;
	pm_power_off = ltq_machine_power_off;
	if (of_machine_is_compatible("lantiq,vr9"))
		ltq_xbar_ahb1s_big_endian();
	return 0;
}

arch_initcall(mips_reboot_setup);
