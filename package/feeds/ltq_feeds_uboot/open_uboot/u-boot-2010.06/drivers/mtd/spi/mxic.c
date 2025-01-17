/*
 * MXIC 25L serial flash support
 *
 * Copyright (C) 2009 Infineon Corporation
 * 
 * Revision history:
 * June 24, 2009 Wu Qi Ming initial version
 *
 */

//#define DEBUG
#include <common.h>
#include <malloc.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

/* MX25LXX05D-specific commands */
#define CMD_MX25L_WREN     0x06
#define CMD_MX25L_WRDI     0x04
#define CMD_MX25L_RDID     0x9f
#define CMD_MX25L_RDSR     0x05
#define CMD_MX25L_WRSR     0x01
#define CMD_MX25L_READ     0x03
#define CMD_MX25L_FASTREAD 0x0B
#define CMD_MX25L_2READ    0xBB
#define CMD_MX25L_SE       0x20
#define CMD_MX25L_BE       0xd8
#define CMD_MX25L_CE       0x60 /*or 0xc7 */
#define CMD_MX25L_PP       0x02
#define CMD_MX25L_CP       0xad
#define CMD_MX25L_DP       0xb9
#define CMD_MX25L_RDP      0xab
#define CMD_MX25L_RES      0xab
#define CMD_MX25L_REMS     0x90
#define CMD_MX25L_REMS2    0xef
#define CMD_MX25L_ENSO     0xb1
#define CMD_MX25L_EXSO     0xc1
#define CMD_MX25L_RDSCUR   0x2b
#define CMD_MX25L_WRSCUR   0x2f
#define CMD_MX25L_ESRY     0x70
#define CMD_MX25L_DSRY     0x80

#define MX25L_SR_WIP         (1 << 0)        /* Write-in-Progress */

/* AT45 status register bits */

/* DataFlash family IDs, as obtained from the second idcode byte */
#define MXIC_FAMILY_25L 0x20

struct mxic_spi_flash_params {
	u8		idcode1;
	u8      idcode2;
	/* Log2 of page size in power-of-two mode */
	u8      l2_page_size;
	u8		pages_per_sector;
	u8		sectors_per_block;
	u16		nr_blocks;
	const char	name[20];
};

/* spi_flash needs to be first so upper layers can free() it */
struct mxic_spi_flash {
	struct spi_flash flash;
	const struct mxic_spi_flash_params *params;
};

static inline struct atmel_spi_flash *
to_mxic_spi_flash(struct spi_flash *flash)
{
	return container_of(flash, struct mxic_spi_flash, flash);
}

static const struct mxic_spi_flash_params mxic_spi_flash_table[] = {
	{
		.idcode1		= 0x20,
		.idcode2        = 0x14,
		.l2_page_size		= 8,
		.pages_per_sector	= 16,
		.sectors_per_block	= 16,
		.nr_blocks		= 16,
		.name			= "MX25L8005",
	},
	{
		.idcode1		= 0x20,
		.idcode2                = 0x15,
		.l2_page_size		= 8,
		.pages_per_sector	= 16,
		.sectors_per_block	= 16,
		.nr_blocks		= 32,
		.name			= "MX25L1605D",
	},
   {
         .idcode1                = 0x20,
		 .idcode2                = 0x16,
		 .l2_page_size           = 8,
		 .pages_per_sector       = 16,
		 .sectors_per_block      = 16,
		 .nr_blocks              = 64,
		 .name                   = "MX25L3205D",
	 },
	 {
	      .idcode1               = 0x20,
		  .idcode2               = 0x17,
		  .l2_page_size          = 8,
		  .pages_per_sector      = 16,
		  .sectors_per_block     = 16,
		  .nr_blocks             = 128,
		  .name                  = "MX25L6405D",
	 },
     {
          .idcode1               = 0x20,
          .idcode2               = 0x18,
          .l2_page_size          = 8,
          .pages_per_sector      = 16,
          .sectors_per_block     = 16,
          .nr_blocks             = 256,
          .name                  = "MX25L128XX",
     },
	 {
	      .idcode1               = 0x20,
		  .idcode2               = 0x18,
		  .l2_page_size          = 8,
		  .pages_per_sector      = 16,
		  .sectors_per_block     = 16,
		  .nr_blocks             = 512,
		  .name                  = "MX25L256XX",
	 },
};



