/*
 * ARC HSDK Platform support code
 *
 * Copyright (C) 2016 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/smp.h>
#include <asm/arcregs.h>
#include <asm/mach_desc.h>

/*
 * By default ICCM is mapped to 0x7z while this area is used for
 * Virtual kernel mappings, so move it to currently unused area.
 */
void relocate_iccm(void)
{
	if (cpuinfo_arc700[smp_processor_id()].iccm.sz)
		write_aux_reg(ARC_REG_AUX_ICCM, 0x60000000);
}

static void hsdk_early_init(void)
{
	relocate_iccm();
}

static void hsdk_init_per_cpu(unsigned int cpu)
{
	relocate_iccm();
}

/*----------------------- Machine Descriptions ------------------------------
 *
 * Machine description is simply a set of platform/board specific callbacks
 * This is not directly related to DeviceTree based dynamic device creation,
 * however as part of early device tree scan, we also select the right
 * callback set, by matching the DT compatible name.
 */

static const char *hsdk_compat[] __initconst = {
	"snps,hsdk",
	NULL,
};

MACHINE_START(SIMULATION, "hsdk")
	.dt_compat	= hsdk_compat,
MACHINE_END
