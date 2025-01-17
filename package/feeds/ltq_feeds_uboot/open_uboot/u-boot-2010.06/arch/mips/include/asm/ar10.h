#ifndef AR10_H
#define AR10_H

#define AR10_PLATFORM            "AR10"
#define KSEG1 0xa0000000


/***********************************************************************/
/*  Module      :  CSS register address and bits                       */
/***********************************************************************/
#define CSS_BASE   (KSEG1+0x1F800000)
#define CSS_DDR_ECHO_DLL0  (CSS_BASE + 0x1d8)
#define CSS_DDR_ECHO_DLL1  (CSS_BASE + 0x1e0)


/***********************************************************************/
/*  Module      :  WDT register address and bits                       */
/***********************************************************************/

#define BSP_BIU_WDT                           (KSEG1+0x1F880000)

/***Watchdog Timer Control Register ***/
#define BSP_BIU_WDT_CR                   ((volatile u32*)(BSP_BIU_WDT + 0x03F0))


/***********************************************************************/
/*  Module      :  CGU register address and bits                       */
/***********************************************************************/

#define AR10_CGU_BASE_ADDR                             (KSEG1+0x1F103000)

#define AR10_CGU_PLL0_CFG                ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0004))
#define AR10_CGU_PLL1_CFG                ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0008))
#define AR10_CGU_SYS                     ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x000C))
#define AR10_CGU_CLKFSR                  ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0010))
#define AR10_CGU_CLKGSR0                 ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0014))
#define AR10_CGU_CLKGSR0_A               ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0018))
#define AR10_CGU_CLKGSR0_B               ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x001C))
#define AR10_CGU_UPDATE                  ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0020))
#define AR10_CGU_IF_CLK                  ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0024))
#define AR10_CGU_CT1SR                   ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x002C))
#define AR10_CGU_CT_KVAL                 ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0030))
#define AR10_CGU_PCMCR                   ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0034))
#define AR10_EPHY1_CFG                   ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0040))
#define AR10_EPHY2_CFG                   ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0044))
#define AR10_EPHY3_CFG                   ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x0048))
#define AR10_EPHY0_CFG                   ((volatile u32*)(AR10_CGU_BASE_ADDR  + 0x004C))

/***********************************************************************/
/*  Module      :  DMA register address and bits                       */
/***********************************************************************/
#define AR10_DMA_OFFSET 0xBE104100
/***********************************************************************/
#define AR10_DMA_CLC                   ((volatile u32*)(AR10_DMA_OFFSET + 0x0000))
#define AR10_DMA_ID                    ((volatile u32*)(AR10_DMA_OFFSET + 0x0008))
#define AR10_DMA_CTRL                  ((volatile u32*)(AR10_DMA_OFFSET + 0x0010))
#define AR10_DMA_CPOLL                 ((volatile u32*)(AR10_DMA_OFFSET + 0x0014))
#define AR10_DMA_CS                    ((volatile u32*)(AR10_DMA_OFFSET + 0x0018))
#define AR10_DMA_CCTRL                 ((volatile u32*)(AR10_DMA_OFFSET + 0x001C))
#define AR10_DMA_CDBA                  ((volatile u32*)(AR10_DMA_OFFSET + 0x0020))
#define AR10_DMA_CDLEN                 ((volatile u32*)(AR10_DMA_OFFSET + 0x0024))
#define AR10_DMA_CIS                   ((volatile u32*)(AR10_DMA_OFFSET + 0x0028))
#define AR10_DMA_CIE                   ((volatile u32*)(AR10_DMA_OFFSET + 0x002C))
#define AR10_DMA_CGBL                  ((volatile u32*)(AR10_DMA_OFFSET + 0x0030))
#define AR10_DMA_CDPTNRD               ((volatile u32*)(AR10_DMA_OFFSET + 0x0034))
#define AR10_DMA_CDPTNRD1              ((volatile u32*)(AR10_DMA_OFFSET + 0x0038))
#define AR10_DMA_CS1                   ((volatile u32*)(AR10_DMA_OFFSET + 0x0050))
#define AR10_DMA_CCTRL1                ((volatile u32*)(AR10_DMA_OFFSET + 0x0054))
#define AR10_DMA_CDBA1                 ((volatile u32*)(AR10_DMA_OFFSET + 0x0058))
#define AR10_DMA_CDLEN1                ((volatile u32*)(AR10_DMA_OFFSET + 0x005C))
#define AR10_DMA_CIS1                  ((volatile u32*)(AR10_DMA_OFFSET + 0x0060))
#define AR10_DMA_CIE1                  ((volatile u32*)(AR10_DMA_OFFSET + 0x0064))
/*Registers, Port Related Registers*/
#define AR10_DMA_PS                    ((volatile u32*)(AR10_DMA_OFFSET + 0x0040))
#define AR10_DMA_PCTRL                 ((volatile u32*)(AR10_DMA_OFFSET + 0x0044))
#define AR10_DMA_PS1                   ((volatile u32*)(AR10_DMA_OFFSET + 0x0070))
#define AR10_DMA_PCTRL1                ((volatile u32*)(AR10_DMA_OFFSET + 0x0074))
/*Registers, Port Related Registers*/
#define AR10_DMA_IRNEN                 ((volatile u32*)(AR10_DMA_OFFSET + 0x00F4))
#define AR10_DMA_IRNCR                 ((volatile u32*)(AR10_DMA_OFFSET + 0x00F8))
#define AR10_DMA_IRNICR                ((volatile u32*)(AR10_DMA_OFFSET + 0x00FC))


/***********************************************************************/
/*  Module      :  EBU register address and bits                       */
/***********************************************************************/

#define AR10_EBU                          (0xB6000000)

/***EBU Clock Control Register***/
#define AR10_EBU_CLC                      ((volatile u32*)(AR10_EBU+ 0x0000))
#define AR10_EBU_ID                       ((volatile u32*)(AR10_EBU+ 0x0008))
#define AR10_EBU_CON                      ((volatile u32*)(AR10_EBU+ 0x0010))
#define AR10_EBU_ADDR_SEL_0               ((volatile u32*)(AR10_EBU+ 0x0020))
#define AR10_EBU_ADDR_SEL_1               ((volatile u32*)(AR10_EBU+ 0x0024))
#define AR10_EBU_CON_0                    ((volatile u32*)(AR10_EBU+ 0x0060))
#define AR10_EBU_CON_1                    ((volatile u32*)(AR10_EBU+ 0x0064))
#define AR10_EBU_ECC_ISTAT                ((volatile u32*)(AR10_EBU+ 0x00A0))
#define AR10_EBU_ECC_IEN                  ((volatile u32*)(AR10_EBU+ 0x00A4))
#define AR10_EBU_ECC_INT_OUT              ((volatile u32*)(AR10_EBU+ 0x00A8))
#define AR10_EBU_NAND_CON                 ((volatile u32*)(AR10_EBU+ 0x00B0))
#define AR10_EBU_NAND_WAIT                ((volatile u32*)(AR10_EBU+ 0x00B4))


#define EBU_ADDR_SEL0     (volatile u32*)(AR10_EBU + 0x20)
#define EBU_ADDR_SEL1     (volatile u32*)(AR10_EBU + 0x24)
#define EBU_ADDR_SEL2     (volatile u32*)(AR10_EBU + 0x28)
#define EBU_ADDR_SEL3     (volatile u32*)(AR10_EBU + 0x2C)
#define EBU_CON0          (volatile u32*)(AR10_EBU + 0x60)
#define EBU_CON1          (volatile u32*)(AR10_EBU + 0x64)
#define EBU_CON2          (volatile u32*)(AR10_EBU + 0x68)
#define EBU_CON3          (volatile u32*)(AR10_EBU + 0x6C)
#define EBU_NAND_CON       (volatile u32*)(AR10_EBU + 0xB0)
#define EBU_NAND_WAIT      (volatile u32*)(AR10_EBU + 0xB4)
#define EBU_NAND_ECC0      (volatile u32*)(AR10_EBU + 0xB8)
#define EBU_NAND_ECC_AC    (volatile u32*)(AR10_EBU + 0xBC)


#define EBU_ADDR_SEL_0     (volatile u32*)(AR10_EBU + 0x20)
#define EBU_ADDR_SEL_1     (volatile u32*)(AR10_EBU + 0x24)
#define EBU_CON_0          (volatile u32*)(AR10_EBU + 0x60)
#define EBU_CON_1          (volatile u32*)(AR10_EBU + 0x64)

/***********************************************************************/
/*  Module      :  HSNAND registers                                    */
/***********************************************************************/

#define HSNAND_BASE                       (0xB6000100)

#define BSP_NDAC_CTL1             ((volatile u32*)(HSNAND_BASE + 0x0010))
#define BSP_NDAC_CTL2             ((volatile u32*)(HSNAND_BASE + 0x0014))
#define BSP_INT_Msk_CTL           ((volatile u32*)(HSNAND_BASE + 0x0024))
#define BSP_INT_Sta               ((volatile u32*)(HSNAND_BASE + 0x0028))
#define BSP_MD_CTL                ((volatile u32*)(HSNAND_BASE + 0x0030))
#define BSP_ND_PARA0              ((volatile u32*)(HSNAND_BASE + 0x003C))
#define BSP_ND_ODD_ECC            ((volatile u32*)(HSNAND_BASE + 0x0040))
#define BSP_ND_ODD_ECC1           ((volatile u32*)(HSNAND_BASE + 0x0044))
#define BSP_ND_EVEN_ECC           ((volatile u32*)(HSNAND_BASE + 0x0048))
#define BSP_ND_EVEN_ECC1          ((volatile u32*)(HSNAND_BASE + 0x004C))


