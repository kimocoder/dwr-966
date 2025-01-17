/* adpInetNtoa.c - convert internet host address into number-dot notation */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01c,21may05,rnp  minor edits for standards compliance 
01b,20aug03,ksn  added support for psos 2.5 renamed unx_inet_ntoa to
                 adp_inet_ntoa
01a,19jun03,gnm  added copyright information, added module heading.
*/

/*
 * Copyright (c) 1983, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "adaptos.h"

#if defined(BROKEN_INET_NTOA) || !defined(HAVE_INET_NTOA)

#if defined(LIBC_SCCS) && !defined(lint)
static char rcsid[] = 
    "$OpenBSD: inet_ntoa.c,v 1.2 1996/08/19 08:29:16 tholo Exp $";
#endif /* LIBC_SCCS and not lint */

/*
 * Convert network-format internet address
 * to base 256 d.d.d.d representation.
 */

char *adp_inet_ntoa(struct in_addr in)
{
    static char b[18];
    register char *p;

    p = (char *)&in;
#define UC(b)   (((int)b)&0xff)
#ifdef ADAPTOS
    (void)sprintf(b,
#else  /* ADAPTOS */
    (void)snprintf(b, sizeof(b),
#endif /* ADAPTOS */
        "%d.%d.%d.%d", UC(p[0]), UC(p[1]), UC(p[2]), UC(p[3]));
    return (b);
}

#endif /* defined(BROKEN_INET_NTOA) || !defined(HAVE_INET_NTOA) */
