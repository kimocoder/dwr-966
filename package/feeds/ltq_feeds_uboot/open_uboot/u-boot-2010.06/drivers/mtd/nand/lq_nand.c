/******************************************************************************
 * **
 * ** FILE NAME    : lq_nand.c
 * ** PROJECT      : Latiq UEIP
 * ** MODULES      : NAND Flash
 * **
 * ** DATE         : 23 Apr 2005
 * ** AUTHOR       : Wu Qi Ming
 * ** DESCRIPTION  : NAND Flash MTD Driver
 * ** COPYRIGHT    :       Copyright (c) 2010
 * **                      Lantiq Technologies 
 * **                      
 * **
 * **    This program is free software; you can redistribute it and/or modify
 * **    it under the terms of the GNU General Public License as published by
 * **    the Free Software Foundation; either version 2 of the License, or
 * **    (at your option) any later version.
 * **
 * ** HISTORY
 * ** $Date        $Author      $Version   $Comment
 * ** 23 Apr 2008  Wu Qi Ming   1.0        initial version
 * ** 27 Jul 2010  Wu Qi Ming   2.0        modified for u-boot-2010.06
 * *******************************************************************************/

#include <common.h>


#include <nand.h>
#include <asm/errno.h>
#include <asm/addrspace.h>
#ifdef CONFIG_NAND_ECC_HW_REED_SOLOMON 
#include <asm/lq_dma.h>
#define NUM_RX_DESC 2
#define NUM_TX_DESC 2
#define TX_CHAN_NO   15
#define RX_CHAN_NO   14
#define MAX_PACKET_SIZE 	8192

extern void flush_dcache_range(unsigned long start, unsigned long stop);
extern void clean_dcache_range(unsigned long start, unsigned long stop);
extern void invalidate_dcache_range(unsigned long start, unsigned long stop);
extern void flush_dcache(void);
extern void invalidate_dcache(void);
extern void invalidate_icache(void);


static int hsnand_tx_num, hsnand_rx_num;
static dma_rx_descriptor_t hsnand_rx_des_ring[NUM_RX_DESC] __attribute__ ((aligned(16)));
static dma_tx_descriptor_t hsnand_tx_des_ring[NUM_TX_DESC] __attribute__ ((aligned(16)));
static int current_cmd =  NAND_CMD_NONE;
static int addr_cnt;

u8 g_num2alpha[256] = {0,0,1,99,2,198,100,106,3,205,199,188,101,126,107,42,4,141,206,
             78,200,212,189,225,102,221,127,49,108,32,43,243,5,87,142,232,207,
             172,79,131,201,217,213,65,190,148,226,180,103,39,222,240,128,177,
             50,53,109,69,33,18,44,13,244,56,6,155,88,26,143,121,233,112,208,
             194,173,168,80,117,132,72,202,252,218,138,214,84,66,36,191,152,149,
             249,227,94,181,21,104,97,40,186,223,76,241,47,129,230,178,63,51,
             238,54,16,110,24,70,166,34,136,19,247,45,184,14,61,245,164,57,59,7,
             158,156,157,89,159,27,8,144,9,122,28,234,160,113,90,209,29,195,123,
             174,10,169,145,81,91,118,114,133,161,73,235,203,124,253,196,219,30,
             139,210,215,146,85,170,67,11,37,175,192,115,153,119,150,92,250,82,
             228,236,95,74,182,162,22,134,105,197,98,254,41,125,187,204,224,211,
             77,140,242,31,48,220,130,171,231,86,179,147,64,216,52,176,239,38,
             55,12,17,68,111,120,25,154,71,116,167,193,35,83,137,251,20,93,248,
             151,46,75,185,96,15,237,62,229,246,135,165,23,58,163,60,183};

u8 g_alpha2num[256] = {1,2,4,8,16,32,64,128,135,137,149,173,221,61,122,244,111,222,59,118,
             236,95,190,251,113,226,67,134,139,145,165,205,29,58,116,232,87,174,
             219,49,98,196,15,30,60,120,240,103,206,27,54,108,216,55,110,220,63,
             126,252,127,254,123,246,107,214,43,86,172,223,57,114,228,79,158,187,
             241,101,202,19,38,76,152,183,233,85,170,211,33,66,132,143,153,181,
             237,93,186,243,97,194,3,6,12,24,48,96,192,7,14,28,56,112,224,71,142,
             155,177,229,77,154,179,225,69,138,147,161,197,13,26,52,104,208,39,
             78,156,191,249,117,234,83,166,203,17,34,68,136,151,169,213,45,90,
             180,239,89,178,227,65,130,131,129,133,141,157,189,253,125,250,115,
             230,75,150,171,209,37,74,148,175,217,53,106,212,47,94,188,255,121,
             242,99,198,11,22,44,88,176,231,73,146,163,193,5,10,20,40,80,160,
             199,9,18,36,72,144,167,201,21,42,84,168,215,41,82,164,207,25,50,
             100,200,23,46,92,184,247,105,210,35,70,140,159,185,245,109,218,51,
             102,204,31,62,124,248,119,238,91,182,235,81,162,195,1};


static uint8_t scan_ff_pattern[] = { 0xff, 0xff };

