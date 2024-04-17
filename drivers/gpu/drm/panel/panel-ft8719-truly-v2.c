// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2024 gentoocat
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct ft8719_truly_v2 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct ft8719_truly_v2 *to_ft8719_truly_v2(struct drm_panel *panel)
{
	return container_of(panel, struct ft8719_truly_v2, panel);
}

static void ft8719_truly_v2_reset(struct ft8719_truly_v2 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int ft8719_truly_v2_on(struct ft8719_truly_v2 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x87, 0x19, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x87, 0x19);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xca,
			       0xeb, 0xd2, 0xb6, 0xae, 0xa6, 0x9a, 0x96, 0x90,
			       0x89, 0x86, 0x83, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xca,
			       0xfe, 0xff, 0x66, 0xf6, 0xff, 0x66, 0xfb, 0xff,
			       0x32);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	mipi_dsi_dcs_write_seq(dsi, 0xca, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xb2);
	mipi_dsi_dcs_write_seq(dsi, 0xca, 0x0c);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xb5);
	mipi_dsi_dcs_write_seq(dsi, 0xca, 0x06);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x10);

	ret = mipi_dsi_dcs_set_tear_on(dsi, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	if (ret < 0) {
		dev_err(dev, "Failed to set tear on: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_display_brightness(dsi, 0x0fff);
	if (ret < 0) {
		dev_err(dev, "Failed to set display brightness: %d\n", ret);
		return ret;
	}

	mipi_dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x2c);
	mipi_dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_POWER_SAVE, 0x01);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(85);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	usleep_range(2000, 3000);

	return 0;
}

static int ft8719_truly_v2_off(struct ft8719_truly_v2 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(20);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(140);

	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xf7, 0x5a, 0xa5, 0x95, 0x27);

	return 0;
}

static int ft8719_truly_v2_prepare(struct drm_panel *panel)
{
	struct ft8719_truly_v2 *ctx = to_ft8719_truly_v2(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ft8719_truly_v2_reset(ctx);

	ret = ft8719_truly_v2_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int ft8719_truly_v2_unprepare(struct drm_panel *panel)
{
	struct ft8719_truly_v2 *ctx = to_ft8719_truly_v2(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = ft8719_truly_v2_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode ft8719_truly_v2_mode = {
	.clock = (1080 + 50 + 4 + 32) * (2340 + 112 + 4 + 26) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 50,
	.hsync_end = 1080 + 50 + 4,
	.htotal = 1080 + 50 + 4 + 32,
	.vdisplay = 2340,
	.vsync_start = 2340 + 112,
	.vsync_end = 2340 + 112 + 4,
	.vtotal = 2340 + 112 + 4 + 26,
	.width_mm = 65,
	.height_mm = 129,
};

static int ft8719_truly_v2_get_modes(struct drm_panel *panel,
				     struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &ft8719_truly_v2_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs ft8719_truly_v2_panel_funcs = {
	.prepare = ft8719_truly_v2_prepare,
	.unprepare = ft8719_truly_v2_unprepare,
	.get_modes = ft8719_truly_v2_get_modes,
};

static int ft8719_truly_v2_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct ft8719_truly_v2 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &ft8719_truly_v2_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	return 0;
}

static void ft8719_truly_v2_remove(struct mipi_dsi_device *dsi)
{
	struct ft8719_truly_v2 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id ft8719_truly_v2_of_match[] = {
	{ .compatible = "mdss,ft8719-truly-v2" }, 
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ft8719_truly_v2_of_match);

static struct mipi_dsi_driver ft8719_truly_v2_driver = {
	.probe = ft8719_truly_v2_probe,
	.remove = ft8719_truly_v2_remove,
	.driver = {
		.name = "panel-ft8719-truly-v2",
		.of_match_table = ft8719_truly_v2_of_match,
	},
};
module_mipi_dsi_driver(ft8719_truly_v2_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <gentoocat@gmail.com>"); 
MODULE_DESCRIPTION("DRM driver for ft8719 v2 video mode dsi truly panel");
MODULE_LICENSE("GPL");