/*
 *  linux/arch/arm/mach-pxa/tion270.c
 *
 *  Support for ZAO ZEO PXA270 based Tion270 module
 *  Based on colibri-pxa270.c
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#undef ORION270

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/sysdev.h>
#include <linux/interrupt.h>
#include <linux/bitops.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/gpio.h>
#include <linux/usb.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <mach/pxa27x-udc.h>
#include <asm/irq.h>
#include <asm/sizes.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>
#include <asm/mach/flash.h>
#include <asm/mach/flash.h>
#include <mach/pxa27x.h>

#include "generic.h"
#include "devices.h"

#include <mach/ohci.h>
#include <mach/pxafb.h>
#include <mach/pxa27x_keypad.h>
#include <asm/uaccess.h>
#include <linux/i2c.h>
#include <plat/i2c.h>
#include <linux/leds.h>
#include <mach/mmc.h>
#include <mach/audio.h>
#include <linux/ucb1400.h>


/*
 * GPIO configuration
 */
static mfp_cfg_t tion270_pin_config[] __initdata = {
			/* Ethernet */
	GPIO49_nPWE,
	GPIO78_nCS_2,	/* CS */
	GPIO79_nCS_3,	/* Orion270 CS */
	GPIO114_GPIO,	/* IRQ */
	GPIO106_GPIO,	/* Orion270 Ethernet IRQ */

				/* USB */
#if 0
	GPIO88_USBH1_PWR,	/* Not for USB host ports 1 and 2? */
#endif
	GPIO89_USBH1_PEN,

	GPIO10_FFUART_DCD,	/* FFUART */
	GPIO27_FFUART_RTS,
	GPIO33_FFUART_DSR,
	GPIO34_FFUART_RXD,
	GPIO38_FFUART_RI,
	GPIO39_FFUART_TXD,
	GPIO40_FFUART_DTR,
	GPIO100_FFUART_CTS,

	GPIO44_BTUART_CTS,	/* BTUART */
	GPIO42_BTUART_RXD,
	GPIO45_BTUART_RTS,
	GPIO43_BTUART_TXD,

	GPIO46_STUART_RXD,	/* STUART */
	GPIO47_STUART_TXD,

	GPIO117_I2C_SCL,	/* I2C */
	GPIO118_I2C_SDA,

#if defined(ORION270)
	GPIO16_KP_MKIN_5,	/* Keypad */
	GPIO107_KP_MKOUT_4,
#endif

	GPIO32_MMC_CLK,		/* MMC */
	GPIO112_MMC_CMD,
	GPIO92_MMC_DAT_0,
	GPIO109_MMC_DAT_1,
	GPIO110_MMC_DAT_2,
	GPIO111_MMC_DAT_3,

	GPIO28_AC97_BITCLK,	/* AC97 */
	GPIO29_AC97_SDATA_IN_0,
	GPIO30_AC97_SDATA_OUT,
	GPIO31_AC97_SYNC,
	GPIO95_AC97_nRESET,
	GPIO98_AC97_SYSCLK,
	GPIO113_GPIO,		/* TS & Wake-on-LAN IRQ */

//	GPIO16_PWM0_OUT,	/* LCD interface */
	GPIO16_GPIO,

	/* TFT LCD */
	/* TODO: 18bpp */
	GPIOxx_LCD_TFT_16BPP,

	/* TODO: SSP */
};

#if (defined(CONFIG_FB_PXA) || defined(CONFIG_FB_PXA_MODULE)) && !defined(ORION270)
static struct pxafb_mode_info tion270_lcd_modes[] = {
{
	.pixclock	= 57692,
	.xres		= 640,
	.yres		= 480,
	.bpp		= 32,
	.depth		= 18,

	.left_margin	= 144,
	.right_margin	= 32,
	.upper_margin	= 13,
	.lower_margin	= 30,

	.hsync_len	= 32,
	.vsync_len	= 2,

	.sync		= FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,
},
};

static void tion270_lcd_power(int on, struct fb_var_screeninfo *info)
{
#if 0
	/* Only for Tion-Pro270 rev2.0 */ 
	gpio_set_value(GPIO81_VPAC270_BKL_ON, on);
#endif
}

static struct pxafb_mach_info tion270_lcd_screen = {
	.modes		= tion270_lcd_modes,
	.num_modes	= ARRAY_SIZE(tion270_lcd_modes),
	.lcd_conn	= LCD_COLOR_TFT_18BPP,
	.pxafb_lcd_power= tion270_lcd_power,
};