static struct nand_bbt_descr smallpage_memorybased = {
    .options = NAND_BBT_SCAN2NDPAGE,
    .offs = 5,
    .len = 1,
    .pattern = scan_ff_pattern
};

static struct nand_bbt_descr largepage_memorybased = {
    .options = 0,
    .offs = 0,
    .len = 2,
    .pattern = scan_ff_pattern
};

static struct nand_bbt_descr smallpage_flashbased = {
    .options = NAND_BBT_SCAN2NDPAGE,
    .offs = 5,
    .len = 1,
    .pattern = scan_ff_pattern
};

static struct nand_bbt_descr largepage_flashbased = {
    .options = NAND_BBT_SCAN2NDPAGE,
    .offs = 0,
    .len = 2,
    .pattern = scan_ff_pattern
};

static uint8_t bbt_pattern[] = {'B', 'b', 't', '0' };
static uint8_t mirror_pattern[] = {'1', 't', 'b', 'B' };

static struct nand_bbt_descr bbt_main_descr = {
    .options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
        | NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
    .offs = 8,
    .len = 4,
    .veroffs = 12,
    .maxblocks = 4,
    .pattern = bbt_pattern
};

static struct nand_bbt_descr bbt_mirror_descr = {
    .options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
        | NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
    .offs = 8,
    .len = 4,
    .veroffs = 12,
    .maxblocks = 4,
    .pattern = mirror_pattern
};



#endif

static u32 latchcmd=0;

static void ifx_nand_write_byte(struct mtd_info *mtd, u_char byte);
extern void nand_gpio_init(void);

#ifdef CONFIG_NAND_SPL
extern int nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code);
extern int nand_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc);

static void lq_enable_hwecc(struct mtd_info *mtd, int mode)
{
    return;
}
	
#endif
static void ifx_nand_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
     struct nand_chip *chip = mtd->priv;
     if(ctrl & NAND_CTRL_CHANGE){
        if( ctrl & NAND_ALE ){
				 NAND_CLRCLE;
        	     NAND_SETALE;
		         latchcmd = WRITE_ADDR;
		        }
		    else if( ctrl & NAND_CLE ){
		    	   NAND_CLRALE;
		    	   NAND_SETCLE;
		         latchcmd = WRITE_CMD;
		        }
           else{
		       NAND_CLRALE;
			     NAND_CLRCLE;
			     latchcmd = WRITE_DATA;
		       }
	   }
	   if(cmd!=NAND_CMD_NONE){
#ifdef  CONFIG_NAND_ECC_HW_REED_SOLOMON
         u32 PiB, page;
		 /*if it is a readpage or writepage command, store it in current_cmd*/
		 if((cmd == NAND_CMD_SEQIN || cmd == NAND_CMD_READ0) && (ctrl & NAND_CLE)){
	       current_cmd = cmd;
	       addr_cnt=0;
			   /*decide number of pages in one block, as required by ND_PARA0*/
			   switch(mtd->erasesize/mtd->writesize){
                   case 32:
				      PiB = 0;
					  break;
				   case 64:
				      PiB = 1;
					  break;
				   case	128:
				      PiB = 2;
					  break;
				   case 256:
				      PiB = 3;
					  break;
				   default:
				      PiB = 0; /*should not happen*/
					  break;

			   }
         switch(mtd->writesize){
                   case 512:
				      page = 0;
				      break;
				   case 2048:
				      page = 1;
					  break;
				   case 4096:
				      page = 2;
					  break;
				   case 8192:
				      page=3;
					  break;
			   }
			   
			   *BSP_NDAC_CTL1 = (u32)(cmd & 0xff); /*write command to the LSB, set other fields to be 0*/              
			   //*BSP_NDAC_CTL2 = 0x04000000;
			   *BSP_NDAC_CTL2 = 0;
			   /*works on single plane, single operation, ECC in spare area, 4 byte ecc modes
			     User should change the setting according to their own needs*/  
#ifdef CONFIG_NAND_ECC_HW_4BYTES			  
			  *BSP_ND_PARA0  = ((chip->onfi_version?1:0) & 0x1)<<18 | 1<<8 | 1<<6 | (PiB & 0x3)<<4 | (page & 0xf);
#else /*3 bytes ECC*/
              *BSP_ND_PARA0  = ((chip->onfi_version?1:0) & 0x1)<<18 | 1<<6 | (PiB & 0x3)<<4 | (page & 0xf);  
#endif

#ifdef  CONFIG_NAND_CS0
			   *BSP_MD_CTL= 0x00000009 | ((cmd == NAND_CMD_SEQIN)?0x400:0);  /*Write operation, DMA enable mode, chip select 0 */
#else
			   *BSP_MD_CTL= 0x00000011 | ((cmd == NAND_CMD_SEQIN)?0x400:0);  /*write operation, DMA enable mode, chip select 1 */
#endif
			} /*current_cmd=read or write*/
		   else if( (cmd==NAND_CMD_READSTART) && (ctrl & NAND_CLE) ){
			  current_cmd = NAND_CMD_NONE;
			  *BSP_NDAC_CTL2 |=((addr_cnt==5)?2:1)<<16;	  
		   }
		   else if((cmd == NAND_CMD_PAGEPROG) && (ctrl & NAND_CLE) && (addr_cnt != 0)) {
                /*skip program command if the operation is done by dma*/
                /*addr_cnt will be cleared after raw write to the page*/ 
                addr_cnt = 0;
				current_cmd = NAND_CMD_NONE;
		   }
		   else{
              if( current_cmd != NAND_CMD_NONE ){
                  /*It is an address byte, record it in the register*/  
				  switch(addr_cnt){
                       case 0:
                           *BSP_NDAC_CTL1 |= (cmd & 0xff) <<8;
						   addr_cnt++;
						   break;
					   case 1:
					       *BSP_NDAC_CTL1 |= (cmd & 0xff) <<16;
					       addr_cnt++;
						   break;
					   case 2:
					       *BSP_NDAC_CTL1 |= (cmd & 0xff) <<24;
					       addr_cnt++;
						   break;
                       case 3:
					       *BSP_NDAC_CTL2 |= cmd & 0xff;
						   addr_cnt++;
                           break;
					   case 4:
					       *BSP_NDAC_CTL2 |= (cmd & 0xff)<<8;
						   addr_cnt++;
					       break;
				  }
			  }else{
			    /*normal command or address byte, just write to the nand controller*/   
                ifx_nand_write_byte(mtd, cmd);
			  }
		   }

#else /*!CONFIG_NAND_ECC_HW_REED_SOLOMON*/	   	
         ifx_nand_write_byte(mtd, cmd);
     
#endif      
      }
     return;
}



