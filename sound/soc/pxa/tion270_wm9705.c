/*
 * tion270_wm9705.c -- SoC audio for Tion270
 * Based on e750_wm9705.c
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation; version 2 ONLY.
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/gpio.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <mach/audio.h>
#include <mach/eseries-gpio.h>

#include <asm/mach-types.h>

#include "../codecs/wm9705.h"
#include "pxa2xx-pcm.h"
#include "pxa2xx-ac97.h"

static const struct snd_soc_dapm_widget tion270_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_SPK("Speaker", NULL),
	SND_SOC_DAPM_MIC("Mic (Internal)", NULL),
};

static const struct snd_soc_dapm_route audio_map[] = {
	{"Headphone Amp", NULL, "HPOUTL"},
	{"Headphone Amp", NULL, "HPOUTR"},
	{"Headphone Jack", NULL, "Headphone Amp"},

	{"Speaker Amp", NULL, "MONOOUT"},
	{"Speaker", NULL, "Speaker Amp"},

	{"MIC1", NULL, "Mic (Internal)"},
};

static int tion270_ac97_init(struct snd_soc_codec *codec)
{
	snd_soc_dapm_nc_pin(codec, "LOUT");
	snd_soc_dapm_nc_pin(codec, "ROUT");
	snd_soc_dapm_nc_pin(codec, "PHONE");
	snd_soc_dapm_nc_pin(codec, "LINEINL");
	snd_soc_dapm_nc_pin(codec, "LINEINR");
	snd_soc_dapm_nc_pin(codec, "CDINL");
	snd_soc_dapm_nc_pin(codec, "CDINR");
	snd_soc_dapm_nc_pin(codec, "PCBEEP");
	snd_soc_dapm_nc_pin(codec, "MIC2");

	snd_soc_dapm_new_controls(codec, tion270_dapm_widgets,
					ARRAY_SIZE(tion270_dapm_widgets));

	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

	snd_soc_dapm_sync(codec);

	return 0;
}

static struct snd_soc_dai_link tion270_dai[] = {
	{
		.name = "AC97",
		.stream_name = "AC97 HiFi",
		.cpu_dai = &pxa_ac97_dai[PXA2XX_DAI_AC97_HIFI],
		.codec_dai = &wm9705_dai[WM9705_DAI_AC97_HIFI],
		.init = tion270_ac97_init,
		/* use ops to check startup state */
	},
	{
		.name = "AC97 Aux",
		.stream_name = "AC97 Aux",
		.cpu_dai = &pxa_ac97_dai[PXA2XX_DAI_AC97_AUX],
		.codec_dai = &wm9705_dai[WM9705_DAI_AC97_AUX],
	},
};

static struct snd_soc_card tion270 = {
	.name = "Tion270",
	.platform = &pxa2xx_soc_platform,
	.dai_link = tion270_dai,
	.num_links = ARRAY_SIZE(tion270_dai),
};

static struct snd_soc_device tion270_snd_devdata = {
	.card = &tion270,
	.codec_dev = &soc_codec_dev_wm9705,
};

static struct platform_device *tion270_snd_device;

static int __init tion270_wm9705_init(void)
{
	int ret;

	tion270_snd_device = platform_device_alloc("soc-audio", -1);
	if (!tion270_snd_device) {
		ret = -ENOMEM;
		goto err;
	}

	platform_set_drvdata(tion270_snd_device, &tion270_snd_devdata);
	tion270_snd_devdata.dev = &tion270_snd_device->dev;
	ret = platform_device_add(tion270_snd_device);

	if (!ret)
		return 0;

	platform_device_put(tion270_snd_device);
err:
	return ret;
}

static void __exit tion270_wm9705_exit(void)
{
	platform_device_unregister(tion270_snd_device);
}

module_init(tion270_wm9705_init);
module_exit(tion270_wm9705_exit);

/* Module information */
MODULE_AUTHOR("Sergey Alyoshin <alyoshin.s@gmail.com>");
MODULE_DESCRIPTION("ALSA SoC driver for Tion270");
MODULE_LICENSE("GPL v2");
