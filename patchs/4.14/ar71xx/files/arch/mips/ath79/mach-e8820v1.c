/*
 * Support for ZTE E8820V1 boards
 *
 * Copyright (C) 2016 Piotr Dymacz <pepe2k@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/ar8216_platform.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define E8820V1_GPIO_LED_SYSTEM	20
#define E8820V1_GPIO_LED_WLAN2G	19
#define E8820V1_GPIO_LED_WLAN5G	5
#define E8820V1_GPIO_LED_WPS	7
#define E8820V1_GPIO_LED_USB	8

#define E8820V1_GPIO_BTN_RESET	2
#define E8820V1_GPIO_BTN_WPS	21

#define E8820V1_KEYS_POLL_INTERVAL	20
#define E8820V1_KEYS_DEBOUNCE_INTERVAL	\
		(3 * E8820V1_KEYS_POLL_INTERVAL)

#define E8820V1_WMAC_CALDATA_OFFSET	0x1000
#define E8820V1_PCIE_CALDATA_OFFSET	0x5000

static struct flash_platform_data e8820v1_flash_data = {
	.part_probes	= e8820v1_part_probes,
};

static struct gpio_led e8820v1_leds_gpio[] __initdata = {
	{
		.name		= "e8820v1:green:system",
		.gpio		= E8820V1_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
	{
		.name		= "e8820v1:green:wlan2g",
		.gpio		= E8820V1_GPIO_LED_WLAN2G,
		.active_low	= 1,
	},
	{
		.name		= "e8820v1:green:wlan2g",
		.gpio		= E8820V1_GPIO_LED_WLAN5G,
		.active_low	= 1,
	},
	{
		.name		= "e8820v1:green:wlan2g",
		.gpio		= E8820V1_GPIO_LED_WPS,
		.active_low	= 1,
	},
	{
		.name		= "e8820v1:green:wlan2g",
		.gpio		= E8820V1_GPIO_LED_USB,
		.active_low	= 1,
	},
};

static struct gpio_keys_button e8820v1_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = E8820V1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= E8820V1_GPIO_BTN_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = E8820V1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= E8820V1_GPIO_BTN_WPS,
		.active_low	= 1,
	},
};

static const struct ar8327_led_info e8820v1_leds_qca833x[] = {
	AR8327_LED_INFO(PHY0_0, HW, "e8820v1:green:lan1"),
	AR8327_LED_INFO(PHY1_0, HW, "e8820v1:green:lan2"),
	AR8327_LED_INFO(PHY2_0, HW, "e8820v1:green:lan3"),
	AR8327_LED_INFO(PHY3_0, HW, "e8820v1:green:lan4"),
	AR8327_LED_INFO(PHY4_0, HW, "e8820v1:green:wan"),
};

/* Blink rate: 1 Gbps -> 8 hz, 100 Mbs -> 4 Hz, 10 Mbps -> 2 Hz */
static struct ar8327_led_cfg e8820v1_qca833x_led_cfg = {
	.led_ctrl0 = 0xcf37cf37,
	.led_ctrl1 = 0xcf37cf37,
	.led_ctrl2 = 0xcf37cf37,
	.led_ctrl3 = 0x0,
	.open_drain = true,
};

static struct ar8327_pad_cfg e8820v1_qca833x_pad0_cfg = {
	.mode = AR8327_PAD_MAC_SGMII,
	.sgmii_delay_en = true,
};

static struct ar8327_platform_data e8820v1_qca833x_data = {
	.pad0_cfg = &e8820v1_qca833x_pad0_cfg,
	.port0_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	},
	.led_cfg = &e8820v1_qca833x_led_cfg,
};

static struct mdio_board_info e8820v1_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.mdio_addr = 0,
		.platform_data = &e8820v1_qca833x_data,
	},
};

static void __init e8820v1_common_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(e8820v1_flash_data);

	ath79_register_mdio(0, 0x0);
	mdiobus_register_board_info(e8820v1_mdio0_info,
				    ARRAY_SIZE(e8820v1_mdio0_info));

	/* GMAC0 is connected to QCA8334/QCA8337N switch */
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_SGMII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.speed = SPEED_1000;
	ath79_eth0_data.duplex = DUPLEX_FULL;

	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);
	ath79_register_eth(0);

	ath79_register_wmac(mac + E8820V1_WMAC_CALDATA_OFFSET, NULL);

	ap91_pci_init(mac + E8820V1_PCIE_CALDATA_OFFSET, NULL);

	ath79_gpio_direction_select(E8820V1_GPIO_LED_SYSTEM, true);
	ath79_gpio_direction_select(E8820V1_GPIO_LED_WLAN2G, true);
	ath79_gpio_direction_select(E8820V1_GPIO_LED_WLAN5G, true);
	ath79_gpio_direction_select(E8820V1_GPIO_LED_WPS, true);
	ath79_gpio_direction_select(E8820V1_GPIO_LED_USB, true);

	/* Mute LEDs on boot */
	gpio_set_value(E8820V1_GPIO_LED_SYSTEM, 1);
	gpio_set_value(E8820V1_GPIO_LED_WLAN2G, 1);

	ath79_gpio_output_select(E8820V1_GPIO_LED_SYSTEM, 0);
	ath79_gpio_output_select(E8820V1_GPIO_LED_WLAN2G, 0);

	ath79_register_gpio_keys_polled(-1, E8820V1_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(e8820v1_gpio_keys),
					e8820v1_gpio_keys);
	ath79_register_usb();
}

static void __init e8820v1_setup(void)
{
	e8820v1_qca833x_data.leds = e8820v1_leds_qca833x;
	e8820v1_qca833x_data.num_leds = ARRAY_SIZE(e8820v1_leds_qca833x);

	e8820v1_common_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(e8820v1_leds_gpio),
				 e8820v1_leds_gpio);

	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_E8820V1, "E8820V1", "ZTE E8820V1", e8820v1_setup);