static int NanD_WaitReady(struct mtd_info *mtd)
{
      struct nand_chip *nand = mtd->priv;
	  NAND_WAIT_READY(nand);
	  return 1;
}


/**
 * nand_block_bad - [DEFAULT] Read bad block marker from the chip
 * @mtd:        MTD device structure
 * @ofs:        offset from device start
 * @getchip:    0, if the chip is already selected
 *
 * Check, if the block is bad.
 */
static int ifx_nand_block_bad(struct mtd_info *mtd, loff_t ofs, int getchip)
{
      return 0;
}


/**
 * amazon_s_nand_read_byte -  read one byte from the chip
 * @mtd:        MTD device structure
 *
 *  read function for 8bit buswith
 */
static u_char ifx_nand_read_byte(struct mtd_info *mtd)
{
      struct nand_chip *nand = mtd->priv;
      u_char ret;
      asm("sync");
      NAND_READ(READ_DATA, ret); 
      asm("sync");
      //printf("read_byte: %02x\n",ret);
      return ret;
}

/**
 * amazon_s_nand_read_buf - [DEFAULT] read chip data into buffer
 * @mtd:        MTD device structure
 * @buf:        buffer to store date
 * @len:        number of bytes to read
 *
 * Default read function for 8bit buswith
 */
static void ifx_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
        int i;
        struct nand_chip *chip = mtd->priv;

        for (i = 0; i < len; i++){
		         NAND_READ(READ_DATA, buf[i]);
				}

}


/**
 * nand_write_byte - [DEFAULT] write one byte to the chip
 * @mtd:        MTD device structure
 * @byte:       pointer to data byte to write
 *
 * Default write function for 8it buswith
 */
static void ifx_nand_write_byte(struct mtd_info *mtd, u_char byte)
{
      struct nand_chip *this = mtd->priv;
      *(volatile u8*)((u32)this->IO_ADDR_W | latchcmd)=byte;
      while((*EBU_NAND_WAIT & 0x08) == 0);
}
								

#ifndef CONFIG_NAND_SPL
/**
 * amazon_s_nand_write_buf - [DEFAULT] write buffer to chip
 * @mtd:        MTD device structure
 * @buf:        data buffer
 * @len:        number of bytes to write
 *
 * Default write function for 8bit buswith
 */
static void ifx_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
        int i;
        struct nand_chip *chip = mtd->priv;

        for (i = 0; i < len; i++){
                  //printf("x%02x", buf[i]);                 
 				  WRITE_NAND(buf[i],chip->IO_ADDR_W); 
			   }
             // writeb(buf[i], chip->IO_ADDR_W);
}



/**
 * amazon_s_nand_verify_buf - [DEFAULT] Verify chip data against buffer
 * @mtd:        MTD device structure
 * @buf:        buffer containing the data to compare
 * @len:        number of bytes to compare
 *
 * Default verify function for 8bit buswith
 */
static int ifx_nand_verify_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
        int i;
		uint8_t tmp;
        struct nand_chip *chip = mtd->priv;

        for (i = 0; i < len; i++){
		        NAND_READ(READ_DATA, tmp);
                if ( buf[i] != tmp ) return -EFAULT;
			}
        return 0;
}

#endif
/**
 * amazon_s_nand_select_chip - control -CE line
 *      Forbid driving -CE manually permitting the NAND controller to do this.
 *      Keeping -CE asserted during the whole sector reads interferes with the
 *      NOR flash and PCMCIA drivers as it causes contention on the static bus.
 *      We only have to hold -CE low for the NAND read commands since the flash
 *      chip needs it to be asserted during chip not ready time but the NAND
 *      controller keeps it released.
 *
 * @mtd:        MTD device structure
 * @chip:       chipnumber to select, -1 for deselect
 */