/***********************************************************************/
/*  Module      :  GPIO register address and bits                       */
/***********************************************************************/
#define AR10_GPIO                     (0xBE100B00)
#define AR10_GPIO_P0_OUT              ((volatile u32 *)(AR10_GPIO+ 0x0010))
#define AR10_GPIO_P0_IN               ((volatile u32 *)(AR10_GPIO+ 0x0014))
#define AR10_GPIO_P0_DIR              ((volatile u32 *)(AR10_GPIO+ 0x0018))
#define AR10_GPIO_P0_ALTSEL0          ((volatile u32 *)(AR10_GPIO+ 0x001C))
#define AR10_GPIO_P0_ALTSEL1          ((volatile u32 *)(AR10_GPIO+ 0x0020))
#define AR10_GPIO_P0_OD               ((volatile u32 *)(AR10_GPIO+ 0x0024))
#define AR10_GPIO_P0_STOFF            ((volatile u32 *)(AR10_GPIO+ 0x0028))
#define AR10_GPIO_P0_PUDSEL           ((volatile u32 *)(AR10_GPIO+ 0x002C))
#define AR10_GPIO_P0_PUDEN            ((volatile u32 *)(AR10_GPIO+ 0x0030))
#define AR10_GPIO_P3_OD               ((volatile u32 *)(AR10_GPIO+ 0x0034))
#define AR10_GPIO_P3_PUDSEL           ((volatile u32 *)(AR10_GPIO+ 0x0038))
#define AR10_GPIO_P3_PUDEN            ((volatile u32 *)(AR10_GPIO+ 0x003C))
#define AR10_GPIO_P1_OUT              ((volatile u32 *)(AR10_GPIO+ 0x0040))
#define AR10_GPIO_P1_IN               ((volatile u32 *)(AR10_GPIO+ 0x0044))
#define AR10_GPIO_P1_DIR              ((volatile u32 *)(AR10_GPIO+ 0x0048))
#define AR10_GPIO_P1_ALTSEL0          ((volatile u32 *)(AR10_GPIO+ 0x004C))
#define AR10_GPIO_P1_ALTSEL1          ((volatile u32 *)(AR10_GPIO+ 0x0050))
#define AR10_GPIO_P1_OD               ((volatile u32 *)(AR10_GPIO+ 0x0054))
#define AR10_GPIO_P1_STOFF            ((volatile u32 *)(AR10_GPIO+ 0x0058))
#define AR10_GPIO_P1_PUDSEL           ((volatile u32 *)(AR10_GPIO+ 0x005C))
#define AR10_GPIO_P1_PUDEN            ((volatile u32 *)(AR10_GPIO+ 0x0060))
#define AR10_GPIO_P3_ALTSEL1          ((volatile u32 *)(AR10_GPIO+ 0x0064))

#define AR10_GPIO_P2_OUT              ((volatile u32 *)(AR10_GPIO+ 0x0070))
#define AR10_GPIO_P2_IN               ((volatile u32 *)(AR10_GPIO+ 0x0074))
#define AR10_GPIO_P2_DIR              ((volatile u32 *)(AR10_GPIO+ 0x0078))
#define AR10_GPIO_P2_ALTSEL0          ((volatile u32 *)(AR10_GPIO+ 0x007C))
#define AR10_GPIO_P2_ALTSEL1          ((volatile u32 *)(AR10_GPIO+ 0x0080))
#define AR10_GPIO_P2_OD               ((volatile u32 *)(AR10_GPIO+ 0x0084))
#define AR10_GPIO_P2_STOFF            ((volatile u32 *)(AR10_GPIO+ 0x0088))
#define AR10_GPIO_P2_PUDSEL           ((volatile u32 *)(AR10_GPIO+ 0x008C))
#define AR10_GPIO_P2_PUDEN            ((volatile u32 *)(AR10_GPIO+ 0x0090))

#define AR10_GPIO_P3_OUT              ((volatile u32 *)(AR10_GPIO+ 0x00A0))
#define AR10_GPIO_P3_IN               ((volatile u32 *)(AR10_GPIO+ 0x00A4))
#define AR10_GPIO_P3_DIR              ((volatile u32 *)(AR10_GPIO+ 0x00A8))
#define AR10_GPIO_P3_ALTSEL0          ((volatile u32 *)(AR10_GPIO+ 0x00AC))


/***********************************************************************/
/*  Module      :  SSIO register address and bits                       */
/***********************************************************************/
#define AR10_SSIO_BASE_ADDR             (KSEG1+0x1E100B00)
#define AR10_LED_CON0                   ((volatile u32*)(AR10_SSIO_BASE_ADDR + 0x00B0))
#define AR10_LED_CON1                   ((volatile u32*)(AR10_SSIO_BASE_ADDR + 0x00B4))
#define AR10_LED_CPU0                   ((volatile u32*)(AR10_SSIO_BASE_ADDR + 0x00B8))
#define AR10_LED_CPU1                   ((volatile u32*)(AR10_SSIO_BASE_ADDR + 0x00BC))
#define AR10_LED_AR                     ((volatile u32*)(AR10_SSIO_BASE_ADDR + 0x00C0))

/***********************************************************************/
/*  Module      :  PMU register address and bits                       */
/***********************************************************************/
#define AR10_PMU_BASE_ADDR 				      (KSEG1+0x1F102000)

#define AR10_PMU_PWDCR                   ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x001c))
#define AR10_PMU_PWDCR1                  ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0024))
#define AR10_PMU_PWDSR                   ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0020))
#define AR10_PMU_PWDSR1                  ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0028))


#define AR10_PMU_CLKGSR1                ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0020))
#define AR10_PMU_CLKGCR1_A              ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0024))
#define AR10_PMU_CLKGCR1_B			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0028))
#define AR10_PMU_CLKGSR2                ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0030))
#define AR10_PMU_CLKGCR2_A              ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0034))
#define AR10_PMU_CLKGCR2_B			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0038))

#define AR10_PMU_ANALOG_SR			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0040))
#define AR10_PMU_ANALOGCR_A			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0044))
#define AR10_PMU_ANALOGCR_B			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0048))

#define AR10_PMU_PWGCSR   			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0100))
#define AR10_PMU_RD       			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0104))
#define AR10_PMU_POD       			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0108))
#define AR10_PMU_PD_SEL   			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x010C))
#define AR10_PMU_WKUP_LOC  			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0200))
#define AR10_PMU_PD_WKUP_EN			        ((volatile u32*)(AR10_PMU_BASE_ADDR + 0x0204))

/***PM Global Enable Register 0 ***/
#define BSP_PMU_USB0_PHY        (1 << 0)
#define BSP_PMU_FPIS            (1 << 1)
#define BSP_PMU_DFEV0           (1 << 2)
#define BSP_PMU_DEV1            (1 << 3)
#define BSP_PMU_PCI             (1 << 4)
#define BSP_PMU_DMA             (1 << 5)
#define BSP_PMU_USB0            (1 << 6)
#define BSP_PMU_USIF            (1 << 7)
#define BSP_PMU_SPI             (1 << 8)
#define BSP_PMU_DSL             (1 << 9)
#define BSP_PMU_EBU             (1 << 10)
#define BSP_PMU_LEDC            (1 << 11)
#define BSP_PMU_GPTC            (1 << 12)
#define BSP_PMU_AHBS            (1 << 13)
#define BSP_PMU_FPIM            (1 << 14)
#define BSP_PMU_AHBM            (1 << 15)
#define BSP_PMU_SDIO            (1 << 16)
#define BSP_PMU_UART1           (1 << 17)
#define BSP_PMU_PPE_QSB         (1 << 18)
#define BSP_PMU_SSL01           (1 << 19)
#define BSP_PMU_DEU             (1 << 20)
#define BSP_PMU_PPE_TC          (1 << 21)
#define BSP_PMU_PPE_EMA         (1 << 22)
#define BSP_PMU_PPE_DPLUS       (1 << 23)
#define BSP_PMU_PPE_DPSS        (1 << 24)
#define BSP_PMU_TDM             (1 << 25)
#define BSP_PMU_USB1_PHY        (1 << 26)
#define BSP_PMU_USB1            (1 << 27)
#define BSP_PMU_SWITCH          (1 << 28)
#define BSP_PMU_PPE_TOP         (1 << 29)
#define BSP_PMU_GPHY            (1 << 30)
#define BSP_PMU_PCIE0_EN        (1 << 31)



/***********************************************************************/
/*  Module      :  RCU register address and bits                       */
/***********************************************************************/
#define AR10_RCU_BASE_ADDR 		(0xBF203000)

/*Reset related Register Description*/
#define AR10_RCU_RST_REQ              ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0010))
#define AR10_RCU_RST_STAT             ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0014))
#define AR10_RCU_RST_STAT2            ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0024))
#define AR10_RCU_RST_REQ2             ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0048))

/*Configuration related Register Description*/
#define AR10_RCU_USB0_CFG             ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0018))
#define AR10_RCU_GP_STRAP             ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x001C))
#define AR10_RCU_RST_PPM              ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0028))
#define AR10_RCU_RST_PPE_CONF         ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x002C))
#define AR10_RCU_RST_PCIEPHY1_CON     ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0030))
#define AR10_RCU_USB1_Cfg             ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0034))
#define AR10_RCU_USB_ANA_CFG1A        ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0038))
#define AR10_RCU_USB_ANA_CFG1B        ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x003C))
#define AR10_RCU_ENDIAN               ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x004C))
#define AR10_RCU_PCIPH2_CON           ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x005C))

/*PCM DMA FCC Registers*/
#define AR10_RCU_PCM_FCC_CFG          ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0060))


/*Module Base Address Configuration*/
#define AR10_RCU_GFS_ADD0             ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0020))
#define AR10_RCU_GCC                  ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0040))
#define AR10_RCU_GFS_ADD1             ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x0058))
#define AR10_RCU_GFS_ADD2             ((volatile u32*)(AR10_RCU_BASE_ADDR + 0x00AC))

/***************************************************************************/
/* Module     : Debug unit                                                 */
/***************************************************************************/
#define AR10_DEBUG_BASE_ADDR    (0xBF203200)

#define AR10_RCU_GFS_ADD3       ((volatile u32*)(AR10_DEBUG_BASE_ADDR + 0x0064))


/* AR10 GPORT SWITCH Register */
#define AR10_SWITCH	  		(0xBE108000)