static int mx25l_wait_ready(struct spi_flash *flash, unsigned long timeout)
{
    struct spi_slave *spi = flash->spi;
	unsigned long timebase;
	int ret;
	u8 cmd = CMD_MX25L_RDSR;
	u8 status;
	timebase = get_timer(0);
	do {
	        ret = spi_flash_cmd(spi, CMD_MX25L_RDSR, &status, sizeof(status));
                if (ret)  return -1;
             
                if ((status & MX25L_SR_WIP) == 0)
                        break;
	   } while (get_timer(timebase) < timeout);
	
        if ((status & MX25L_SR_WIP) == 0)
                return 0;
	 
         /* Timed out */
	 return -1;
							
}


int mx25l_erase(struct spi_flash *flash, u32 offset, size_t len)
{
        struct mxic_spi_flash *asf = to_mxic_spi_flash(flash);
	unsigned long sector_addr;
	unsigned long page_size;
	unsigned int page_shift;
        int sector_size;
	size_t actual;
	int ret;
	u8 cmd[4];
	cmd[0] = CMD_MX25L_SE;
        page_size =1<<asf->params->l2_page_size;
        
        sector_size = page_size * asf->params->pages_per_sector;	

        if (offset % sector_size || len % sector_size) {
                debug("SF: Erase offset/length not multiple of sector size\n");
                return -1;
        }

        len /= sector_size;
        ret = spi_claim_bus(flash->spi);
        if (ret) {
                debug("SF: Unable to claim SPI bus\n");
                return ret;
        }
        
        for (actual = 0; actual < len; actual++) {
                sector_addr = offset + (actual * sector_size);
                cmd[1] = (sector_addr >> 16) & 0xff;
                cmd[2] = (sector_addr >> 8 ) & 0xff;
                cmd[3] = sector_addr & 0xff;
                ret = spi_flash_cmd(flash->spi, CMD_MX25L_WREN, NULL, 0);
                if (ret < 0) {
                        debug("SF: Enabling Write failed\n");
                        break;
                }

                ret = spi_flash_cmd_write(flash->spi, cmd, 4, NULL, 0);
                if (ret < 0) {
                        debug("SF: page erase failed\n");
                        break;
                }

                /* Up to 2 seconds */
                ret = mx25l_wait_ready(flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);
                if (ret < 0) {
                        debug("SF: page erase timed out\n");
                        break;
                }
        }

        debug("SF: MXIC: Successfully erased %u bytes @ 0x%x\n",
              len * sector_size, offset);

        spi_release_bus(flash->spi);
        return ret;
						
}

static int mx25l_read(struct spi_flash *flash,
        u32 offset, size_t len, void *buf)
{
       struct mxic_spi_flash *asf = to_mxic_spi_flash(flash);
       u8 cmd[4];

       cmd[0] = CMD_MX25L_READ;
       cmd[1] = (offset >> 16) & 0xff;
       cmd[2] = (offset >> 8 ) & 0xff;
       cmd[3] = offset & 0xff;
  
       return spi_flash_read_common(flash, cmd, sizeof(cmd), buf, len);
}
													 
static int mx25l_write(struct spi_flash *flash,
                         u32 offset, size_t len, const void *buf)
{
        struct mxic_spi_flash *asf = to_mxic_spi_flash(flash);
        unsigned long page_addr;
        unsigned long byte_addr;
        unsigned long page_size;
        size_t chunk_len;
        size_t actual;
        int ret;
        u8 cmd[4];

        page_size = 1<< asf->params->l2_page_size;
        page_addr = offset / page_size;
        byte_addr = offset % page_size;

        ret = spi_claim_bus(flash->spi);
        if (ret) {
                debug("SF: Unable to claim SPI bus\n");
                return ret;
        }
        ret = 0;
        for (actual = 0; actual < len; actual += chunk_len) {
                chunk_len = min(len - actual, page_size - byte_addr);
                cmd[0] = CMD_MX25L_PP;
                cmd[1] = page_addr >> 8;
                cmd[2] = page_addr;
                cmd[3] = byte_addr;
                
                /*
                debug
                    ("PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x } chunk_len = %d\n",
                     buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
                */
                ret = spi_flash_cmd(flash->spi, CMD_MX25L_WREN, NULL, 0);
                if (ret < 0) {
                        debug("SF: Enabling Write failed\n");
                        break;
                }

                ret = spi_flash_cmd_write(flash->spi, cmd, 4,
                                          buf + actual, chunk_len);
                if (ret < 0) {
                        debug("SF: MXIC Page Program failed\n");
                        break;
                }

                ret = mx25l_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
                if (ret < 0) {
                        debug("SF: MXIC page programming timed out\n");
                        break;
                }

               page_addr++;
               byte_addr = 0;
             
        }

        debug("SF: MXIC Successfully programmed %u bytes @ 0x%x\n",
              len, offset);

        spi_release_bus(flash->spi);
        return ret;


}