static void ifx_nand_select_chip(struct mtd_info *mtd, int chip)
{
      struct nand_chip *nand = mtd->priv;

        switch (chip) {
        case -1:
                NAND_CE_CLEAR;
				*EBU_NAND_CON &=~(0x1);
			    break;
        case 0:
		        *EBU_NAND_CON |=(0x1);
				NAND_CE_SET;
				break;
        default:
               break;
        }
 

}

#ifdef CONFIG_NAND_ECC_HW

u8 g_mul(u8 arg1,u8 arg2) {
   u8 s=0;
   if ((arg1== 0) || (arg2==0)) {
                return 0;
          };
         s = (s + g_num2alpha[arg1]) % 255;
         s = (s + g_num2alpha[arg2]) % 255;
    return g_alpha2num[s];
};


u8 g_add(u8 arg3,u8 arg4) {
       u8 s=0;
       s = s ^ arg3;
       s = s ^ arg4;
       return s;    //add in G math is XOR
};

u8 *reed_solomn_128bytes_ecc(u8 *data_bytes_partial) {
   u8 g[4];
   u8 temp[4];
   u8 degree;
   u8 bytes;
   u8 y;
   u8 i;
   u8 s[4] = {0,0,0,0};
   u8 t[3];
#ifdef CONFIG_NAND_ECC_HW_4BYTES 
   g[3] = 205;
   g[2] =  63;
   g[1] =  92;
   g[0] =  32;
   degree = 4;
   bytes = 128; 
   for (i=0;i<=bytes-1;i++) {
               y     = g_add( s[3], data_bytes_partial[i] );
               temp[1]  = g_mul( y, g[3] );
               temp[2]  = g_mul( y, g[2]);
               temp[3]  = g_mul( y, g[1] );
               s[3]  = g_add( s[2], temp[1] );
               s[2]  = g_add( s[1], temp[2] );
               s[1]  = g_add( s[0], temp[3] );
               s[0]  = g_mul( y, g[0] );
        };
#else /*3 bytes mode*/
   g[3] = 0;
   g[2] = g_add(g_add(2,4), 8);
   g[1] = g_add(g_mul(2,4), g_add(g_mul(4,8), g_mul(2,8)));
   g[0] = g_mul(g_mul(2,4),8);
   bytes = 128;
   for (i=0;i<=bytes-1;i++) {
   y   = g_add( s[2], data_bytes_partial[i] );
   temp[2]  = g_mul( y, g[2]);
   temp[3]  = g_mul( y, g[1] );
   s[2]  = g_add( s[1], temp[2] );
   s[1]  = g_add( s[0], temp[3] );
   s[0]  = g_mul( y, g[0] );
  };
#endif
    return s;  
};


#ifndef CONFIG_NAND_SPL
/**
 * nand_default_bbt - [NAND Interface] Select a default bad block table for the device
 * @mtd:    MTD device structure
 *
 * This function selects the default bad block table
 * support for the device and calls the nand_scan_bbt function
 *
*/
int lq_rs_nand_default_bbt(struct mtd_info *mtd)
{
       int ecc_length;
	   struct nand_chip *this = mtd->priv;
#ifdef CONFIG_NAND_ECC_HW_4BYTES
       ecc_length=4;
#else
       ecc_length=3;
#endif
    /* Is a flash based bad block table requested ? */
    if (this->options & NAND_USE_FLASH_BBT) {
        /* Use the default pattern descriptors */
        if (!this->bbt_td) {
            this->bbt_td = &bbt_main_descr;
            this->bbt_md = &bbt_mirror_descr;
        }
        if (!this->badblock_pattern) {
            this->badblock_pattern = (mtd->writesize > 512) ? &largepage_flashbased : &smallpage_flashbased;
        }
        this->bbt_td->offs =8 + (mtd->writesize/128)*ecc_length;
	    this->bbt_td->veroffs =12 + (mtd->writesize/128)*ecc_length;
	    this->bbt_md->offs =8 + (mtd->writesize/128)*ecc_length;
	    this->bbt_md->veroffs =12 + (mtd->writesize/128)*ecc_length;
    } else {
        this->bbt_td = NULL;
        this->bbt_md = NULL;
        if (!this->badblock_pattern) {
            this->badblock_pattern = (mtd->writesize > 512) ?
                &largepage_memorybased : &smallpage_memorybased;
        }
    }
	
	return nand_scan_bbt(mtd, this->badblock_pattern);

}
#endif