#define AR10_SWITCH_PS	  		(AR10_SWITCH + 0x000)
#define AR10_SWITCH_P0_CTL		(AR10_SWITCH + 0x004)
#define AR10_SWITCH_P1_CTL		(AR10_SWITCH + 0x008)
#define AR10_SWITCH_P2_CTL		(AR10_SWITCH + 0x00C)
#define AR10_SWITCH_P0_VLAN		(AR10_SWITCH + 0x010)
#define AR10_SWITCH_P1_VLAN		(AR10_SWITCH + 0x014)
#define AR10_SWITCH_P2_VLAN		(AR10_SWITCH + 0x018)
#define AR10_SWITCH_P0_INCTL		(AR10_SWITCH + 0x020)
#define AR10_SWITCH_P1_INCTL		(AR10_SWITCH + 0x024)
#define AR10_SWITCH_P2_INCTL		(AR10_SWITCH + 0x028)
#define AR10_SWITCH_DF_PORTMAP		(AR10_SWITCH + 0x02C)
#define AR10_SWITCH_P0_ECS_Q32		(AR10_SWITCH + 0x030)
#define AR10_SWITCH_P0_ECS_Q10		(AR10_SWITCH + 0x034)
#define AR10_SWITCH_P0_ECW_Q32		(AR10_SWITCH + 0x038)
#define AR10_SWITCH_P0_ECW_Q10		(AR10_SWITCH + 0x03C)
#define AR10_SWITCH_P1_ECS_Q32		(AR10_SWITCH + 0x040)
#define AR10_SWITCH_P1_ECS_Q10		(AR10_SWITCH + 0x044)
#define AR10_SWITCH_P1_ECW_Q32		(AR10_SWITCH + 0x048)
#define AR10_SWITCH_P1_ECW_Q10		(AR10_SWITCH + 0x04C)
#define AR10_SWITCH_P2_ECS_Q32		(AR10_SWITCH + 0x050)
#define AR10_SWITCH_P2_ECS_Q10		(AR10_SWITCH + 0x054)
#define AR10_SWITCH_P2_ECW_Q32		(AR10_SWITCH + 0x058)
#define AR10_SWITCH_P2_ECW_Q10		(AR10_SWITCH + 0x05C)
#define AR10_SWITCH_INT_ENA		(AR10_SWITCH + 0x060)
#define AR10_SWITCH_INT_ST		(AR10_SWITCH + 0x064)
#define AR10_SWITCH_GCTL0		(AR10_SWITCH + 0x068)
#define AR10_SWITCH_GCTL1		(AR10_SWITCH + 0x06C)
#define AR10_SWITCH_ARP			(AR10_SWITCH + 0x070)
#define AR10_SWITCH_STRM_CTL		(AR10_SWITCH + 0x074)
#define AR10_SWITCH_RGMII_CTL		(AR10_SWITCH + 0x078)
#define AR10_SWITCH_1P_PRT		(AR10_SWITCH + 0x07C)
#define AR10_SWITCH_GBKT_SZBS		(AR10_SWITCH + 0x080)
#define AR10_SWITCH_GBKT_SZEBS		(AR10_SWITCH + 0x084)
#define AR10_SWITCH_BF_TH		(AR10_SWITCH + 0x088)
#define AR10_SWITCH_PMAC_HD_CTL		(AR10_SWITCH + 0x08C)
#define AR10_SWITCH_PMAC_SA1		(AR10_SWITCH + 0x090)
#define AR10_SWITCH_PMAC_SA2		(AR10_SWITCH + 0x094)
#define AR10_SWITCH_PMAC_DA1		(AR10_SWITCH + 0x098)
#define AR10_SWITCH_PMAC_DA2		(AR10_SWITCH + 0x09C)
#define AR10_SWITCH_PMAC_VLAN		(AR10_SWITCH + 0x0A0)
#define AR10_SWITCH_PMAC_TX_IPG		(AR10_SWITCH + 0x0A4)
#define AR10_SWITCH_PMAC_RX_IPG		(AR10_SWITCH + 0x0A8)
#define AR10_SWITCH_ADR_TB_CTL0		(AR10_SWITCH + 0x0AC)
#define AR10_SWITCH_ADR_TB_CTL1		(AR10_SWITCH + 0x0B0)
#define AR10_SWITCH_ADR_TB_CTL2		(AR10_SWITCH + 0x0B4)
#define AR10_SWITCH_ADR_TB_ST0		(AR10_SWITCH + 0x0B8)
#define AR10_SWITCH_ADR_TB_ST1		(AR10_SWITCH + 0x0BC)
#define AR10_SWITCH_ADR_TB_ST2		(AR10_SWITCH + 0x0C0)
#define AR10_SWITCH_RMON_CTL		(AR10_SWITCH + 0x0C4)
#define AR10_SWITCH_RMON_ST		(AR10_SWITCH + 0x0C8)
#define AR10_SWITCH_MDIO_CTL		(AR10_SWITCH + 0x0CC)
#define AR10_SWITCH_MDIO_DATA		(AR10_SWITCH + 0x0D0)
#define AR10_SWITCH_TP_FLT_ACT		(AR10_SWITCH + 0x0D4)
#define AR10_SWITCH_PRTCL_FLT_ACT	(AR10_SWITCH + 0x0D8)
#define AR10_SWITCH_P0_PCDU_CTRL		(AR10_SWITCH + 0x0E4)
#define AR10_SWITCH_P1_PCDU_CTRL		(AR10_SWITCH + 0x0E8)
#define AR10_SWITCH_VLAN_FLT0		(AR10_SWITCH + 0x100)
#define AR10_SWITCH_VLAN_FLT1		(AR10_SWITCH + 0x104)
#define AR10_SWITCH_VLAN_FLT2		(AR10_SWITCH + 0x108)
#define AR10_SWITCH_VLAN_FLT3		(AR10_SWITCH + 0x10C)
#define AR10_SWITCH_VLAN_FLT4		(AR10_SWITCH + 0x110)
#define AR10_SWITCH_VLAN_FLT5		(AR10_SWITCH + 0x114)
#define AR10_SWITCH_VLAN_FLT6		(AR10_SWITCH + 0x118)
#define AR10_SWITCH_VLAN_FLT7		(AR10_SWITCH + 0x11C)
#define AR10_SWITCH_VLAN_FLT8		(AR10_SWITCH + 0x120)
#define AR10_SWITCH_VLAN_FLT9		(AR10_SWITCH + 0x124)
#define AR10_SWITCH_VLAN_FLT10		(AR10_SWITCH + 0x128)
#define AR10_SWITCH_VLAN_FLT11		(AR10_SWITCH + 0x12C)
#define AR10_SWITCH_VLAN_FLT12		(AR10_SWITCH + 0x130)
#define AR10_SWITCH_VLAN_FLT13		(AR10_SWITCH + 0x134)
#define AR10_SWITCH_VLAN_FLT14		(AR10_SWITCH + 0x138)
#define AR10_SWITCH_VLAN_FLT15		(AR10_SWITCH + 0x13C)
#define AR10_SWITCH_TP_FLT10		(AR10_SWITCH + 0x140)
#define AR10_SWITCH_TP_FLT32		(AR10_SWITCH + 0x144)
#define AR10_SWITCH_TP_FLT54		(AR10_SWITCH + 0x148)
#define AR10_SWITCH_TP_FLT76		(AR10_SWITCH + 0x14C)
#define AR10_SWITCH_DFSRV_MAP0		(AR10_SWITCH + 0x150)
#define AR10_SWITCH_DFSRV_MAP1		(AR10_SWITCH + 0x154)
#define AR10_SWITCH_DFSRV_MAP2		(AR10_SWITCH + 0x158)
#define AR10_SWITCH_DFSRV_MAP3		(AR10_SWITCH + 0x15C)
#define AR10_SWITCH_TCP_PF0		(AR10_SWITCH + 0x160)
#define AR10_SWITCH_TCP_PF1		(AR10_SWITCH + 0x164)
#define AR10_SWITCH_TCP_PF2		(AR10_SWITCH + 0x168)
#define AR10_SWITCH_TCP_PF3		(AR10_SWITCH + 0x16C)
#define AR10_SWITCH_TCP_PF4		(AR10_SWITCH + 0x170)
#define AR10_SWITCH_TCP_PF5		(AR10_SWITCH + 0x174)
#define AR10_SWITCH_TCP_PF6		(AR10_SWITCH + 0x178)
#define AR10_SWITCH_TCP_PF7		(AR10_SWITCH + 0x17C)
#define AR10_SWITCH_RA_03_00		(AR10_SWITCH + 0x180)
#define AR10_SWITCH_RA_07_04		(AR10_SWITCH + 0x184)
#define AR10_SWITCH_RA_0B_08		(AR10_SWITCH + 0x188)
#define AR10_SWITCH_RA_0F_0C		(AR10_SWITCH + 0x18C)
#define AR10_SWITCH_RA_13_10		(AR10_SWITCH + 0x190)
#define AR10_SWITCH_RA_17_14		(AR10_SWITCH + 0x194)
#define AR10_SWITCH_RA_1B_18		(AR10_SWITCH + 0x198)
#define AR10_SWITCH_RA_1F_1C		(AR10_SWITCH + 0x19C)
#define AR10_SWITCH_RA_23_20		(AR10_SWITCH + 0x1A0)
#define AR10_SWITCH_RA_27_24		(AR10_SWITCH + 0x1A4)
#define AR10_SWITCH_RA_2B_28		(AR10_SWITCH + 0x1A8)
#define AR10_SWITCH_RA_2F_2C		(AR10_SWITCH + 0x1AC)
#define AR10_SWITCH_F0			(AR10_SWITCH + 0x1B0)
#define AR10_SWITCH_F1			(AR10_SWITCH + 0x1B4)




/* AR10 SWITCH Register */
#define AR10_ETHSW_SS_BASE_ADDR                  (0xBE108000)



#define AR10_ETHSW_TOP   (AR10_ETHSW_SS_BASE_ADDR+( 0xC40 * 4) )

#define AR10_ETHSW_MAC   (AR10_ETHSW_SS_BASE_ADDR+(0x900*4))


#define AR10_ETHSW_SWRES         (AR10_ETHSW_SS_BASE_ADDR )
/*Hardware reset, reset all HW modules except for register settings  */
#define AR10_ETHSW_SWRES_R1      0x1     /*Bit< 1 >*/
/*Reset all registers */
#define AR10_ETHSW_SWRES_R0      0x0     /*Bit< 0 >*/

#define AR10_ETHSW_CLK           (AR10_ETHSW_SS_BASE_ADDR+ (0x1 * 4) )

#define AR10_ETHSW_BM_RAM_VAL_3  (AR10_ETHSW_SS_BASE_ADDR+ (0x40 * 4) )
#define AR10_ETHSW_BM_RAM_VAL_2  (AR10_ETHSW_SS_BASE_ADDR+ (0x41 * 4) )
#define AR10_ETHSW_BM_RAM_VAL_1  (AR10_ETHSW_SS_BASE_ADDR+ (0x42 * 4) )
#define AR10_ETHSW_BM_RAM_VAL_0  (AR10_ETHSW_SS_BASE_ADDR+ (0x43 * 4) )
#define AR10_ETHSW_BM_RAM_ADDR   (AR10_ETHSW_SS_BASE_ADDR+ (0x44 * 4) )
#define AR10_ETHSW_BM_RAM_CTRL   (AR10_ETHSW_SS_BASE_ADDR+ (0x45 * 4) )

#define AR10_ETHSW_BM_FSQM_GCTRL (AR10_ETHSW_SS_BASE_ADDR+ (0x46 * 4) )
#define AR10_ETHSW_BM_CONS_SEG   (AR10_ETHSW_SS_BASE_ADDR+ (0x47 * 4) )
#define AR10_ETHSW_BM_CONS_PKT   (AR10_ETHSW_SS_BASE_ADDR+ (0x48 * 4) )