static void __init tion270_lcd_init(void)
{
	set_pxa_fb_info(&tion270_lcd_screen);
}
#else
static inline void tion270_lcd_init(void) {}
#endif

#if defined(CONFIG_KEYBOARD_PXA27x) || defined(CONFIG_KEYBOARD_PXA27x_MODULE)
# if defined(ORION270)
static unsigned int tion270_matrix_key_map[] = {
	KEY(5, 4, KEY_SETUP),
};
static struct pxa27x_keypad_platform_data tion270_keypad_info = {
	.matrix_key_rows =	6,	/* Effectively only ONE key */
	.matrix_key_cols =	5,
	.matrix_key_map =	tion270_matrix_key_map,
	.matrix_key_map_size = 	1,

#if 0
	{	/* Matrix key GPIO modes */
		GPIO107_KP_MKOUT4_MD,
		GPIO16_KP_MKIN5_MD,
	},
#endif
};
# else
static struct pxa27x_keypad_platform_data tion270_keypad_info = {
	0, 0,
};
# endif
static void __init tion270_keypad_init(void)
{
	pxa_set_keypad_info(&tion270_keypad_info);
}
#else
static void __init tion270_keypad_init(void) {}
#endif	/* CONFIG_KEYBOARD_PXA27x */

#if defined(ORION270)
/*
 * LED
 */
static struct gpio_led tion270_leds[] = {
	{
		.name		 = "tion270:yellow:1",
		.default_trigger = "none",
		.gpio		 = 17,
		.active_low	 = 1,
	},
};

static struct gpio_led_platform_data tion270_leds_info = {
	.leds		= tion270_leds,
	.num_leds	= ARRAY_SIZE(tion270_leds),
};

static struct platform_device tion270_leds_device = {
	.name		= "leds-gpio",
	.id		= -1,
	.dev		= {
		.platform_data	= &tion270_leds_info,
	},
};
#endif

static struct physmap_flash_data tion270_flash_data = {
	.width = 4,			/* Bankwidth in bytes */
};

static struct resource tion270_flash_resource = {
	.start	= PXA_CS0_PHYS,
	.end	= PXA_CS0_PHYS + SZ_64M - 1,
	.flags	= IORESOURCE_MEM,
};

static struct platform_device tion270_flash_device = {
	.name	= "physmap-flash",
	.id	= 0,
	.dev 	= {
		.platform_data = &tion270_flash_data,
	},
	.num_resources = 1,
	.resource = &tion270_flash_resource,
};

/*
 * DM9000 Ethernet
 */
#if defined(CONFIG_DM9000)
static struct resource dm9000_resources[] = {
	[0] = {
		.start	= PXA_CS2_PHYS,
		.end	= PXA_CS2_PHYS + 3,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= PXA_CS2_PHYS + 4,
		.end	= PXA_CS2_PHYS + 4 + 500,
		.flags	= IORESOURCE_MEM,
	},
	[2] = {
		.start	= gpio_to_irq(114),
		.end	= gpio_to_irq(114),
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_RISING,
	},
};

static struct platform_device dm9000_device = {
	.name		= "dm9000",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(dm9000_resources),
	.resource	= dm9000_resources,
};
#endif /* CONFIG_DM9000 */

/*
 * LAN9221 Ethernet
 */
#if defined(CONFIG_SMC911X)
static struct resource lan9221_resources[] = {
	[0] = {
		.start	= PXA_CS3_PHYS,
		.end	= (PXA_CS3_PHYS + 0x100 - 1),
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= gpio_to_irq(106),
		.end	= gpio_to_irq(106),
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_RISING,
	},
/* TODO: IRQ autodetecton */
};

static struct platform_device lan9221_device = {
	.name		= "smc911x",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(lan9221_resources),
	.resource	= lan9221_resources,
};
#endif /* CONFIG_SMC911X */

/*
 * USB Host (OHCI)
 */
#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
enum {
	IOCTL_SET_OP_MODE = 0x400 + 66,
	IOCTL_GET_OP_MODE,
};
enum serial_mode {
	NONE = -1,
	RS232,
	RS485,
	RS422,
	RS485_4WIRE,
};

struct serial_port {
	char gpios[4];				/* Order of GPIO matters */
	enum serial_mode mode;
};