static void hsnand_dma_setup(void)
{
         int i;     
		 /*configure DMA*/
         *AR10_PMU_CLKGCR1_A |= (1<<5)|(1<<10); /*enable EBU and DMA clocks in PMU*/
         *AR10_DMA_PS = 0x3;                    /*port3 for HSNAND*/ 
      
	
		 *AR10_DMA_PCTRL = 0xf3c;                /*burst size to 8 words*/ 
   
         *AR10_DMA_CS    = RX_CHAN_NO;
         *AR10_DMA_CCTRL=  0x2;
         *AR10_DMA_CCTRL = 0x30000;  /* Channel is off , we will enable it once des are set*/
         *AR10_DMA_CDBA  = (u32)hsnand_rx_des_ring & 0x1ffffff0;;   
		 *AR10_DMA_CDLEN = NUM_RX_DESC; 
         *AR10_DMA_CIE   = 0x0;
         *AR10_DMA_CPOLL = 0x80000080;
         
         *AR10_DMA_CS    = TX_CHAN_NO;
         *AR10_DMA_CCTRL=  0x2;
         *AR10_DMA_CCTRL = 0x30100;  /* Channel is off , we will enable it once des are set*/
         *AR10_DMA_CDBA  = (u32)hsnand_tx_des_ring & 0x1ffffff0;;   
		 *AR10_DMA_CDLEN = NUM_TX_DESC; 
         *AR10_DMA_CIE   = 0x0;
         *AR10_DMA_CPOLL = 0x80000080;
			   
	      hsnand_tx_num = 0; 
          hsnand_rx_num = 0;
          
          for(i=0;i < NUM_RX_DESC; i++)
	      {
	          dma_rx_descriptor_t * rx_desc = KSEG1ADDR(&hsnand_rx_des_ring[i]);
              rx_desc->status.word=0;
              rx_desc->status.field.OWN = 0;
	          rx_desc->status.field.DataLen = 0;   /* 1536  */
	          rx_desc->DataPtr = 0;
	      }
		  
			  
}

/**
 * lq_nand_read_page_hwecc - [REPLACABLE] hardware ecc based page read function
 * @mtd:    mtd info structure
 * @chip:   nand chip info structure
 * @buf:    buffer to store read data
 * @page:   page number to read
 *
 * Not for syndrome calculating ecc controllers which need a special oob layout
 */
static int lq_nand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
                uint8_t *buf, int page)
{
     int ret=0;
	 dma_rx_descriptor_t * rx_desc;
     u8* tmp_buf;
	 int flag=0;
	 int i;
	
	 rx_desc = KSEG1ADDR(&hsnand_rx_des_ring[hsnand_rx_num]); 
     while(rx_desc->status.field.OWN);
     rx_desc->status.field.Sop=0;
     rx_desc->status.field.Eop=0;
     rx_desc->status.field.C=0;
     rx_desc->status.field.DataLen=mtd->writesize;
#ifdef CONFIG_NAND_SPL    
     rx_desc->DataPtr = (u32)buf & 0x1ffffff0;
#else
     if((u32)buf & 0x1f ){
		//tmp_buf = (u8*)0x80200000; /*fix me, need to allocate 32byte aligned buffer in kernel*/
		tmp_buf = (uchar *)memalign(32, mtd->writesize);
		rx_desc->DataPtr = (u32)tmp_buf & 0x1ffffff0;
		flag=1;
	 }else{
	    rx_desc->DataPtr = (u32)buf & 0x1ffffff0;
	 }
#endif
	 
	 rx_desc->status.field.OWN=1;
	 
	 	 
     *AR10_DMA_CS=RX_CHAN_NO;

	 if(!(*AR10_DMA_CCTRL & 1))
	 *AR10_DMA_CCTRL|=1;
			 
	 NAND_SETALE;
     *BSP_INT_Msk_CTL |=1<<((page%2)?6:5);	/*even page address is odd page, odd is even*/ 
	 *BSP_MD_CTL &=~(1<<10);
	 *BSP_NDAC_CTL2 |=((addr_cnt==5)?2:1)<<16;
	 *BSP_MD_CTL |= 1<<2;
   
	 while(rx_desc->status.field.OWN || !rx_desc->status.field.C);
     /*invalidate cache here*/
#ifdef CONFIG_NAND_SPL     
     invalidate_dcache_range(buf, buf+mtd->writesize);
#else	 
	 if(flag){
	     invalidate_dcache_range(tmp_buf, tmp_buf+mtd->writesize);
         memcpy(buf, tmp_buf, mtd->writesize);
		 free(tmp_buf);
	 }else{
         invalidate_dcache_range(buf, buf+mtd->writesize);
	 }
#endif	 
	 
     hsnand_rx_num++;
     if(hsnand_rx_num==NUM_RX_DESC) hsnand_rx_num=0;
	 NAND_CLRALE;

     if(*BSP_INT_Sta){
       //printf("page %d read error!\n",page);
	   for(i=0;i<mtd->writesize;i++){
            if(buf[i]!=0xff) ret=-1;
	   }
	 }
	 *BSP_INT_Sta |=((page%2)?6:5);/*clear interrupts status*/
	 return ret;
}


/**
 * lq_nand_write_page_hwecc - [REPLACABLE] hardware ecc based page write function
 * @mtd:    mtd info structure
 * @chip:   nand chip info structure
 * @buf:    data buffer
 */
