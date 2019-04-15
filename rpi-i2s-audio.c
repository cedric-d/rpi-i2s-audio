/*
 * Raspberry Pi I2S audio module
 * Fork of https://github.com/PaulCreaser/rpi-i2s-audio
 *
 * modified for linux 4.1.5
 * inspired by https://github.com/msperl/spi-config
 * with thanks for https://github.com/notro/rpi-source/wiki
 * as well as Florian Meier for the rpi i2s and dma drivers
 *
 * to use a differant (simple-card compatible) codec
 * change the codec name string in two places and the
 * codec_dai name string. (see codec's source file)
 *
 *
 * N.B. playback vs capture is determined by the codec choice
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/platform_device.h>
#include <sound/simple_card.h>


static void device_release_callback(struct device *dev) { /* do nothing */ };

static struct asoc_simple_card_info snd_rpi_i2s_card_info = {
	.card = "snd_rpi_i2s_card",		// -> snd_soc_card.name
	.name = "i2s_codec_link",		// -> snd_soc_dai_link.name
	.codec = "snd-soc-dummy",		// -> snd_soc_dai_link.codec_name
	.platform = "3f203000.i2s",
	.daifmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS,
	.cpu_dai = {
		.name = "3f203000.i2s",		// -> snd_soc_dai_link.cpu_dai_name
		.sysclk = 0
	},
	.codec_dai = {
		.name = "snd-soc-dummy-dai",	// -> snd_soc_dai_link.codec_dai_name
		.sysclk = 0
	},
};

static struct platform_device snd_rpi_i2s_card_device = {
	.name = "asoc-simple-card",
	.id = 0,
	.num_resources = 0,
	.dev = {
		.release = &device_release_callback,
		.platform_data = &snd_rpi_i2s_card_info,
	},
};


static int rpi_i2s_audio_init(void)
{
	const char *basemodule = "snd-soc-bcm2835-i2s";
	int err;

	err = request_module(basemodule);
	pr_notice("Request module load '%s': %d\n", basemodule, err);
	if (err)
		goto fail_module;

	err = platform_device_register(&snd_rpi_i2s_card_device);
	pr_notice("Register platform device '%s': %d\n", snd_rpi_i2s_card_device.name, err);
	if (err)
		goto fail_register;

	return 0;

fail_register:
fail_module:
	return err;
}

static void rpi_i2s_audio_exit(void)
{
	platform_device_unregister(&snd_rpi_i2s_card_device);
}

module_init(rpi_i2s_audio_init);
module_exit(rpi_i2s_audio_exit);

MODULE_DESCRIPTION("Raspberry Pi I2S audio module");
MODULE_AUTHOR("Airbus Operations S.A.S.");
MODULE_LICENSE("GPL");