# if defined(ORION270)
static struct serial_port serial_ports[] = {
	{
		.gpios = {77, 65, 67, 69},	/* Order matters */
		.mode = NONE,
	}, {
		.gpios = {70, 71, 76, 61},
		.mode = NONE,
	}, {
		.gpios = {60, 66, 73, 72},
		.mode = NONE,
	}, {
		.gpios = {75, 59, 63, 68},
		.mode = NONE,
	},
};

static int serial_ports_gpio_initialized = 0;

static void set_all_gpios(int on)
{
	int i, j;

	for (i = 0; i < ARRAY_SIZE(serial_ports); i++)
		for (j = 0; j < ARRAY_SIZE(serial_ports[0].gpios); j++)
			gpio_direction_output(serial_ports[i].gpios[j], on);
}
# endif

static int tion270_ohci_init(struct device *dev)
{
# if defined(ORION270)
	char gpio;
	int i, j;

	/* Request GPIO for serial ports mode switching */
	for (i = 0; i < ARRAY_SIZE(serial_ports); i++)
		for (j = 0; j < ARRAY_SIZE(serial_ports[0].gpios); j++) {
			gpio = serial_ports[i].gpios[j];
			if (gpio_request(gpio, "Serial port mode select"))
				printk(KERN_ERR "can't request GPIO%d for serial port\n", gpio);
			gpio_direction_output(gpio, 1);
		}
	set_all_gpios(1);
	serial_ports_gpio_initialized++;
# endif

/* TODO: Port 2 OTG ID -- GPIO41 */

	/* Port 2 host */
	UP2OCR = UP2OCR_IDON | UP2OCR_HXS | UP2OCR_HXOE | UP2OCR_DPPDE | UP2OCR_DMPDE;

	return 0;
}

static void tion270_ohci_exit(struct device *dev)
{
}

# if defined(ORION270)
static int set_port_mode(struct serial_port *port, int mode)
{
	int i;

	if (mode == RS485_4WIRE)
		return -EINVAL;

	/* First, set "none" mode for port */
	for (i = 0; i < ARRAY_SIZE(port->gpios); i++)
		gpio_set_value(port->gpios[i], 1);
	msleep(1);

	switch (mode) {
	case RS232:
		gpio_set_value(port->gpios[0], 0);
		break;
	case RS485:
		gpio_set_value(port->gpios[1], 0);
		gpio_set_value(port->gpios[2], 0);
		break;
	case RS422:
		gpio_set_value(port->gpios[2], 0);
		gpio_set_value(port->gpios[3], 0);
		break;
	case RS485_4WIRE:
	case NONE:	/* For no warning */
		break;
	}
	port->mode = mode;
	msleep(1);

	return 0;
}

static int tion270_ohci_data(struct device *dev,
		int ser_port_num, int cmd, unsigned long int arg)
{
	/* Switch serial port mode via GPIO's */

	struct usb_device *usb_dev = container_of(dev, struct usb_device, dev);
	int mode;

	/* Converter attached to first port on root hub */
	if (usb_dev->portnum != 1 || usb_dev->level != 1)
		return -ENOIOCTLCMD;

	switch (cmd) {
	case IOCTL_GET_OP_MODE:
		mode = serial_ports[ser_port_num].mode;
		if (copy_to_user((int __user *) arg, &mode, sizeof(int)))
			return -EFAULT;
		return 0;
	case IOCTL_SET_OP_MODE:
		if (!serial_ports_gpio_initialized)
			return -ENODEV;

		if (copy_from_user(&mode, (int __user *) arg, sizeof(int)))
			return -EFAULT;

		ser_port_num %= 4;
		if (mode != serial_ports[ser_port_num].mode)
			return set_port_mode(serial_ports + ser_port_num, mode);

		return 0;
	}

	return -ENOIOCTLCMD;
}
# else
static int tion270_ohci_data(struct device *dev,
		int ser_port_num, int cmd, unsigned long int arg)
{
	return -ENOIOCTLCMD;
}
# endif	/* ORION270 */

static struct pxaohci_platform_data tion270_ohci_info = {
	.init		= tion270_ohci_init,
	.exit		= tion270_ohci_exit,
	.port_mode	= PMM_PERPORT_MODE,
	.flags		= ENABLE_PORT1 | ENABLE_PORT2 |
			POWER_CONTROL_LOW | NO_OC_PROTECTION,
	.data		= tion270_ohci_data,
};

static void __init tion270_uhc_init(void)
{
	pxa_set_ohci_info(&tion270_ohci_info);
}
#else
static inline void tion270_uhc_init(void) {}
#endif	/* CONFIG_USB_OHCI_HCD */