static void lq_nand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
                  const uint8_t *buf)
{
    dma_tx_descriptor_t * tx_desc= KSEG1ADDR(&hsnand_tx_des_ring[hsnand_tx_num]);  
    u8* tmp_buf;
	
	while(tx_desc->status.field.OWN);/*descriptor not available, wait here*/
    tx_desc->status.field.Sop=1;
    tx_desc->status.field.Eop=1;
    tx_desc->status.field.C=0;
    tx_desc->status.field.DataLen = mtd->writesize;
    tx_desc->status.field.Byteoffset = (u32)buf & 0x1f; 
	tx_desc->DataPtr = (u32)(buf-tx_desc->status.field.Byteoffset) & 0x0ffffff0;
	flush_dcache_range(buf,buf+mtd->writesize);
    tx_desc->status.field.OWN=1;	
    hsnand_tx_num++;
	if(hsnand_tx_num==NUM_TX_DESC) hsnand_tx_num=0;
		
			
	*AR10_DMA_CS=TX_CHAN_NO;

	if(!(*AR10_DMA_CCTRL & 1))
	*AR10_DMA_CCTRL|=1;

    NAND_SETALE;    
    *BSP_INT_Msk_CTL |=1<<4;
	*BSP_NDAC_CTL2 |=((addr_cnt==5)?2:1)<<16;						
    *BSP_MD_CTL |= 1<<2;
  
	while(!(*BSP_INT_Sta & (1<<4)));
	*BSP_INT_Sta |=(1<<4);
	NAND_CLRALE;

} 

/**
 * lq_nand_read_page_raw - [Intern] read raw page data without ecc
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	buffer to store read data
 * @page:	page number to read
 *
 * Not for syndrome calculating ecc controllers, which use a special oob layout
 */
static int lq_nand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
			      uint8_t *buf, int page)
{
    int i, cmd;
	NAND_SETCLE;
    latchcmd = WRITE_CMD;
	cmd = NAND_CMD_READ0;
	ifx_nand_write_byte(mtd, cmd);
	NAND_CLRCLE;
	NAND_SETALE;
	latchcmd = WRITE_ADDR;
	for(i=0;i<addr_cnt;i++){
       switch(i){
           case 0:
		      cmd = (*BSP_NDAC_CTL1 >>8) & 0xff;  
			  ifx_nand_write_byte(mtd, cmd);
		      break;
	       case 1:
		      cmd = (*BSP_NDAC_CTL1 >>16) & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 2:
		      cmd = (*BSP_NDAC_CTL1 >>24) & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 3:
		      cmd = *BSP_NDAC_CTL2 & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 4:
		      cmd = (*BSP_NDAC_CTL2 >>8)  & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
	   }
	}
	NAND_CLRALE;
	NAND_SETCLE;
    if(mtd->writesize > 512){
       latchcmd = WRITE_CMD;
	   cmd = NAND_CMD_READSTART;
       ifx_nand_write_byte(mtd, cmd);
	}
	NAND_CLRCLE;
	while(!NAND_READY);
	chip->read_buf(mtd, buf, mtd->writesize);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);
	addr_cnt = 0;
	return 0;
}


/**
 * lq_nand_write_page_raw - [Intern] raw page write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 *
 * Not for syndrome calculating ecc controllers, which use a special oob layout
 */
static void lq_nand_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
				const uint8_t *buf)
{
	 int i, j, cmd;
	 u8* s;

#ifdef CONFIG_NAND_ECC_HW_4BYTES
     for(i=0;i<mtd->writesize/128;i++){
         s = reed_solomn_128bytes_ecc(buf+i*128);
         chip->oob_poi[i*4]=s[3];
         chip->oob_poi[i*4+1]=s[2];
         chip->oob_poi[i*4+2]=s[1];
         chip->oob_poi[i*4+3]=s[0];
	     }
#else
      for(i=0;i<mtd->writesize/128;i++){
	     s = reed_solomn_128bytes_ecc(buf+i*128);
	     chip->oob_poi[i*3]=s[2];
	     chip->oob_poi[i*3+1]=s[1];
	     chip->oob_poi[i*3+2]=s[0];
         }
#endif

	 NAND_SETCLE;
     latchcmd = WRITE_CMD;
	 cmd = NAND_CMD_SEQIN;
	 ifx_nand_write_byte(mtd, cmd);
	 NAND_CLRCLE;
	 NAND_SETALE;
     latchcmd = WRITE_ADDR;
	 for(i=0;i<addr_cnt;i++){
       switch(i){
           case 0:
		      cmd = (*BSP_NDAC_CTL1 >>8) & 0xff;  
			  ifx_nand_write_byte(mtd, cmd);
		      break;
	       case 1:
		      cmd = (*BSP_NDAC_CTL1 >>16) & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 2:
		      cmd = (*BSP_NDAC_CTL1 >>24) & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 3:
		      cmd = *BSP_NDAC_CTL2 & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 4:
		      cmd = (*BSP_NDAC_CTL2 >>8)  & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
	   }
	}
	 NAND_CLRALE;
     while(!NAND_READY);	
	 chip->write_buf(mtd, buf, mtd->writesize);
	 chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);
	 NAND_SETCLE;
     latchcmd = WRITE_CMD;
	 cmd = NAND_CMD_PAGEPROG;
	 ifx_nand_write_byte(mtd, cmd);
	 NAND_CLRCLE;
     addr_cnt = 0;
	 while(!NAND_READY);

	 /* Send a fake command to program the OOB data */
	 chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);
			  
}


/**
 * lq_nand_write_oob - [REPLACABLE] the most common OOB data write function
 * @mtd:    mtd info structure
 * @chip:   nand chip info structure
 * @page:   page number to write
 */
