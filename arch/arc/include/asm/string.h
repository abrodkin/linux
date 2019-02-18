/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * vineetg: May 2011
 *  -We had half-optimised memset/memcpy, got better versions of those
 *  -Added memcmp, strchr, strcpy, strcmp, strlen
 *
 * Amit Bhor: Codito Technologies 2004
 */

#ifndef _ASM_ARC_STRING_H
#define _ASM_ARC_STRING_H

#include <linux/types.h>

#define __HAVE_ARCH_STRCHR

extern char *strchr(const char *s, int c);

#endif /* _ASM_ARC_STRING_H */