#define AR10_ETHSW_BM_GCTRL      (AR10_ETHSW_SS_BASE_ADDR+ (0x49 * 4) )
#define AR10_ETHSW_BM_GCTRL_R_SRES(val)  ((val & 0x1 ) << 4 )
#define AR10_ETHSW_BM_QUEUE_GCTRL      (AR10_ETHSW_SS_BASE_ADDR+ (0x4A * 4) )

/*Buffer manager per port registrs*/
#define AR10_ETHSW_BM_PCFG       (AR10_ETHSW_SS_BASE_ADDR+ (0x80 * 4) )
#define AR10_ETHSW_BM_PCFG_CNTEN(val)    (((val & 0x1) << 0 ) )
#define AR10_ETHSW_BM_RMON_CTRL  (AR10_ETHSW_SS_BASE_ADDR+ (0x81 * 4) )

/*Rate shaper */

#define AR10_ETHSW_BM_QUEUE_PQM_DP        (AR10_ETHSW_SS_BASE_ADDR+ (0x100 * 4) )
#define AR10_ETHSW_BM_QUEUE_PQM_RS        (AR10_ETHSW_SS_BASE_ADDR+ (0x101 * 4) )
#define AR10_ETHSW_BM_SHAPER_RS_CTRL      (AR10_ETHSW_SS_BASE_ADDR+ (0x140 * 4) )
#define AR10_ETHSW_BM_SHAPER_RS_CBS       (AR10_ETHSW_SS_BASE_ADDR+ (0x141 * 4) )
#define AR10_ETHSW_BM_SHAPER_RS_IBS       (AR10_ETHSW_SS_BASE_ADDR+ (0x142 * 4) )
#define AR10_ETHSW_BM_SHAPER_RS_CIR_EXP   (AR10_ETHSW_SS_BASE_ADDR+ (0x143 * 4) )
#define AR10_ETHSW_BM_SHAPER_RS_CIR_MANT   (AR10_ETHSW_SS_BASE_ADDR+ (0x144 * 4) )

/* Metering */


#define AR10_ETHSW_PCE_TCM_CTRL          (AR10_ETHSW_SS_BASE_ADDR+(0x580*4))

#define AR10_ETHSW_PCE_TCM_STAT              (AR10_ETHSW_SS_BASE_ADDR+(0x581*4))
#define AR10_ETHSW_PCE_TCM_CBS      (AR10_ETHSW_SS_BASE_ADDR+(0x582*4))
#define AR10_ETHSW_PCE_TCM_EBS      (AR10_ETHSW_SS_BASE_ADDR+(0x583*4))
#define AR10_ETHSW_PCE_TCM_IBS      (AR10_ETHSW_SS_BASE_ADDR+(0x584*4))
#define AR10_ETHSW_PCE_TCM_CIR_MANT  (AR10_ETHSW_SS_BASE_ADDR+(0x585*4))
#define AR10_ETHSW_PCE_TCM_CIR_EXP   (AR10_ETHSW_SS_BASE_ADDR+(0x586*4))


/*WRED */
#define AR10_ETHSW_BM_WRED_RTH_0         (AR10_ETHSW_SS_BASE_ADDR+(0x04B*4))
#define AR10_ETHSW_BM_WRED_RTH_1         (AR10_ETHSW_SS_BASE_ADDR+(0x04C*4))
#define AR10_ETHSW_BM_WRED_YTH_0         (AR10_ETHSW_SS_BASE_ADDR+(0x04D*4))
#define AR10_ETHSW_BM_WRED_YTH_1         (AR10_ETHSW_SS_BASE_ADDR+(0x04E*4))
#define AR10_ETHSW_BM_WRED_GTH_0         (AR10_ETHSW_SS_BASE_ADDR+(0x04F*4))
#define AR10_ETHSW_BM_WRED_GTH_1         (AR10_ETHSW_SS_BASE_ADDR+(0x050*4))

/* MAC frame length register*/
#define AR10_ETHSW_MAC_FLEN      ( AR10_ETHSW_SS_BASE_ADDR + (0x8C5 *4))
/* fetch DMA port control */
#define AR10_ETHSW_FDMA_PCTRL_6          ( AR10_ETHSW_SS_BASE_ADDR + (0xAA4 *4))

/* SDMA registers */
#define AR10_ETHSW_SDMA_CTRL    ( AR10_ETHSW_SS_BASE_ADDR + (0xB40 *4))
#define AR10_ETHSW_SDMA_FCTHR1    ( AR10_ETHSW_SS_BASE_ADDR + (0xB41 *4))
#define AR10_ETHSW_SDMA_FCTHR2    ( AR10_ETHSW_SS_BASE_ADDR + (0xB42 *4))
#define AR10_ETHSW_SDMA_FCTHR3    ( AR10_ETHSW_SS_BASE_ADDR + (0xB43 *4))
#define AR10_ETHSW_SDMA_FCTHR4    ( AR10_ETHSW_SS_BASE_ADDR + (0xB44 *4))
#define AR10_ETHSW_SDMA_FCTHR5    ( AR10_ETHSW_SS_BASE_ADDR + (0xB45 *4))
#define AR10_ETHSW_SDMA_FCTHR6    ( AR10_ETHSW_SS_BASE_ADDR + (0xB46 *4))
#define AR10_ETHSW_SDMA_FCTHR7    ( AR10_ETHSW_SS_BASE_ADDR + (0xB47 *4))
#define AR10_ETHSW_SDMA_STAT0    ( AR10_ETHSW_SS_BASE_ADDR + (0xB48 *4))
#define AR10_ETHSW_SDMA_STAT1    ( AR10_ETHSW_SS_BASE_ADDR + (0xB49 *4))
#define AR10_ETHSW_SDMA_STAT2    ( AR10_ETHSW_SS_BASE_ADDR + (0xB4A *4))




/* Port Map Register */
#define AR10_ETHSW_PCE_PMAP_1    ( AR10_ETHSW_SS_BASE_ADDR + (0x453 *4))
#define AR10_ETHSW_PCE_PMAP_2    ( AR10_ETHSW_SS_BASE_ADDR + (0x454 *4))
#define AR10_ETHSW_PCE_PMAP_3    ( AR10_ETHSW_SS_BASE_ADDR + (0x455 *4))
#define AR10_ETHSW_PCE_GCTRL_0   ( AR10_ETHSW_SS_BASE_ADDR + (0x456 *4))
#define AR10_ETHSW_PCE_GCTRL_1   ( AR10_ETHSW_SS_BASE_ADDR + (0x457 *4))

/*Ethernet MAC registers */
/*MAC port status register*/
#define AR10_ETHSW_MAC_PSTAT     ( AR10_ETHSW_MAC  )
/*MAC interrupt status register*/
#define AR10_ETHSW_MAC_PISR      ( AR10_ETHSW_MAC+(0x1 *4 ))
/*MAC Interrupt enable reisgter */
#define AR10_ETHSW_MAC_PIER      ( AR10_ETHSW_MAC+(0x2 *4 ))
/*MAC control registers */
#define AR10_ETHSW_MAC_CTRL_0    ( AR10_ETHSW_MAC+(0x3 *4))
#define AR10_ETHSW_MAC_CTRL_1    ( AR10_ETHSW_MAC+(0x4 *4))
#define AR10_ETHSW_MAC_CTRL_2    ( AR10_ETHSW_MAC+(0x5 *4))
#define AR10_ETHSW_MAC_CTRL_3    ( AR10_ETHSW_MAC+(0x6 *4))
#define AR10_ETHSW_MAC_CTRL_4    ( AR10_ETHSW_MAC+(0x7 *4))
#define AR10_ETHSW_MAC_CTRL_5    ( AR10_ETHSW_MAC+(0x8 *4))
#define AR10_ETHSW_MAC_CTRL_6    ( AR10_ETHSW_MAC+(0x9 *4))

/*G<II/MII interface mode selection */
#define AR10_ETHSW_MAC_CTRL_GMII         0   /* <1:0>  interface mode selection */
#define AR10_ETHSW_MAC_CTRL_GMII_AUTO            0
#define AR10_ETHSW_MAC_CTRL_GMII_MII             1 /*MII/RMII interface mode at 10, 100 or 200 Mbit/s */
#define AR10_ETHSW_MAC_CTRL_GMII_GMII            2       /* GMII/RGMII interface mode at 1000 Mbit/s */

/*Full duplex control */
#define AR10_ETHSW_MAC_CTRL_FDUP         2 /*<3:2> full duplex control */
#define AR10_ETHSW_MAC_CTRL_FDUP_AUTO    0
#define AR10_ETHSW_MAC_CTRL_FDUP_EN      1
#define AR10_ETHSW_MAC_CTRL_FDUP_DIS     3

#define AR10_ETHSW_MAC_CTRL_FCON         4 /*<6:4> Flow control mode*/
#define AR10_ETHSW_MAC_CTRL_AUTO         0
#define AR10_ETHSW_MAC_CTRL_RX           1  /*Rx only */
#define AR10_ETHSW_MAC_CTRL_TX           2 /*Tx only */
#define AR10_ETHSW_MAC_CTRL_RXTX         3       /*Rx and Tx  */
#define AR10_ETHSW_MAC_CTRL_NONE         4       /*Disable*/

#define AR10_ETHSW_MAC_CTRL_FCS          7 /*<7> Tx Frame checksum generation */


/*MAC Rx/Tx buffer status register */
#define AR10_ETHSW_MAC_BUFST     ( AR10_ETHSW_MAC+(0xA *4))
/*MAC test enable register */
#define AR10_ETHSW_MAC_TESTEN    ( AR10_ETHSW_MAC+(0xB *4))

/*ETHSW_MDIO_PDI Registers*/
#define AR10_ETHSW_GLOB_CTRL             (AR10_ETHSW_TOP+0)
#define GLOB_CTRL_SE    15      /*Global switch macro enable*/
#define GLOB_CTRL_HWRES 1       /*Global Hardware reset */
#define GLOB_CTRL_SWRES 0       /*Global Software reset */