static int lq_nand_write_oob(struct mtd_info *mtd, struct nand_chip *chip,
                  int page)
{
    int i, cmd;
    int status = 0;
    const uint8_t *buf = chip->oob_poi;
    int length = mtd->oobsize;

    chip->cmdfunc(mtd, NAND_CMD_SEQIN, mtd->writesize, page);
    
    NAND_SETCLE;
     latchcmd = WRITE_CMD;
	 cmd = NAND_CMD_SEQIN;
	 ifx_nand_write_byte(mtd, cmd);
	 NAND_CLRCLE;
	 NAND_SETALE;
     latchcmd = WRITE_ADDR;
	 for(i=0;i<addr_cnt;i++){
       switch(i){
           case 0:
		      cmd = (*BSP_NDAC_CTL1 >>8) & 0xff;  
			  ifx_nand_write_byte(mtd, cmd);
		      break;
	       case 1:
		      cmd = (*BSP_NDAC_CTL1 >>16) & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 2:
		      cmd = (*BSP_NDAC_CTL1 >>24) & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 3:
		      cmd = *BSP_NDAC_CTL2 & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 4:
		      cmd = (*BSP_NDAC_CTL2 >>8)  & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
	   }
	}
	NAND_CLRALE;
     while(!NAND_READY);	
    
    
    chip->write_buf(mtd, buf, length);

    NAND_SETCLE;
    latchcmd = WRITE_CMD;
	  cmd = NAND_CMD_PAGEPROG;
	  ifx_nand_write_byte(mtd, cmd);
	  NAND_CLRCLE;
    addr_cnt = 0;
     
    while(!NAND_READY); 
    
     /* Send a fake command to program the OOB data */
    chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);


    return 0;
}


/**
 * lq_nand_read_oob - [REPLACABLE] the most common OOB data read function
 * @mtd:    mtd info structure
 * @chip:   nand chip info structure
 * @page:   page number to read
 * @sndcmd: flag whether to issue read command or not
 */
static int lq_nand_read_oob(struct mtd_info *mtd, struct nand_chip *chip,
	                   int page, int sndcmd)
{
	
	  int i, cmd;
    if (sndcmd) {
          chip->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
          sndcmd = 0;
        }
	NAND_SETCLE;
  latchcmd = WRITE_CMD;
	cmd = NAND_CMD_READ0;
	ifx_nand_write_byte(mtd, cmd);
	NAND_CLRCLE;
	NAND_SETALE;
	latchcmd = WRITE_ADDR;
	for(i=0;i<addr_cnt;i++){
       switch(i){
           case 0:
		      cmd = (*BSP_NDAC_CTL1 >>8) & 0xff;  
			  ifx_nand_write_byte(mtd, cmd);
		      break;
	       case 1:
		      cmd = (*BSP_NDAC_CTL1 >>16) & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 2:
		      cmd = (*BSP_NDAC_CTL1 >>24) & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 3:
		      cmd = *BSP_NDAC_CTL2 & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
		   case 4:
		      cmd = (*BSP_NDAC_CTL2 >>8)  & 0xff;
			  ifx_nand_write_byte(mtd, cmd);
		      break;
	   }
	}
	NAND_CLRALE;
	NAND_SETCLE;
    if(mtd->writesize > 512){
       latchcmd = WRITE_CMD;
	   cmd = NAND_CMD_READSTART;
       ifx_nand_write_byte(mtd, cmd);
	}
	NAND_CLRCLE;
	while(!NAND_READY);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);
	addr_cnt = 0;
  return sndcmd;
}
													   

#endif
static void ifx_nand_chip_init(void)
{
         volatile unsigned char tmpc[5];
         volatile unsigned char buf;
         int i;

		 nand_gpio_init();

#ifdef CONFIG_NAND_CS0
         *EBU_CON_0    = 0x40c7fe;
         *EBU_NAND_CON = 0x000000F2; 
#else
		 *EBU_ADDR_SEL_1 = (NAND_BASE_ADDRESS&0x1fffff00)|0x31;
		 /* byte swap;minimum
          * delay*/
		 *EBU_CON_1	= 0x40F295;
	     *EBU_NAND_CON = 0x000005F2;
#endif
#ifdef CONFIG_NAND_ECC_HW_REED_SOLOMON 
       hsnand_dma_setup();
#endif
#if 0

    ifx_nand_select_chip(NULL, 0);
    NAND_WRITE(WRITE_CMD, 0xff);
    while(!NAND_READY){}
    ifx_nand_select_chip(NULL, -1);
    
	ifx_nand_select_chip(NULL, 0);
    NAND_WRITE(WRITE_CMD, 0x60);
    NAND_SETALE;
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_CLRALE;
    NAND_WRITE(WRITE_CMD, 0xd0);
    while(!NAND_READY){}
	ifx_nand_select_chip(NULL, -1);
    
    ifx_nand_select_chip(NULL, 0);
    printf("delay\n");    
	NAND_SETCLE;
	NAND_WRITE(WRITE_CMD, 0x0);
    NAND_CLRCLE;
	NAND_SETALE;
	NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_CLRALE;
	NAND_SETCLE;
    NAND_WRITE(WRITE_CMD, 0x30);
    NAND_CLRCLE;
	for(i=0;i<4096;i++){
    	NAND_READ(READ_DATA, buf);
    	printf("%02x ",buf);
    	//if(buf!=0xff) printf("erase error!\n");
    }
    while(!NAND_READY){}
    ifx_nand_select_chip(NULL, -1);
#endif

#if 0
    ifx_nand_select_chip(NULL, 0);
    NAND_WRITE(WRITE_CMD, 0xff);
    while(!NAND_READY){}
    ifx_nand_select_chip(NULL, -1);
    
    ifx_nand_select_chip(NULL, 0);
    NAND_WRITE(WRITE_CMD, 0x80);
    NAND_SETALE;
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_CLRALE;
    for(i=0;i<8192;i++){
    	WRITE_NAND(i%5,0xb4000000); 
    }
	for(i=0;i<352;i++){
        WRITE_NAND(0xff,0xb4000000);
	}
	for(i=0;i<96;i++){
        WRITE_NAND(0x55,0xb4000000);
	}
    NAND_WRITE(WRITE_CMD, 0x10);
    while(!NAND_READY){}
    ifx_nand_select_chip(NULL, -1);

    ifx_nand_select_chip(NULL, 0);
    NAND_WRITE(WRITE_CMD, 0xff);
    while(!NAND_READY){}
    ifx_nand_select_chip(NULL, -1);


    ifx_nand_select_chip(NULL, 0);
    NAND_WRITE(WRITE_CMD, 0x0);
    NAND_SETALE;
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_WRITE(WRITE_ADDR,0);
    NAND_CLRALE;
    NAND_WRITE(WRITE_CMD, 0x30);
    for(i=0;i<4096;i++){
    	NAND_READ(READ_DATA, buf);
    	printf("%02x ",buf);
    	//if(buf!=i%5) printf("read error!\n");
    	if((i+1)%16==0) printf("\n");
    }
	for(i=0;i<224;i++){
	    NAND_READ(READ_DATA, buf);
	    printf("%02x ",buf);
	    if((i+1)%16==0) printf("\n");
	  }
										
    while(!NAND_READY){}
    ifx_nand_select_chip(NULL, -1);
#endif    
        
    
}
 



