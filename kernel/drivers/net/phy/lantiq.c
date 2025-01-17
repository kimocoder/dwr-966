/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2012 Daniel Schwierzeck <daniel.schwierzeck@googlemail.com>
 */

#include <linux/module.h>
#include <linux/phy.h>

#define MII_MMDCTRL		0x0d
#define MII_MMDDATA		0x0e

#define MII_VR9_11G_IMASK	0x19	/* interrupt mask */
#define MII_VR9_11G_ISTAT	0x1a	/* interrupt status */

#define INT_VR9_11G_WOL		BIT(15)	/* Wake-On-LAN */
#define INT_VR9_11G_ANE		BIT(11)	/* Auto-Neg error */
#define INT_VR9_11G_ANC		BIT(10)	/* Auto-Neg complete */
#define INT_VR9_11G_ADSC	BIT(5)	/* Link auto-downspeed detect */
#define INT_VR9_11G_DXMC	BIT(2)	/* Duplex mode change */
#define INT_VR9_11G_LSPC	BIT(1)	/* Link speed change */
#define INT_VR9_11G_LSTC	BIT(0)	/* Link state change */
#define INT_VR9_11G_MASK	(INT_VR9_11G_LSTC | INT_VR9_11G_ADSC)

#define ADVERTISED_MPD		BIT(10)	/* Multi-port device */

#define MMD_DEVAD		0x1f
#define MMD_ACTYPE_SHIFT	14
#define MMD_ACTYPE_ADDRESS	(0 << MMD_ACTYPE_SHIFT)
#define MMD_ACTYPE_DATA		(1 << MMD_ACTYPE_SHIFT)
#define MMD_ACTYPE_DATA_PI	(2 << MMD_ACTYPE_SHIFT)
#define MMD_ACTYPE_DATA_PIWR	(3 << MMD_ACTYPE_SHIFT)

static __maybe_unused int vr9_gphy_mmd_read(struct phy_device *phydev,
						u16 regnum)
{
	phy_write(phydev, MII_MMDCTRL, MMD_ACTYPE_ADDRESS | MMD_DEVAD);
	phy_write(phydev, MII_MMDDATA, regnum);
	phy_write(phydev, MII_MMDCTRL, MMD_ACTYPE_DATA | MMD_DEVAD);

	return phy_read(phydev, MII_MMDDATA);
}

static __maybe_unused int vr9_gphy_mmd_write(struct phy_device *phydev,
						u16 regnum, u16 val)
{
	phy_write(phydev, MII_MMDCTRL, MMD_ACTYPE_ADDRESS | MMD_DEVAD);
	phy_write(phydev, MII_MMDDATA, regnum);
	phy_write(phydev, MII_MMDCTRL, MMD_ACTYPE_DATA | MMD_DEVAD);
	phy_write(phydev, MII_MMDDATA, val);

	return 0;
}

static int vr9_gphy_config_init(struct phy_device *phydev)
{
	int err;

	dev_dbg(&phydev->dev, "%s\n", __func__);

	/* Mask all interrupts */
	err = phy_write(phydev, MII_VR9_11G_IMASK, 0);
	if (err)
		return err;

	/* Clear all pending interrupts */
	phy_read(phydev, MII_VR9_11G_ISTAT);

	vr9_gphy_mmd_write(phydev, 0x1e0, 0xc5);
	vr9_gphy_mmd_write(phydev, 0x1e1, 0x67);
	vr9_gphy_mmd_write(phydev, 0x1e2, 0x42);
	vr9_gphy_mmd_write(phydev, 0x1e3, 0x10);
	vr9_gphy_mmd_write(phydev, 0x1e4, 0x70);
	vr9_gphy_mmd_write(phydev, 0x1e5, 0x03);
	vr9_gphy_mmd_write(phydev, 0x1e6, 0x20);
	vr9_gphy_mmd_write(phydev, 0x1e7, 0x00);
	vr9_gphy_mmd_write(phydev, 0x1e8, 0x40);
	vr9_gphy_mmd_write(phydev, 0x1e9, 0x20);

	return 0;
}

static int vr9_gphy_config_aneg(struct phy_device *phydev)
{
	int reg, err;

	/* Advertise as multi-port device */
	reg = phy_read(phydev, MII_CTRL1000);
	reg |= ADVERTISED_MPD;
	err = phy_write(phydev, MII_CTRL1000, reg);
	if (err)
		return err;

	return genphy_config_aneg(phydev);
}