/*Switch Parse and Classification Engine registers*/
#define AR10_ETHSW_PCE_PCTRL_0           (AR10_ETHSW_SS_BASE_ADDR+(0x480*4))
#define AR10_ETHSW_PCE_PCTRL_1           (AR10_ETHSW_SS_BASE_ADDR+(0x481*4))
#define AR10_ETHSW_PCE_PCTRL_2           (AR10_ETHSW_SS_BASE_ADDR+(0x482*4))
#define AR10_ETHSW_PCE_PCTRL_3           (AR10_ETHSW_SS_BASE_ADDR+(0x483*4))
#define AR10_ETHSW_PCE_WOL_CTRL          (AR10_ETHSW_SS_BASE_ADDR+(0x484*4))
#define AR10_ETHSW_PCE_VCTRL             (AR10_ETHSW_SS_BASE_ADDR+(0x485*4))
#define AR10_ETHSW_PCE_DEFPVID           (AR10_ETHSW_SS_BASE_ADDR+(0x486*4))
#define AR10_ETHSW_PCE_PSTAT             (AR10_ETHSW_SS_BASE_ADDR+(0x487*4))
#define AR10_ETHSW_PCE_PIER              (AR10_ETHSW_SS_BASE_ADDR+(0x488*4))
#define AR10_ETHSW_PCE_PISR              (AR10_ETHSW_SS_BASE_ADDR+(0x489*4))

//#define AR10_ETHSW_SDMA_CTRL             (AR10_ETHSW_SS_BASE_ADDR+(0xB40*4))
//#define AR10_ETHSW_SDMA_FCTHR5           (AR10_ETHSW_SS_BASE_ADDR+(0xB45*4))
//#define AR10_ETHSW_SDMA_FCTHR6           (AR10_ETHSW_SS_BASE_ADDR+(0xB46*4))
#define AR10_ETHSW_FDMA_PCTRL_3          (AR10_ETHSW_SS_BASE_ADDR+(0xA92*4))
#define AR10_ETHSW_MDIO_CTRL             ( AR10_ETHSW_TOP + (0x8 * 4))
#define AR10_ETHSW_MDIO_READ             ( AR10_ETHSW_TOP + (0x9 * 4))
#define AR10_ETHSW_MDIO_WRITE            ( AR10_ETHSW_TOP + (0xa * 4))

#define AR10_ETHSW_MDC_CFG_0             ( AR10_ETHSW_TOP + (0xb  * 4))
#define AR10_ETHSW_CFG_0_PEN_0           0       /*Polling state machine enable */
#define AR10_ETHSW_CFG_0_PEN_1           1       /*Polling state machine enable */
#define AR10_ETHSW_CFG_0_PEN_2           2       /*Polling state machine enable */
#define AR10_ETHSW_CFG_0_PEN_3           3       /*Polling state machine enable */
#define AR10_ETHSW_CFG_0_PEN_4           4       /*Polling state machine enable */
#define AR10_ETHSW_CFG_0_PEN_5           5       /*Polling state machine enable */

#define AR10_ETHSW_MDC_CFG_1             ( AR10_ETHSW_TOP  + (0xc * 4))
#define AR10_ETHSW_CFG_1_RES             15      /*MDIO Hardware Reset*/
#define AR10_ETHSW_CFG_1_MCEN            8       /*MDC clock enable*/
#define AR10_ETHSW_CFG_1_FREQ            0       /*MDIO interface clock rate*/

#define AR10_ETHSW_PHY_ADDR_5            ( AR10_ETHSW_TOP  + (0x10 * 4))
#define AR10_ETHSW_PHY_ADDR_4            ( AR10_ETHSW_TOP  + (0x11 * 4))
#define AR10_ETHSW_PHY_ADDR_3            ( AR10_ETHSW_TOP  + (0x12 * 4))
#define AR10_ETHSW_PHY_ADDR_2            ( AR10_ETHSW_TOP  + (0x13 * 4))
#define AR10_ETHSW_PHY_ADDR_1            ( AR10_ETHSW_TOP  + (0x14 * 4))
#define AR10_ETHSW_PHY_ADDR_0            ( AR10_ETHSW_TOP  + (0x15 * 4))

#define AR10_ETHSW_MDIO_STAT_0           ( AR10_ETHSW_TOP  + (0x16 * 4))
#define AR10_ETHSW_MDIO_STAT_1           ( AR10_ETHSW_TOP  + (0x17 * 4))
#define AR10_ETHSW_MDIO_STAT_2           ( AR10_ETHSW_TOP  + (0x18 * 4))
#define AR10_ETHSW_MDIO_STAT_3           ( AR10_ETHSW_TOP  + (0x19 * 4))
#define AR10_ETHSW_MDIO_STAT_4           ( AR10_ETHSW_TOP  + (0x1A * 4))
#define AR10_ETHSW_MDIO_STAT_5           ( AR10_ETHSW_TOP  + (0x1B * 4))

#define AR10_ETHSW_ANEG_EEE_0            ( AR10_ETHSW_TOP  + (0x1C * 4))
#define AR10_ETHSW_ANEG_EEE_1            ( AR10_ETHSW_TOP  + (0x1D * 4))
#define AR10_ETHSW_ANEG_EEE_2            ( AR10_ETHSW_TOP  + (0x1E * 4))
#define AR10_ETHSW_ANEG_EEE_3            ( AR10_ETHSW_TOP  + (0x1F * 4))
#define AR10_ETHSW_ANEG_EEE_4            ( AR10_ETHSW_TOP  + (0x20 * 4))
#define AR10_ETHSW_ANEG_EEE_5            ( AR10_ETHSW_TOP  + (0x21 * 4))



/*ETHSW_MII_PDI Registers*/
#define AR10_ETHSW_MII_CFG_0             ( AR10_ETHSW_TOP + (0x36 * 4))
#define AR10_ETHSW_PCDU_0                ( AR10_ETHSW_TOP + (0x37 * 4))
#define AR10_ETHSW_MII_CFG_1             ( AR10_ETHSW_TOP + (0x38 * 4))
#define AR10_ETHSW_PCDU_1                ( AR10_ETHSW_TOP + (0x39 * 4))
#define AR10_ETHSW_MII_CFG_2             ( AR10_ETHSW_TOP + (0x3a * 4))
#define AR10_ETHSW_MII_CFG_3             ( AR10_ETHSW_TOP + (0x3c * 4))
#define AR10_ETHSW_MII_CFG_4             ( AR10_ETHSW_TOP + (0x3e * 4))
#define AR10_ETHSW_MII_CFG_5             ( AR10_ETHSW_TOP + (0x40 * 4))
#define AR10_ETHSW_PCDU_5                ( AR10_ETHSW_TOP + (0x41 * 4))

#define AR10_ETHSW_MII_CFG_RES           15      /*Hardware reset */
#define AR10_ETHSW_MII_CFG_EN            14      /*xMII interface enable*/
#define AR10_ETHSW_MII_CFG_RMII          7       /* RMII Ref clock direction input(0)/output(1) */
#define AR10_ETHSW_MII_CFG_MIIRATE       4       /*<6:4> xMII port interface clock rate bit position*/
#define AR10_ETHSW_MII_CFG_MIIRATE_M2P5  0x0     /*xMII port interface clock rate 2.5MHz */
#define AR10_ETHSW_MII_CFG_MIIRATE_M25   0x1     /*xMII port interface clock rate 25MHz */
#define AR10_ETHSW_MII_CFG_MIIRATE_M125  0x2     /*xMII port interface clock rate 125MHz */
#define AR10_ETHSW_MII_CFG_MIIRATE_M50   0x3     /*xMII port interface clock rate 50MHz */
#define AR10_ETHSW_MII_CFG_MIIRATE_AUTO  0x4     /*xMII port interface clock rate, speed used from autopolling */

#define AR10_ETHSW_MII_CFG_MIIMODE       0x0     /*<3:0> xMII  interface mode */
#define AR10_ETHSW_MII_CFG_MIIMODE_MIIM  0x0     /*MII MAC mode */
#define AR10_ETHSW_MII_CFG_MIIMODE_MIIP  0x1     /*MII PHY  mode */
#define AR10_ETHSW_MII_CFG_MIIMODE_RMIIM 0x2     /*RMII MAC mode */
#define AR10_ETHSW_MII_CFG_MIIMODE_RMIIP 0x3     /*RMII PHY mode */
#define AR10_ETHSW_MII_CFG_MIIMODE_RGMII 0x4     /*RGMII mode */

#define AR10_ETHSW_PCDU_RXSEL_CLK        12      /*<13:12>input clock selection for Receive PCDU */
#define AR10_ETHSW_PCDU_RXSEL_CLK_AUTO   0x0     /*Auto, selection of input clock depends on speed*/
#define AR10_ETHSW_PCDU_RXSEL_CLK_RXCLK  0x1     /*input clock is RXCLK*/
#define AR10_ETHSW_PCDU_RXSEL_CLK_CLKREF 0x2     /*input clock is CLKREF (125MHz)*/

#define AR10_ETHSW_PCDU_RXINIT   11      /*Reset of Receive PCDU*/
#define AR10_ETHSW_PCDU_RXPD     10      /*Power down of Receive PCDU*/
#define AR10_ETHSW_PCDU_RXDLY    7       /*<9:7> Configure receive clock delay*/


#define AR10_ETHSW_PCDU_TXSEL_CLK        5       /*<6:5>input clock selection for Transmit PCDU */
#define AR10_ETHSW_PCDU_TXSEL_CLK_AUTO   0x0     /*Auto, selection of input clock depends on speed*/
#define AR10_ETHSW_PCDU_TXSEL_CLK_RXCLK  0x1     /*input clock is TXCLK*/
#define AR10_ETHSW_PCDU_TXSEL_CLK_CLKREF 0x2     /*input clock is CLKREF (125MHz)*/

#define AR10_ETHSW_PCDU_TXINIT   4       /*Reset of Transmit PCDU*/
#define AR10_ETHSW_PCDU_TXPD     3       /*Power down of Transmit PCDU*/
#define AR10_ETHSW_PCDU_TXDLY    0       /*<2:0> Configure receive clock delay*/

/*ETHSW_PMAC_PDI Registers*/
#define AR10_ETHSW_PMAC_HD_CTL           ( AR10_ETHSW_TOP + (0x82 * 4))
#define AR10_ETHSW_PMAC_HD_CTL_RST       8  /* Remove special tag */
#define AR10_ETHSW_PMAC_HD_CTL_AST       7  /* Add special tag */
#define AR10_ETHSW_PMAC_HD_CTL_RXSH      6  /* Add status header from DMA to PMAC */
#define AR10_ETHSW_PMAC_HD_CTL_RL2       5  /* Remove Layer 2 header */
#define AR10_ETHSW_PMAC_HD_CTL_RC        4  /* Remove CRC  for packets from PMAC to DMA*/
#define AR10_ETHSW_PMAC_HD_CTL_AS        3  /* Add status header from PMAC to DMA*/
#define AR10_ETHSW_PMAC_HD_CTL_AC        2  /* Add  CRC  for packets from DMA to PMAC*/
#define AR10_ETHSW_PMAC_HD_CTL_TAG       1  /* Add  Tag, enable the VLAN tag to packets comming from DMA to PMAC and going to the switch */
#define AR10_ETHSW_PMAC_HD_CTL_ADD       0  /* Add  header  tag for packets comming from DMA to PMAC and going to the swicth*/

