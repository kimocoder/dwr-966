#ifndef _LINUX_EXTMARK_H
#define _LINUX_EXTMARK_H

/* Bit counting starts from Least significant bit, 0..31*/

#define CPEID_START_BIT_POS 0
#define CPEID_NO_OF_BITS 6
#define CPEID_MASK 0x3f

#define QUEPRIO_START_BIT_POS 6
#define QUEPRIO_NO_OF_BITS 4
#define QUEPRIO_MASK 0x3c0

#define SUBQUEUE_START_BIT_POS 10
#define SUBQUEUE_NO_OF_BITS 3
#define SUBQUEUE_MASK 0x1c00

#define ACCELSEL_START_BIT_POS 13
#define ACCELSEL_NO_OF_BITS 1
#define ACCELSEL_MASK 0x2000

#define VPRIO_START_BIT_POS 14
#define VPRIO_NO_OF_BITS 3
#define VPRIO_MASK 0x1c000

#define VLANID_START_BIT_POS 17
#define VLANID_NO_OF_BITS 12
#define VLANID_MASK 0x1ffe0000

#define PPE_START_BIT_POS 29
#define PPE_NO_OF_BITS 1
#define PPE_MASK 0x20000000

/* Two bits(30 and 31) are reserved for future use*/

#endif