#if defined(CONFIG_SND_PXA2XX_AC97)
/* AC97 */
static pxa2xx_audio_ops_t tion270_ac97_info = {
	.reset_gpio = 95,
};

# if defined(CONFIG_TOUCHSCREEN_UCB1400)
static struct ucb1400_pdata tion270_ucb1400_pdata = {
	.irq		= gpio_to_irq(113),
};

static struct platform_device tion270_ucb1400_device = {
	.name		= "ucb1400_core",
	.id		= -1,
	.dev		= {
		.platform_data = &tion270_ucb1400_pdata,
	},
};
# endif

static void __init tion270_audio_and_ts(void)
{
	pxa_set_ac97_info(&tion270_ac97_info);
# if defined(CONFIG_TOUCHSCREEN_UCB1400)
	platform_device_register(&tion270_ucb1400_device);
# endif
}
#else
static void __init tion270_audio_and_ts(void) {}
#endif

static struct platform_device *tion270_devices[] __initdata = {
	&tion270_flash_device,
#if defined(ORION270)
	&tion270_leds_device,
#endif
#if defined(CONFIG_DM9000)
	&dm9000_device,
#endif
#if defined(CONFIG_SMC911X)
	&lan9221_device,
#endif
};

#if defined(CONFIG_I2C_PXA)
static struct i2c_pxa_platform_data i2c_info = {
	.fast_mode = 1,
};

static struct i2c_board_info __initdata tion270_i2c_devs[] = {
	{
		I2C_BOARD_INFO("m41t00", 0x68),
	},
};

static void tion270_i2c_init(void)
{
	pxa_set_i2c_info(&i2c_info);
	i2c_register_board_info(0, ARRAY_AND_SIZE(tion270_i2c_devs));
}
#else
static void tion270_i2c_init(void) {}
#endif	/* CONFIG_I2C_PXA */


#if defined(CONFIG_MMC_PXA)
static struct pxamci_platform_data tion270_mci_platform_data = {
	.ocr_mask		= MMC_VDD_32_33 | MMC_VDD_33_34,
	.detect_delay_ms	= 250,
	.gpio_card_detect       = 0,
	.gpio_card_ro           = -1,
	.gpio_power             = -1
};

static void __init tion270_mmc_init(void)
{
	pxa_set_mci_info(&tion270_mci_platform_data);
}
#else
static void __init tion270_mmc_init(void) {}
#endif	/* CONFIG_MMC_PXA */

static void __init tion270_init(void)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_pin_config));
	pxa_set_ffuart_info(NULL);
	pxa_set_btuart_info(NULL);
	pxa_set_stuart_info(NULL);

	tion270_lcd_init();
#if 0
	/* For Orion270 */
#if defined(CONFIG_SMC911X)
/* TODO: do this in bootloader? */
	/* SMSC LAN9221 data bus is 16 bit wide */
	MSC1 |= 0xc0000;		/* 16 bit data bus on CS3 */
	MSC1;				/* Need to read */
	set_irq_type(gpio_to_irq(106), IRQ_TYPE_EDGE_RISING);
#endif
#endif
	platform_add_devices(ARRAY_AND_SIZE(tion270_devices));
	tion270_uhc_init();
	tion270_i2c_init();
	tion270_audio_and_ts();
	tion270_keypad_init();
	tion270_mmc_init();

#warning Fix me
	/* TODO: do this via backlight PWM */
	if (gpio_request(16, "LCD backlight"))
		printk(KERN_ERR "can't request GPIO16\n");
	gpio_direction_output(16, 1);
	gpio_set_value(16, 1);


}

static struct map_desc tion270_io_desc[] __initdata = {
	{	/* CS2 Ethernet */
		.virtual 	= 0xfb000000,
		.pfn		= __phys_to_pfn(0x0c000000),
		.length		= 0x00100000,
		.type 		= MT_DEVICE
	},
};

static void __init tion270_map_io(void)
{
	pxa_map_io();
	iotable_init(tion270_io_desc, ARRAY_SIZE(tion270_io_desc));
}

MACHINE_START(TION270, "Tion270")
	.phys_io	= 0x40000000,
	.io_pg_offst	= (io_p2v(0x40000000) >> 18) & 0xfffc,
	.boot_params	= 0x100,
	.init_machine	= tion270_init,
	.map_io		= tion270_map_io,
	.init_irq	= pxa27x_init_irq,
	.timer		= &pxa_timer,
MACHINE_END