#define AR10_ETHSW_PMAC_TL               ( AR10_ETHSW_TOP + (0x83 * 4))
#define AR10_ETHSW_PMAC_SA1              ( AR10_ETHSW_TOP + (0x84 * 4))
#define AR10_ETHSW_PMAC_SA2              ( AR10_ETHSW_TOP + (0x85 * 4))
#define AR10_ETHSW_PMAC_SA3              ( AR10_ETHSW_TOP + (0x86 * 4))
#define AR10_ETHSW_PMAC_DA1              ( AR10_ETHSW_TOP + (0x87 * 4))
#define AR10_ETHSW_PMAC_DA2              ( AR10_ETHSW_TOP + (0x88 * 4))
#define AR10_ETHSW_PMAC_DA3              ( AR10_ETHSW_TOP + (0x89 * 4))
#define AR10_ETHSW_PMAC_VLAN             ( AR10_ETHSW_TOP + (0x8a * 4))
#define AR10_ETHSW_PMAC_VLAN_PRI         13      /*<15:13> VLAN priority */
#define AR10_ETHSW_PMAC_VLAN_CFI         12      /* CFI of the interserted VLAN tag */
#define AR10_ETHSW_PMAC_VLAN_VLAN_ID     0       /* <11:0> VLAN id */

#define AR10_ETHSW_PMAC_RX_IPG           ( AR10_ETHSW_TOP + (0x8b * 4))
#define AR10_ETHSW_PMAC_ST_ETYPE         ( AR10_ETHSW_TOP + (0x8c * 4))
#define AR10_ETHSW_PMAC_EWAN             ( AR10_ETHSW_TOP + (0x8d * 4))

#define DCDC_1V0_BASE                 0xBF106A00
#define DCDC_1V8_BASE                 0xBF106C00
#define DCDC_2V5_BASE                 0xBF106E00
#define DCDC_PWM0_OFFSET              0x24

#define PPE32_BASE  0xBE180000
#define PPE32_DEBUG_BREAK_TRACE_REG   PPE32_BASE + (0x0000 * 4)
#define PPE32_INT_MASK_STATUS_REG     PPE32_BASE + (0x0030 * 4)
#define PPE32_INT_RESOURCE_REG        PPE32_BASE + (0x0040 * 4)
#define PPE32_CDM_CODE_MEM_B0         PPE32_BASE + (0x1000 * 4)
#define PPE32_CDM_CODE_MEM_B1         PPE32_BASE + (0x2000 * 4)
#define PPE32_DATA_MEM_MAP_REG_BASE   PPE32_BASE + (0x4000 * 4)

#define PPE32_SLEEP                      PPE32_DATA_MEM_MAP_REG_BASE + (0x0010 * 4)
#define PPE32_SRST                       PPE32_DATA_MEM_MAP_REG_BASE + (0x0020 * 4)
#define PPE32_PP32_CFG                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0030 * 4)
#define PPE32_CDM_CFG                    PPE32_DATA_MEM_MAP_REG_BASE + (0x0100 * 4)
#define PPE32_MBOX_IGU0_ISRS             PPE32_DATA_MEM_MAP_REG_BASE + (0x0200 * 4)
#define PPE32_MBOX_IGU0_ISRC             PPE32_DATA_MEM_MAP_REG_BASE + (0x0201 * 4)
#define PPE32_MBOX_IGU0_ISR              PPE32_DATA_MEM_MAP_REG_BASE + (0x0202 * 4)
#define PPE32_MBOX_IGU0_IER              PPE32_DATA_MEM_MAP_REG_BASE + (0x0203 * 4)
#define PPE32_MBOX_IGU1_ISRS             PPE32_DATA_MEM_MAP_REG_BASE + (0x0204 * 4)
#define PPE32_MBOX_IGU1_ISRC             PPE32_DATA_MEM_MAP_REG_BASE + (0x0205 * 4)
#define PPE32_MBOX_IGU1_ISR              PPE32_DATA_MEM_MAP_REG_BASE + (0x0206 * 4)
#define PPE32_MBOX_IGU1_IER              PPE32_DATA_MEM_MAP_REG_BASE + (0x0207 * 4)
#define PPE32_MBOX_IGU2_ISRS             PPE32_DATA_MEM_MAP_REG_BASE + (0x0210 * 4)
#define PPE32_MBOX_IGU2_ISRC             PPE32_DATA_MEM_MAP_REG_BASE + (0x0211 * 4)
#define PPE32_MBOX_IGU2_ISR              PPE32_DATA_MEM_MAP_REG_BASE + (0x0212 * 4)
#define PPE32_MBOX_IGU2_IER              PPE32_DATA_MEM_MAP_REG_BASE + (0x0213 * 4)
#define PPE32_MBOX_IGU3_ISRS             PPE32_DATA_MEM_MAP_REG_BASE + (0x0214 * 4)
#define PPE32_MBOX_IGU3_ISRC             PPE32_DATA_MEM_MAP_REG_BASE + (0x0215 * 4)
#define PPE32_MBOX_IGU3_ISR              PPE32_DATA_MEM_MAP_REG_BASE + (0x0216 * 4)
#define PPE32_MBOX_IGU3_IER              PPE32_DATA_MEM_MAP_REG_BASE + (0x0217 * 4)
#define PPE32_MBOX_IGU4_ISRS             PPE32_DATA_MEM_MAP_REG_BASE + (0x0218 * 4)
#define PPE32_MBOX_IGU4_ISRC             PPE32_DATA_MEM_MAP_REG_BASE + (0x0219 * 4)
#define PPE32_MBOX_IGU4_ISR              PPE32_DATA_MEM_MAP_REG_BASE + (0x021A * 4)
#define PPE32_MBOX_IGU4_IER              PPE32_DATA_MEM_MAP_REG_BASE + (0x021B * 4)
#define PPE32_SB_MST_PRI0                PPE32_DATA_MEM_MAP_REG_BASE + (0x0300 * 4)
#define PPE32_SB_MST_PRI1                PPE32_DATA_MEM_MAP_REG_BASE + (0x0301 * 4)
#define PPE32_SB_MST_PRI2                PPE32_DATA_MEM_MAP_REG_BASE + (0x0302 * 4)
#define PPE32_SB_MST_PRI3                PPE32_DATA_MEM_MAP_REG_BASE + (0x0303 * 4)
#define PPE32_SB_MST_SEL                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0304 * 4)
#define PPE32_RFBI_CFG                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0400 * 4)
#define PPE32_RBA_CFG0                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0404 * 4)
#define PPE32_RBA_CFG1                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0405 * 4)
#define PPE32_RCA_CFG0                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0408 * 4)
#define PPE32_RCA_CFG1                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0409 * 4)
#define PPE32_RDES_CFG0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x040C * 4)
#define PPE32_RDES_CFG1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x040D * 4)
#define PPE32_SFSM_STATE0                PPE32_DATA_MEM_MAP_REG_BASE + (0x0410 * 4)
#define PPE32_SFSM_STATE1                PPE32_DATA_MEM_MAP_REG_BASE + (0x0411 * 4)
#define PPE32_SFSM_DBA0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0412 * 4)
#define PPE32_SFSM_DBA1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0413 * 4)
#define PPE32_SFSM_CBA0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0414 * 4)
#define PPE32_SFSM_CBA1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0415 * 4)
#define PPE32_SFSM_CFG0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0416 * 4)
#define PPE32_SFSM_CFG1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0417 * 4)
#define PPE32_SFSM_PGCNT0                PPE32_DATA_MEM_MAP_REG_BASE + (0x041C * 4)
#define PPE32_SFSM_PGCNT1                PPE32_DATA_MEM_MAP_REG_BASE + (0x041D * 4)