static int mx25l_write_partial(struct spi_flash *flash,
                         u32 offset, size_t len, const void *buf)
{
        struct mxic_spi_flash *asf = to_mxic_spi_flash(flash);
        unsigned long page_addr;
        unsigned long byte_addr;
        unsigned long page_size;
        unsigned long sect_addr;
        unsigned long sector_size;
        size_t chunk_len;
        size_t actual;
        int ret;
        unsigned char *sect_buf = NULL;
        int f_start_partial=0;
        int f_end_partial=0;
        int f_malloc=0;
        int total_sect_num=0;    
            
        int i;

        page_size = 1<< asf->params->l2_page_size;
        sector_size = page_size * asf->params->pages_per_sector;
        
        page_addr = offset / page_size;
        byte_addr = offset % page_size;
        
        if( (offset % sector_size)!=0 ) f_start_partial = 1;
        if( ( (offset + len ) % sector_size)!=0 ) f_end_partial = 1;
        debug("f_start_partial=%d,f_end_partial=%d\n",f_start_partial,f_end_partial);
        total_sect_num = (offset + len -1) / sector_size - offset / sector_size + 1; 
           
        sect_addr = offset / sector_size;         
        sect_buf = buf;
        for(i=0; i<total_sect_num; i++){
             if((f_start_partial && i==0) || (f_end_partial && total_sect_num ==1)) {
                 //sect_buf = malloc(sizeof(sector_size));
                 sect_buf=(uchar *)calloc(sector_size, sizeof(char));
                 if(!sect_buf) {
                    debug("cannot allocate memory!\n");
                  }
                 asf->flash.read(flash, (sect_addr + i) * sector_size, sector_size, sect_buf);
                 memcpy(sect_buf + (offset % sector_size), buf, \
                        min((sector_size-(offset % sector_size)),len));
                 f_malloc = 1;
                 
              }else if(f_end_partial && i == total_sect_num -1 && i!=0){
                 //sect_buf = malloc(sizeof(sector_size));
                 sect_buf=(uchar *)calloc(sector_size, sizeof(char));
                 if(!sect_buf) {
                    debug("cannot allocate memory!\n");
                  }
                 asf->flash.read(flash, (sect_addr + i) * sector_size, sector_size, sect_buf);
                 memcpy(sect_buf,buf+i*sector_size-(offset % sector_size), (offset+len) % sector_size);
                 f_malloc=1;   
              } 
             asf->flash.erase(flash, (sect_addr + i) * sector_size, sector_size);  
             ret=mx25l_write(flash, (sect_addr + i) * sector_size, sector_size, sect_buf);
             if(f_malloc) {
                 free(sect_buf);
                 f_malloc=0;
               }
             sect_buf = buf + (i+1)*sector_size - (offset % sector_size);
        }
        return ret;
 
}


struct spi_flash *spi_flash_probe_mxic(struct spi_slave *spi, u8 *idcode)
{
	const struct mxic_spi_flash_params *params;
	unsigned long page_size;
	unsigned int family;
	struct mxic_spi_flash *asf;
	unsigned int i;
	int ret;
	int addr=0;
	u8 status;

	for (i = 0; i < ARRAY_SIZE(mxic_spi_flash_table); i++) {
		params = &mxic_spi_flash_table[i];
		if (params->idcode1 == idcode[1] && params->idcode2 == idcode[2])
			break;
	}

	if (i == ARRAY_SIZE(mxic_spi_flash_table)) {
		debug("SF: Unsupported DataFlash ID %02x\n",
				idcode[1]);
		return NULL;
	}

	asf = malloc(sizeof(struct mxic_spi_flash));
	if (!asf) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	asf->params = params;
	asf->flash.spi = spi;
	asf->flash.name = params->name;

	page_size =1<<params->l2_page_size;

	family = idcode[1];

	switch (family) {
	case MXIC_FAMILY_25L:
                asf->flash.read = mx25l_read;        
		asf->flash.write = mx25l_write_partial;
		//asf->flash.write = mx25l_write;
		asf->flash.erase = mx25l_erase;
								
		break;

	default:
		debug("SF: Unsupported DataFlash family %u\n", family);
		goto err;
	}

	asf->flash.size = page_size * params->pages_per_sector \
				* params->sectors_per_block               \
				* params->nr_blocks;
	
	debug("SF: Detected %s with page size %lu, total %u bytes\n",
			params->name, page_size, asf->flash.size);
   
         
	for(i=0;i<params->sectors_per_block * params->nr_blocks; i++)
	{
        addr = i * params->pages_per_sector *page_size;
        debug("%08x ",addr);
		if( (i + 1)%8 ==0) debug("\n");
	}
        
	return &asf->flash;

err:
	free(asf);
	return NULL;
}