static int vr9_gphy_ack_interrupt(struct phy_device *phydev)
{
	int reg;

	/*
	 * Possible IRQ numbers:
	 * - IM3_IRL18 for GPHY0
	 * - IM3_IRL17 for GPHY1
	 *
	 * Due to a silicon bug IRQ lines are not really independent from
	 * each other. Sometimes the two lines are driven at the same time
	 * if only one GPHY core raises the interrupt.
	 */

	reg = phy_read(phydev, MII_VR9_11G_ISTAT);

	return (reg < 0) ? reg : 0;
}

static int vr9_gphy_did_interrupt(struct phy_device *phydev)
{
	int reg;

	reg = phy_read(phydev, MII_VR9_11G_ISTAT);

	return reg > 0;
}

static int vr9_gphy_config_intr(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED)
		err = phy_write(phydev, MII_VR9_11G_IMASK, INT_VR9_11G_MASK);
	else
		err = phy_write(phydev, MII_VR9_11G_IMASK, 0);

	return err;
}

static struct phy_driver lantiq_phy[] = {
	{
		.phy_id		= 0xd565a400,
		.phy_id_mask	= 0xffffffff,
		.name		= "Lantiq XWAY PEF7071",
		.features	= (PHY_GBIT_FEATURES | SUPPORTED_Pause),
		.flags		= 0, /*PHY_HAS_INTERRUPT,*/
		.config_init	= vr9_gphy_config_init,
		.config_aneg	= vr9_gphy_config_aneg,
		.read_status	= genphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		.did_interrupt	= vr9_gphy_did_interrupt,
		.config_intr	= vr9_gphy_config_intr,
		.driver		= { .owner = THIS_MODULE },
	}, {
		.phy_id		= 0x030260D0,
		.phy_id_mask	= 0xfffffff0,
		.name		= "Lantiq XWAY VR9 GPHY 11G v1.3",
		.features	= (PHY_GBIT_FEATURES | SUPPORTED_Pause),
		.flags		= 0, /*PHY_HAS_INTERRUPT,*/
		.config_init	= vr9_gphy_config_init,
		.config_aneg	= vr9_gphy_config_aneg,
		.read_status	= genphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		.did_interrupt	= vr9_gphy_did_interrupt,
		.config_intr	= vr9_gphy_config_intr,
		.driver		= { .owner = THIS_MODULE },
	}, {
		.phy_id		= 0xd565a408,
		.phy_id_mask	= 0xfffffff8,
		.name		= "Lantiq XWAY VR9 GPHY 11G v1.4",
		.features	= (PHY_GBIT_FEATURES | SUPPORTED_Pause),
		.flags		= 0, /*PHY_HAS_INTERRUPT,*/
		.config_init	= vr9_gphy_config_init,
		.config_aneg	= vr9_gphy_config_aneg,
		.read_status	= genphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		.did_interrupt	= vr9_gphy_did_interrupt,
		.config_intr	= vr9_gphy_config_intr,
		.driver		= { .owner = THIS_MODULE },
	}, {
		.phy_id		= 0xd565a418,
		.phy_id_mask	= 0xfffffff8,
		.name		= "Lantiq XWAY XRX PHY22F v1.4",
		.features	= (PHY_BASIC_FEATURES | SUPPORTED_Pause),
		.flags		= 0, /*PHY_HAS_INTERRUPT,*/
		.config_init	= vr9_gphy_config_init,
		.config_aneg	= vr9_gphy_config_aneg,
		.read_status	= genphy_read_status,
		.ack_interrupt	= vr9_gphy_ack_interrupt,
		.did_interrupt	= vr9_gphy_did_interrupt,
		.config_intr	= vr9_gphy_config_intr,
		.driver		= { .owner = THIS_MODULE },
	},
};

static int __init ltq_phy_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(lantiq_phy); i++) {
		int err = phy_driver_register(&lantiq_phy[i]);
		if (err)
			pr_err("lantiq_phy: failed to load %s\n", lantiq_phy[i].name);
	}

	return 0;
}

static void __exit ltq_phy_exit(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(lantiq_phy); i++)
		phy_driver_unregister(&lantiq_phy[i]);
}

module_init(ltq_phy_init);
module_exit(ltq_phy_exit);

MODULE_DESCRIPTION("Lantiq PHY drivers");
MODULE_AUTHOR("Daniel Schwierzeck <daniel.schwierzeck@googlemail.com>");
MODULE_LICENSE("GPL");