#define PPE32_FFSM_DBA0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0508 * 4)
#define PPE32_FFSM_DBA1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0509 * 4)
#define PPE32_FFSM_CFG0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x050A * 4)
#define PPE32_FFSM_CFG1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x050B * 4)
#define PPE32_FFSM_IDLE_HEAD_BC0         PPE32_DATA_MEM_MAP_REG_BASE + (0x050E * 4)
#define PPE32_FFSM_IDLE_HEAD_BC1         PPE32_DATA_MEM_MAP_REG_BASE + (0x050F * 4)
#define PPE32_FFSM_PGCNG0                PPE32_DATA_MEM_MAP_REG_BASE + (0x0514 * 4)
#define PPE32_FFSM_PGCNG1                PPE32_DATA_MEM_MAP_REG_BASE + (0x0515 * 4)
#define PPE32_ETOP_MDIO_CFG              PPE32_DATA_MEM_MAP_REG_BASE + (0x0600 * 4)
#define PPE32_ETOP_MDIO_ACC              PPE32_DATA_MEM_MAP_REG_BASE + (0x0601 * 4)
#define PPE32_ETOP_CFG                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0602 * 4)
#define PPE32_ETOP_IG_VLAN_COS           PPE32_DATA_MEM_MAP_REG_BASE + (0x0603 * 4)
#define PPE32_ETOP_IG_DSCP_COS3          PPE32_DATA_MEM_MAP_REG_BASE + (0x0604 * 4)
#define PPE32_ETOP_IG_DSCP_COS2          PPE32_DATA_MEM_MAP_REG_BASE + (0x0605 * 4)
#define PPE32_ETOP_IG_DSCP_COS1          PPE32_DATA_MEM_MAP_REG_BASE + (0x0606 * 4)
#define PPE32_ETOP_IG_DSCP_COS0          PPE32_DATA_MEM_MAP_REG_BASE + (0x0607 * 4)
#define PPE32_ETOP_IG_PLEN_CTRL          PPE32_DATA_MEM_MAP_REG_BASE + (0x0608 * 4)
#define PPE32_ETOP_ISR                   PPE32_DATA_MEM_MAP_REG_BASE + (0x060A * 4)
#define PPE32_ETOP_IER                   PPE32_DATA_MEM_MAP_REG_BASE + (0x060B * 4)
#define PPE32_ETOP_VPID                  PPE32_DATA_MEM_MAP_REG_BASE + (0x060C * 4)
#define PPE32_ENET_0                     PPE32_DATA_MEM_MAP_REG_BASE + (0x0610 * 4)
#define PPE32_ENET_1                     PPE32_DATA_MEM_MAP_REG_BASE + (0x0650 * 4)
#define PPE32_ENET0_MAC_CFG              PPE32_DATA_MEM_MAP_REG_BASE + (0x0610 * 4)
#define PPE32_ENET0S_DBA                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0612 * 4)
#define PPE32_ENET0S_CBA                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0613 * 4)
#define PPE32_ENET0S_CFG                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0614 * 4)
#define PPE32_ENET0S_PGCNT               PPE32_DATA_MEM_MAP_REG_BASE + (0x0615 * 4)
#define PPE32_ENET0S_PKTCNT              PPE32_DATA_MEM_MAP_REG_BASE + (0x0616 * 4)
#define PPE32_ENET0S_BUF_CTRL            PPE32_DATA_MEM_MAP_REG_BASE + (0x0617 * 4)
#define PPE32_ENET0S_COS_CFG             PPE32_DATA_MEM_MAP_REG_BASE + (0x0618 * 4)
#define PPE32_ENET0S_IGDROP              PPE32_DATA_MEM_MAP_REG_BASE + (0x0619 * 4)
#define PPE32_ENET0S_IGERR               PPE32_DATA_MEM_MAP_REG_BASE + (0x061A * 4)
#define PPE32_ENET0S_MAC_DA0             PPE32_DATA_MEM_MAP_REG_BASE + (0x061B * 4)
#define PPE32_ENET0S_MAC_DA1             PPE32_DATA_MEM_MAP_REG_BASE + (0x061C * 4)
#define PPE32_ENET0F_DBA                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0630 * 4)
#define PPE32_ENET0F_CBA                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0631 * 4)
#define PPE32_ENET0F_CFG                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0632 * 4)
#define PPE32_ENET0F_PGCNT               PPE32_DATA_MEM_MAP_REG_BASE + (0x0633 * 4)
#define PPE32_ENET0F_PKTCNT              PPE32_DATA_MEM_MAP_REG_BASE + (0x0634 * 4)
#define PPE32_ENET0F_HFCTRL              PPE32_DATA_MEM_MAP_REG_BASE + (0x0635 * 4)
#define PPE32_ENET0F_TXCTRL              PPE32_DATA_MEM_MAP_REG_BASE + (0x0636 * 4)
#define PPE32_ENET0F_VLCOS0              PPE32_DATA_MEM_MAP_REG_BASE + (0x0638 * 4)
#define PPE32_ENET0F_VLCOS1              PPE32_DATA_MEM_MAP_REG_BASE + (0x0639 * 4)
#define PPE32_ENET0F_VLCOS2              PPE32_DATA_MEM_MAP_REG_BASE + (0x063A * 4)
#define PPE32_ENET0F_VLCOS3              PPE32_DATA_MEM_MAP_REG_BASE + (0x063B * 4)
#define PPE32_ENET0F_EGCOL               PPE32_DATA_MEM_MAP_REG_BASE + (0x063C * 4)
#define PPE32_ENET0F_EGDROP              PPE32_DATA_MEM_MAP_REG_BASE + (0x063D * 4)
#define PPE32_DMRX_DBA			 PPE32_DATA_MEM_MAP_REG_BASE + (0x0612 * 4)
#define PPE32_DMRX_CBA			 PPE32_DATA_MEM_MAP_REG_BASE + (0x0613 * 4)
#define PPE32_DMRX_CFG           PPE32_DATA_MEM_MAP_REG_BASE + (0x0614 * 4)
#define PPE32_DSRX_DB			 PPE32_DATA_MEM_MAP_REG_BASE + (0x0710 * 4)
#define PPE32_DSRX_CB			 PPE32_DATA_MEM_MAP_REG_BASE + (0x0711 * 4)
#define PPE32_DSRX_CFG                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0712 * 4)

// #define PPE32_ENET1_MAC_CFG              PPE32_DATA_MEM_MAP_REG_BASE + (0x0650 * 4)
// #define PPE32_ENET1S_DBA                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0652 * 4)
// #define PPE32_ENET1S_CBA                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0653 * 4)
// #define PPE32_ENET1S_CFG                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0654 * 4)
// #define PPE32_ENET1S_PGCNT               PPE32_DATA_MEM_MAP_REG_BASE + (0x0655 * 4)
// #define PPE32_ENET1S_PKTCNT              PPE32_DATA_MEM_MAP_REG_BASE + (0x0656 * 4)
// #define PPE32_ENET1S_BUF_CTRL            PPE32_DATA_MEM_MAP_REG_BASE + (0x0657 * 4)
// #define PPE32_ENET1S_COS_CFG             PPE32_DATA_MEM_MAP_REG_BASE + (0x0658 * 4)
// #define PPE32_ENET1S_IGDROP              PPE32_DATA_MEM_MAP_REG_BASE + (0x0659 * 4)
// #define PPE32_ENET1S_IGERR               PPE32_DATA_MEM_MAP_REG_BASE + (0x065A * 4)
// #define PPE32_ENET1S_MAC_DA0             PPE32_DATA_MEM_MAP_REG_BASE + (0x065B * 4)
// #define PPE32_ENET1S_MAC_DA1             PPE32_DATA_MEM_MAP_REG_BASE + (0x065C * 4)
// #define PPE32_ENET1F_DBA                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0670 * 4)
// #define PPE32_ENET1F_CBA                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0671 * 4)
// #define PPE32_ENET1F_CFG                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0672 * 4)
// #define PPE32_ENET1F_PGCNT               PPE32_DATA_MEM_MAP_REG_BASE + (0x0673 * 4)
// #define PPE32_ENET1F_PKTCNT              PPE32_DATA_MEM_MAP_REG_BASE + (0x0674 * 4)
// #define PPE32_ENET1F_HFCTRL              PPE32_DATA_MEM_MAP_REG_BASE + (0x0675 * 4)
// #define PPE32_ENET1F_TXCTRL              PPE32_DATA_MEM_MAP_REG_BASE + (0x0676 * 4)
// #define PPE32_ENET1F_VLCOS0              PPE32_DATA_MEM_MAP_REG_BASE + (0x0678 * 4)
// #define PPE32_ENET1F_VLCOS1              PPE32_DATA_MEM_MAP_REG_BASE + (0x0679 * 4)
// #define PPE32_ENET1F_VLCOS2              PPE32_DATA_MEM_MAP_REG_BASE + (0x067A * 4)
// #define PPE32_ENET1F_VLCOS3              PPE32_DATA_MEM_MAP_REG_BASE + (0x067B * 4)
// #define PPE32_ENET1F_EGCOL               PPE32_DATA_MEM_MAP_REG_BASE + (0x067C * 4)
// #define PPE32_ENET1F_EGDROP              PPE32_DATA_MEM_MAP_REG_BASE + (0x067D * 4)

#define PPE32_DPLUS_TXDB                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0700 * 4)
#define PPE32_DPLUS_TXCB                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0701 * 4)
#define PPE32_DPLUS_TXCFG                PPE32_DATA_MEM_MAP_REG_BASE + (0x0702 * 4)
#define PPE32_DPLUS_TXPGCNT              PPE32_DATA_MEM_MAP_REG_BASE + (0x0703 * 4)
#define PPE32_DPLUS_RXDB                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0710 * 4)
#define PPE32_DPLUS_RXCB                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0711 * 4)
#define PPE32_DPLUS_RXCFG                PPE32_DATA_MEM_MAP_REG_BASE + (0x0712 * 4)
#define PPE32_DPLUS_RXPGCNT              PPE32_DATA_MEM_MAP_REG_BASE + (0x0713 * 4)
#define PPE32_BMC_CFG0                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0800 * 4)
#define PPE32_BMC_POLY0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0801 * 4)
#define PPE32_BMC_CFG1                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0802 * 4)
#define PPE32_BMC_CRC0                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0803 * 4)
#define PPE32_BMC_POLY1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0804 * 4)
#define PPE32_BMC_CRC1                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0805 * 4)
#define PPE32_BMC_CMD3                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0806 * 4)
#define PPE32_BMC_CMD2                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0807 * 4)
#define PPE32_BMC_CMD1                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0808 * 4)
#define PPE32_BMC_CMD0                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0809 * 4)
#define PPE32_SLL_CMD1                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0900 * 4)
#define PPE32_SLL_CMD0                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0901 * 4)
#define PPE32_SLL_KEY0                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0910 * 4)
#define PPE32_SLL_KEY1                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0911 * 4)
#define PPE32_SLL_KEY2                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0912 * 4)
#define PPE32_SLL_KEY3                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0913 * 4)
#define PPE32_SLL_KEY4                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0914 * 4)
#define PPE32_SLL_KEY5                   PPE32_DATA_MEM_MAP_REG_BASE + (0x0915 * 4)
#define PPE32_SLL_RESULT                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0920 * 4)
#define PPE32_SLL1_CMD1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B00 * 4)
#define PPE32_SLL1_CMD0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B01 * 4)
#define PPE32_SLL1_KEY0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B10 * 4)
#define PPE32_SLL1_KEY1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B11 * 4)
#define PPE32_SLL1_KEY2                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B12 * 4)
#define PPE32_SLL1_KEY3                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B13 * 4)
#define PPE32_SLL1_KEY4                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B14 * 4)
#define PPE32_SLL1_KEY5                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B15 * 4)
#define PPE32_SLL1_RESULT                PPE32_DATA_MEM_MAP_REG_BASE + (0x0B20 * 4)
#define PPE32_EMA_CMDCFG                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0A00 * 4)
#define PPE32_EMA_DATACFG                PPE32_DATA_MEM_MAP_REG_BASE + (0x0A01 * 4)
#define PPE32_EMA_CMDCNT                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0A02 * 4)
#define PPE32_EMA_DATACNT                PPE32_DATA_MEM_MAP_REG_BASE + (0x0A03 * 4)
#define PPE32_EMA_ISR                    PPE32_DATA_MEM_MAP_REG_BASE + (0x0A04 * 4)
#define PPE32_EMA_IER                    PPE32_DATA_MEM_MAP_REG_BASE + (0x0A05 * 4)
#define PPE32_EMA_CFG                    PPE32_DATA_MEM_MAP_REG_BASE + (0x0A06 * 4)
#define PPE32_EMA_SUBID                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0A07 * 4)

// #define PPE32_UTP_TXCA0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B00 * 4)
// #define PPE32_UTP_TXCA1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B01 * 4)
// #define PPE32_UTP_TXNA0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B02 * 4)
// #define PPE32_UTP_TXNA1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B03 * 4)
// #define PPE32_UTP_RXCA0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B10 * 4)
// #define PPE32_UTP_RXCA1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B11 * 4)
// #define PPE32_UTP_RXNA0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B12 * 4)
// #define PPE32_UTP_RXNA1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0B13 * 4)
// #define PPE32_UTP_CFG                    PPE32_DATA_MEM_MAP_REG_BASE + (0x0B20 * 4)
// #define PPE32_UTP_ISR                    PPE32_DATA_MEM_MAP_REG_BASE + (0x0B30 * 4)
// #define PPE32_UTP_IER                    PPE32_DATA_MEM_MAP_REG_BASE + (0x0B31 * 4)