/*
 * Board-specific NAND initialization. The following members of the
 * argument are board-specific (per include/linux/mtd/nand.h):
 * - IO_ADDR_R?: address to read the 8 I/O lines of the flash device
 * - IO_ADDR_W?: address to write the 8 I/O lines of the flash device
 * - hwcontrol: hardwarespecific function for accesing control-lines
 * - dev_ready: hardwarespecific function for  accesing device ready/busy line
 * - enable_hwecc?: function to enable (reset)  hardware ecc generator. Must
 *   only be provided if a hardware ECC is available
 * - eccmode: mode of ecc, see defines
 * - chip_delay: chip dependent delay for transfering data from array to
 *   read regs (tR)
 * - options: various chip options. They can partly be set to inform
 *   nand_scan about special functionality. See the defines for further
 *   explanation
 * Members with a "?" were not set in the merged testing-NAND branch,
 * so they are not set here either.
 */
int board_nand_init(struct nand_chip *nand)
{
	 ifx_nand_chip_init();
	 /* insert callbacks */
   nand->IO_ADDR_R = NAND_BASE_ADDRESS;
   nand->IO_ADDR_W = NAND_BASE_ADDRESS;
	 
   nand->read_byte=ifx_nand_read_byte;
   nand->read_buf=ifx_nand_read_buf;
   //nand->write_byte=ifx_nand_write_byte;
#ifndef CONFIG_NAND_SPL   
   nand->write_buf=ifx_nand_write_buf;
   nand->verify_buf=ifx_nand_verify_buf;
#endif   
   //nand->hwcontrol=ifx_nand_hwcontrol;
   //nand->block_bad = ifx_nand_block_bad;
   nand->cmd_ctrl=ifx_nand_cmd_ctrl;
   nand->dev_ready=NanD_WaitReady;
   nand->select_chip=ifx_nand_select_chip;
#ifdef CONFIG_NAND_ECC_HW_REED_SOLOMON
   nand->ecc.mode = NAND_ECC_HW;
   nand->options |= NAND_NO_SUBPAGE_WRITE;
#ifndef CONFIG_NAND_SPL   
   nand->scan_bbt = lq_rs_nand_default_bbt;
#endif   
   nand->ecc.read_page  = lq_nand_read_page_hwecc;
   nand->ecc.write_page = lq_nand_write_page_hwecc;
   nand->ecc.read_page_raw = lq_nand_read_page_raw;
   nand->ecc.write_page_raw = lq_nand_write_page_raw;
   nand->ecc.read_oob = lq_nand_read_oob;
   nand->ecc.write_oob = lq_nand_write_oob;
   nand->ecc.size = 128;
   nand->ops.mode = MTD_OOB_AUTO;
#elif defined(CONFIG_NAND_ECC_BCH)
   nand->ecc.mode = NAND_ECC_SOFT_BCH; 
#else
   nand->ecc.mode=NAND_ECC_SOFT;
#endif
   //nand->ecc.mode=NAND_ECC_NONE;
   nand->options|=NAND_USE_FLASH_BBT;
   //nand->options|=NAND_SKIP_BBTSCAN;
#ifdef CONFIG_NAND_SPL
   nand->ecc.hwctl = lq_enable_hwecc;
   nand->ecc.calculate = nand_calculate_ecc;
   nand->ecc.correct = nand_correct_data;
#endif
	return 0;
}