#define PPE32_QSB_RELOG                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0C00 * 4)
#define PPE32_QSB_EMIT0                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0C01 * 4)
#define PPE32_QSB_EMIT1                  PPE32_DATA_MEM_MAP_REG_BASE + (0x0C02 * 4)
#define PPE32_DREG_A_VERSION             PPE32_DATA_MEM_MAP_REG_BASE + (0x0D00 * 4)
#define PPE32_DREG_A_CFG                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0D01 * 4)
#define PPE32_DREG_AT_CTRL               PPE32_DATA_MEM_MAP_REG_BASE + (0x0D02 * 4)
#define PPE32_DREG_AT_CB_CFG0            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D03 * 4)
#define PPE32_DREG_AT_CB_CFG1            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D04 * 4)
#define PPE32_DREG_AR_CTRL               PPE32_DATA_MEM_MAP_REG_BASE + (0x0D08 * 4)
#define PPE32_DREG_AR_CB_CFG0            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D09 * 4)
#define PPE32_DREG_AR_CB_CFG1            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D0A * 4)
#define PPE32_DREG_A_UTPCFG              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D0E * 4)
#define PPE32_DREG_A_STATUS              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D0F * 4)
#define PPE32_DREG_AT_CFG0               PPE32_DATA_MEM_MAP_REG_BASE + (0x0D20 * 4)
#define PPE32_DREG_AT_CFG1               PPE32_DATA_MEM_MAP_REG_BASE + (0x0D21 * 4)
#define PPE32_DREG_AT_FB_SIZE0           PPE32_DATA_MEM_MAP_REG_BASE + (0x0D22 * 4)
#define PPE32_DREG_AT_FB_SIZE1           PPE32_DATA_MEM_MAP_REG_BASE + (0x0D23 * 4)
#define PPE32_DREG_AT_CELL0              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D24 * 4)
#define PPE32_DREG_AT_CELL1              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D25 * 4)
#define PPE32_DREG_AT_IDLE_CNT0          PPE32_DATA_MEM_MAP_REG_BASE + (0x0D26 * 4)
#define PPE32_DREG_AT_IDLE_CNT1          PPE32_DATA_MEM_MAP_REG_BASE + (0x0D27 * 4)
#define PPE32_DREG_AT_IDLE0              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D28 * 4)
#define PPE32_DREG_AT_IDLE1              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D29 * 4)
#define PPE32_DREG_AR_CFG0               PPE32_DATA_MEM_MAP_REG_BASE + (0x0D60 * 4)
#define PPE32_DREG_AR_CFG1               PPE32_DATA_MEM_MAP_REG_BASE + (0x0D61 * 4)
#define PPE32_DREG_AR_FB_STAT0           PPE32_DATA_MEM_MAP_REG_BASE + (0x0D62 * 4)
#define PPE32_DREG_AR_FB_STAT1           PPE32_DATA_MEM_MAP_REG_BASE + (0x0D63 * 4)
#define PPE32_DREG_AR_FB_END0            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D64 * 4)
#define PPE32_DREG_AR_FB_END1            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D65 * 4)
#define PPE32_DREG_AR_ATM_STAT0          PPE32_DATA_MEM_MAP_REG_BASE + (0x0D66 * 4)
#define PPE32_DREG_AR_ATM_STAT1          PPE32_DATA_MEM_MAP_REG_BASE + (0x0D67 * 4)
#define PPE32_DREG_AR_CELL0              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D68 * 4)
#define PPE32_DREG_AR_CELL1              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D69 * 4)
#define PPE32_DREG_AR_IDLE_CNT0          PPE32_DATA_MEM_MAP_REG_BASE + (0x0D6A * 4)
#define PPE32_DREG_AR_IDLE_CNT1          PPE32_DATA_MEM_MAP_REG_BASE + (0x0D6B * 4)
#define PPE32_DREG_AR_AIIDLE_CNT0        PPE32_DATA_MEM_MAP_REG_BASE + (0x0D6C * 4)
#define PPE32_DREG_AR_AIIDLE_CNT1        PPE32_DATA_MEM_MAP_REG_BASE + (0x0D6D * 4)
#define PPE32_DREG_AR_BE_CNT0            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D6E * 4)
#define PPE32_DREG_AR_BE_CNT1            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D6F * 4)
#define PPE32_DREG_AR_HEC_CNT0           PPE32_DATA_MEM_MAP_REG_BASE + (0x0D70 * 4)
#define PPE32_DREG_AR_HEC_CNT1           PPE32_DATA_MEM_MAP_REG_BASE + (0x0D71 * 4)
#define PPE32_DREG_AR_CD_CNT0            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D72 * 4)
#define PPE32_DREG_AR_CD_CNT1            PPE32_DATA_MEM_MAP_REG_BASE + (0x0D73 * 4)
#define PPE32_DREG_AR_IDLE0              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D74 * 4)
#define PPE32_DREG_AR_IDLE1              PPE32_DATA_MEM_MAP_REG_BASE + (0x0D75 * 4)
#define PPE32_DREG_AR_DELIN0             PPE32_DATA_MEM_MAP_REG_BASE + (0x0D76 * 4)
#define PPE32_DREG_AR_DELIN1             PPE32_DATA_MEM_MAP_REG_BASE + (0x0D77 * 4)
#define PPE32_DREG_RESV0                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0D78 * 4)
#define PPE32_DREG_RESV1                 PPE32_DATA_MEM_MAP_REG_BASE + (0x0D79 * 4)
#define PPE32_DREG_RX_MIB_CMD0           PPE32_DATA_MEM_MAP_REG_BASE + (0x0D80 * 4)
#define PPE32_DREG_RX_MIB_CMD1           PPE32_DATA_MEM_MAP_REG_BASE + (0x0D81 * 4)
#define PPE32_DREG_AR_OVDROP_CNT0        PPE32_DATA_MEM_MAP_REG_BASE + (0x0D98 * 4)
#define PPE32_DREG_AR_OVDROP_CNT1        PPE32_DATA_MEM_MAP_REG_BASE + (0x0D99 * 4)

#define PPE32_CDM_PRIV_DATA_MEM_B1    PPE32_BASE + (0x5000 * 4)
#define PPE32_PPM_INT_UNIT            PPE32_BASE + (0x6000 * 4)
#define PPE32_PPM_TIMER0              PPE32_BASE + (0x6100 * 4)
#define PPE32_PPM_TASK_INDEP_REG      PPE32_BASE + (0x6200 * 4)
#define PPE32_PPS_BRK                 PPE32_BASE + (0x6300 * 4)
#define PPE32_PPM_TIMER1              PPE32_BASE + (0x6400 * 4)
#define PPE32_SB_RAM_B0	              PPE32_BASE + (0x8000 * 4) 
#define PPE32_SB_RAM_B1               PPE32_BASE + (0x8800 * 4)
#define PPE32_SB_RAM_B2               PPE32_BASE + (0x9000 * 4)
#define PPE32_SB_RAM_B3               PPE32_BASE + (0x9800 * 4)
#define PPE32_SB_DFEMEM_START         PPE32_BASE + (0xA000 * 4)
#define PPE32_SB_DFEMEM_END           PPE32_BASE + (0xABFF * 4)
#define PPE32_QSB_CFG_REG             PPE32_BASE + (0xC000 * 4)

#define PPE32_DATA_MEM_INT               0x0000
#define PPE32_CMD_PRIV_DATA_MEM_B1_INT   0x1000
#define PPE32_SB_RAM_B0_INT              0x2000
#define PPE32_SB_RAM_B1_INT              0x2800
#define PPE32_SB_RAM_B2_INT              0x3000
#define PPE32_SB_RAM_B3_INT              0x3800
#define PPE32_SB_DFEMEM_INT              0x4000  
//PROGRAMMED BY MIPS - QSB REGBANK
#define MIPS_QSB_ICDV_REG            PPE32_BASE + (0xC007 * 4)
#define MIPS_QSB_SBL_REG             PPE32_BASE + (0xC009 * 4)
#define MIPS_QSB_CFG_REG             PPE32_BASE + (0xC00A * 4)
#define MIPS_QSB_RTM_REG             PPE32_BASE + (0xC00B * 4)
#define MIPS_QSB_RTD_REG             PPE32_BASE + (0xC00C * 4)
#define MIPS_QSB_RAMAC_REG           PPE32_BASE + (0xC00D * 4)
#define MIPS_QSB_ISTAT_REG           PPE32_BASE + (0xC00E * 4)
#define MIPS_QSB_IMR_REG             PPE32_BASE + (0xC00F * 4)
#define MIPS_QSB_SCR_REG             PPE32_BASE + (0xC010 * 4)

/***********************************************************************/
/*  Module      :  CHIPID register address and bits                    */
/***********************************************************************/
#define AR10_CHIPID_BASE     		0xBF107340
#define AR10_MPS_MANID				AR10_CHIPID_BASE
#define AR10_MPS_CHIPID				(AR10_CHIPID_BASE + 0x4)
#define AR10_CHIPID   				(AR10_CHIPID_BASE + 0x8) 
#define AR10_ID_CFG					(AR10_CHIPID_BASE + 0x10)

/***********************************************************************/
/*  Module      :  Cross Bar register address and bits                       */
/***********************************************************************/

#define AR10_CSS                          (0xBF400000)
/***********************************************************************/

/***CSS Control***/
#define AR10_CSS_AL_CONTROL                      ((volatile u32*)(AR10_CSS + 0x0430))


/***********************************************************************/
/*  Module      :  Cross Bar register address and bits                       */
/***********************************************************************/

#define AR10_ICU_BASE                     (0xBF880200)
/***********************************************************************/

#define AR10_ICU_IM3_ISR                  ((volatile u32*)(AR10_ICU_BASE + 0x0078))



#define AR10_REG32(addr)                 *((volatile u32 *)(addr))
#define AR10_REG16(addr)                 *((volatile u16 *)(addr))
#define AR10_REG8(addr)                  *((volatile u8 *)(addr))
#define REG32(addr)                 *((volatile u32 *)(addr))
#define REG16(addr)                 *((volatile u16 *)(addr))
#define REG8(addr)                  *((volatile u8 *)(addr))
/***********************************************************************/
#endif //AR10_H 
