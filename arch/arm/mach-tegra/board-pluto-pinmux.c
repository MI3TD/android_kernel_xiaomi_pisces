/*
 * arch/arm/mach-tegra/board-pluto-pinmux.c
 *
 * Copyright (C) 2012 NVIDIA Corporation
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <mach/pinmux.h>
#include <mach/gpio-tegra.h>
#include "board.h"
#include "board-pluto.h"
#include "devices.h"
#include "gpio-names.h"

#if defined(CONFIG_ARCH_TEGRA_11x_SOC)
#include <mach/pinmux-t11.h>
#else
#include <mach/pinmux-tegra30.h>
#endif

#define DEFAULT_DRIVE(_name)					\
	{							\
		.pingroup = TEGRA_DRIVE_PINGROUP_##_name,	\
		.hsm = TEGRA_HSM_DISABLE,			\
		.schmitt = TEGRA_SCHMITT_ENABLE,		\
		.drive = TEGRA_DRIVE_DIV_1,			\
		.pull_down = TEGRA_PULL_31,			\
		.pull_up = TEGRA_PULL_31,			\
		.slew_rising = TEGRA_SLEW_SLOWEST,		\
		.slew_falling = TEGRA_SLEW_SLOWEST,		\
	}
/* Setting the drive strength of pins
 * hsm: Enable High speed mode (ENABLE/DISABLE)
 * Schimit: Enable/disable schimit (ENABLE/DISABLE)
 * drive: low power mode (DIV_1, DIV_2, DIV_4, DIV_8)
 * pulldn_drive - drive down (falling edge) - Driver Output Pull-Down drive
 *                strength code. Value from 0 to 31.
 * pullup_drive - drive up (rising edge)  - Driver Output Pull-Up drive
 *                strength code. Value from 0 to 31.
 * pulldn_slew -  Driver Output Pull-Up slew control code  - 2bit code
 *                code 11 is least slewing of signal. code 00 is highest
 *                slewing of the signal.
 *                Value - FASTEST, FAST, SLOW, SLOWEST
 * pullup_slew -  Driver Output Pull-Down slew control code -
 *                code 11 is least slewing of signal. code 00 is highest
 *                slewing of the signal.
 *                Value - FASTEST, FAST, SLOW, SLOWEST
 */
#define SET_DRIVE(_name, _hsm, _schmitt, _drive, _pulldn_drive, _pullup_drive, _pulldn_slew, _pullup_slew) \
	{                                               \
		.pingroup = TEGRA_DRIVE_PINGROUP_##_name,   \
		.hsm = TEGRA_HSM_##_hsm,                    \
		.schmitt = TEGRA_SCHMITT_##_schmitt,        \
		.drive = TEGRA_DRIVE_##_drive,              \
		.pull_down = TEGRA_PULL_##_pulldn_drive,    \
		.pull_up = TEGRA_PULL_##_pullup_drive,		\
		.slew_rising = TEGRA_SLEW_##_pulldn_slew,   \
		.slew_falling = TEGRA_SLEW_##_pullup_slew,	\
	}

#define DEFAULT_PINMUX(_pingroup, _mux, _pupd, _tri, _io)	\
	{							\
		.pingroup	= TEGRA_PINGROUP_##_pingroup,	\
		.func		= TEGRA_MUX_##_mux,		\
		.pupd		= TEGRA_PUPD_##_pupd,		\
		.tristate	= TEGRA_TRI_##_tri,		\
		.io		= TEGRA_PIN_##_io,		\
		.lock		= TEGRA_PIN_LOCK_DEFAULT,	\
		.od		= TEGRA_PIN_OD_DEFAULT,		\
		.ioreset	= TEGRA_PIN_IO_RESET_DEFAULT,	\
	}

#define I2C_PINMUX(_pingroup, _mux, _pupd, _tri, _io, _lock, _od) \
	{							\
		.pingroup	= TEGRA_PINGROUP_##_pingroup,	\
		.func		= TEGRA_MUX_##_mux,		\
		.pupd		= TEGRA_PUPD_##_pupd,		\
		.tristate	= TEGRA_TRI_##_tri,		\
		.io		= TEGRA_PIN_##_io,		\
		.lock		= TEGRA_PIN_LOCK_##_lock,	\
		.od		= TEGRA_PIN_OD_##_od,		\
		.ioreset	= TEGRA_PIN_IO_RESET_DEFAULT,	\
	}

#define VI_PINMUX(_pingroup, _mux, _pupd, _tri, _io, _lock, _ioreset) \
	{							\
		.pingroup	= TEGRA_PINGROUP_##_pingroup,	\
		.func		= TEGRA_MUX_##_mux,		\
		.pupd		= TEGRA_PUPD_##_pupd,		\
		.tristate	= TEGRA_TRI_##_tri,		\
		.io		= TEGRA_PIN_##_io,		\
		.lock		= TEGRA_PIN_LOCK_##_lock,	\
		.od		= TEGRA_PIN_OD_DEFAULT,		\
		.ioreset	= TEGRA_PIN_IO_RESET_##_ioreset	\
	}

#define CEC_PINMUX(_pingroup, _mux, _pupd, _tri, _io, _lock, _od)   \
	{							\
		.pingroup   = TEGRA_PINGROUP_##_pingroup,	\
		.func       = TEGRA_MUX_##_mux,			\
		.pupd       = TEGRA_PUPD_##_pupd,		\
		.tristate   = TEGRA_TRI_##_tri,			\
		.io         = TEGRA_PIN_##_io,			\
		.lock       = TEGRA_PIN_LOCK_##_lock,		\
		.od         = TEGRA_PIN_OD_##_od,		\
		.ioreset    = TEGRA_PIN_IO_RESET_DEFAULT,	\
	}

/* We are disabling this code for now. */
#define GPIO_INIT_PIN_MODE(_gpio, _is_input, _value)	\
	{					\
		.gpio_nr	= _gpio,	\
		.is_input	= _is_input,	\
		.value		= _value,	\
	}

#if defined(CONFIG_ARCH_TEGRA_11x_SOC)

static __initdata struct tegra_drive_pingroup_config pluto_drive_pinmux[] = {
	/* DEFAULT_DRIVE(<pin_group>), */
	/* SDMMC1 */
	SET_DRIVE(SDIO1, DISABLE, DISABLE, DIV_1, 36, 20, SLOW, SLOW),

	/* SDMMC3 */
	SET_DRIVE(SDIO3, DISABLE, DISABLE, DIV_1, 22, 36, FASTEST, FASTEST),

	/* SDMMC4 */
	SET_DRIVE(GMA, DISABLE, DISABLE, DIV_1, 2, 1, FASTEST, FASTEST),
};

/* Initially setting all used GPIO's to non-TRISTATE */
static __initdata struct tegra_pingroup_config pluto_pinmux_set_nontristate[] = {
	DEFAULT_PINMUX(GPIO_X4_AUD,    RSVD,    PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_X5_AUD,    RSVD,    PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_X6_AUD,    RSVD3,   PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_X7_AUD,    RSVD,    PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_W2_AUD,    RSVD1,   PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_W3_AUD,    SPI6,    PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_X1_AUD,    RSVD3,   PULL_DOWN,    NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_X3_AUD,    RSVD3,   PULL_UP,      NORMAL,    INPUT),

	DEFAULT_PINMUX(GPIO_PV0,      RSVD3,    NORMAL,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_PV1,      RSVD,     NORMAL,      NORMAL,    INPUT),
	DEFAULT_PINMUX(ULPI_DATA0,    ULPI,     NORMAL,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA1,    ULPI,     NORMAL,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA2,    ULPI,     NORMAL,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA3,    ULPI,     NORMAL,      NORMAL,    INPUT),
	DEFAULT_PINMUX(ULPI_DATA4,    ULPI,     NORMAL,      NORMAL,    INPUT),
	DEFAULT_PINMUX(ULPI_DATA5,    ULPI,     NORMAL,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA6,    ULPI,     NORMAL,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA7,    ULPI,     NORMAL,      NORMAL,    OUTPUT),

	DEFAULT_PINMUX(GPIO_PBB3,     RSVD3,    PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_PBB4,     RSVD3,    PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_PBB5,     RSVD3,    PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_PBB6,     RSVD3,    PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_PBB7,     RSVD3,    PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_PCC1,     RSVD3,    PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_PCC2,     RSVD3,    PULL_DOWN,    NORMAL,    INPUT),

	DEFAULT_PINMUX(GMI_AD1,     GMI,      NORMAL,       NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD10,    GMI,      PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD12,    GMI,      PULL_UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD13,    GMI,      PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD14,    GMI,      PULL_UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD2,     GMI,      NORMAL,       NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_AD3,     GMI,      NORMAL,       NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_AD8,     GMI,      PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_ADV_N,   GMI,      PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_CLK,     GMI,      PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_CS0_N,   GMI,      PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_CS3_N,   GMI,      PULL_UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_CS4_N,   GMI,      PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_CS7_N,   GMI,      PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_IORDY,   GMI,      PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_RST_N,   GMI,      PULL_DOWN,    NORMAL,    INPUT),

	DEFAULT_PINMUX(SDMMC1_WP_N, SDMMC1,      PULL_UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(CLK2_REQ,    RSVD3,     NORMAL,       NORMAL,    OUTPUT),

	DEFAULT_PINMUX(KB_COL3,     KBC,      PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_COL4,     KBC,      PULL_UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_COL6,     KBC,      PULL_UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(KB_COL7,     KBC,      PULL_UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(KB_ROW3,     KBC,      PULL_DOWN,    NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_ROW4,     KBC,      PULL_DOWN,    NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_ROW5,     KBC,      PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(KB_ROW6,     KBC,      PULL_DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(KB_ROW7,     KBC,      PULL_DOWN,    NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_ROW8,     KBC,      PULL_DOWN,    NORMAL,    INPUT),

	DEFAULT_PINMUX(CLK3_REQ,    RSVD3,    NORMAL,      NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GPIO_PU4,    RSVD3,     NORMAL,      NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GPIO_PU5,    RSVD3,     NORMAL,      NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PU6,    RSVD3,      NORMAL,      NORMAL,     INPUT),

	DEFAULT_PINMUX(HDMI_INT,    RSVD,      PULL_DOWN,    NORMAL,    INPUT),
};

#include "board-pluto-pinmux-t11x.h"

#else

/* !!!FIXME!!!! POPULATE THIS TABLE */
static __initdata struct tegra_drive_pingroup_config pluto_drive_pinmux[] = {
	/* DEFAULT_DRIVE(<pin_group>), */
	/* SET_DRIVE(ATA, DISABLE, DISABLE, DIV_1, 31, 31, FAST, FAST) */
	SET_DRIVE(DAP2,		DISABLE, ENABLE, DIV_1, 31, 31, FASTEST, FASTEST),

	/* All I2C pins are driven to maximum drive strength */
	/* GEN1 I2C */
	SET_DRIVE(DBG,		DISABLE, ENABLE, DIV_1, 31, 31, FASTEST, FASTEST),

	/* GEN2 I2C */
	SET_DRIVE(AT5,		DISABLE, ENABLE, DIV_1, 31, 31, FASTEST, FASTEST),

	/* CAM I2C */
	SET_DRIVE(GME,		DISABLE, ENABLE, DIV_1, 31, 31, FASTEST, FASTEST),

	/* DDC I2C */
	SET_DRIVE(DDC,		DISABLE, ENABLE, DIV_1, 31, 31, FASTEST, FASTEST),

	/* PWR_I2C */
	SET_DRIVE(AO1,		DISABLE, ENABLE, DIV_1, 31, 31, FASTEST, FASTEST),

	/* UART3 */
	SET_DRIVE(UART3,	DISABLE, ENABLE, DIV_1, 31, 31, FASTEST, FASTEST),

	/* SDMMC1 */
	SET_DRIVE(SDIO1,	DISABLE, DISABLE, DIV_1, 46, 42, FAST, FAST),

	/* SDMMC3 */
	SET_DRIVE(SDIO3,	DISABLE, DISABLE, DIV_1, 46, 42, FAST, FAST),

	/* SDMMC4 */
	SET_DRIVE(GMA,		DISABLE, DISABLE, DIV_1, 9, 9, SLOWEST, SLOWEST),
	SET_DRIVE(GMB,		DISABLE, DISABLE, DIV_1, 9, 9, SLOWEST, SLOWEST),
	SET_DRIVE(GMC,		DISABLE, DISABLE, DIV_1, 9, 9, SLOWEST, SLOWEST),
	SET_DRIVE(GMD,		DISABLE, DISABLE, DIV_1, 9, 9, SLOWEST, SLOWEST),

};

static __initdata struct tegra_pingroup_config pluto_pinmux_common[] = {
	/* SDMMC1 pinmux */
	DEFAULT_PINMUX(SDMMC1_CLK,      SDMMC1,          NORMAL,     NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC1_CMD,      SDMMC1,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC1_DAT3,     SDMMC1,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC1_DAT2,     SDMMC1,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC1_DAT1,     SDMMC1,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC1_DAT0,     SDMMC1,          PULL_UP,    NORMAL,     INPUT),

	/* SDMMC3 pinmux */
	DEFAULT_PINMUX(SDMMC3_CLK,      SDMMC3,          NORMAL,     NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC3_CMD,      SDMMC3,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT0,     SDMMC3,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT1,     SDMMC3,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT2,     SDMMC3,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT3,     SDMMC3,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT6,     SDMMC3,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT7,     SDMMC3,          PULL_UP,    NORMAL,     INPUT),

	/* SDMMC4 pinmux */
	DEFAULT_PINMUX(SDMMC4_CLK,      SDMMC4,          NORMAL,     NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_CMD,      SDMMC4,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_DAT0,     SDMMC4,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_DAT1,     SDMMC4,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_DAT2,     SDMMC4,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_DAT3,     SDMMC4,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_DAT4,     SDMMC4,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_DAT5,     SDMMC4,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_DAT6,     SDMMC4,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_DAT7,     SDMMC4,          PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SDMMC4_RST_N,    RSVD1,           PULL_DOWN,  NORMAL,     INPUT),

	/* I2C1 pinmux */
	I2C_PINMUX(GEN1_I2C_SCL,	I2C1,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),
	I2C_PINMUX(GEN1_I2C_SDA,	I2C1,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),

	/* I2C2 pinmux */
	I2C_PINMUX(GEN2_I2C_SCL,	I2C2,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),
	I2C_PINMUX(GEN2_I2C_SDA,	I2C2,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),

	/* I2C3 pinmux */
	I2C_PINMUX(CAM_I2C_SCL,		I2C3,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),
	I2C_PINMUX(CAM_I2C_SDA,		I2C3,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),

	/* I2C4 pinmux */
	I2C_PINMUX(DDC_SCL,		I2C4,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),
	I2C_PINMUX(DDC_SDA,		I2C4,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),

	/* Power I2C pinmux */
	I2C_PINMUX(PWR_I2C_SCL,		I2CPWR,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),
	I2C_PINMUX(PWR_I2C_SDA,		I2CPWR,		NORMAL,	NORMAL,	INPUT,	DISABLE,	ENABLE),

	/* LCD */
	DEFAULT_PINMUX(LCD_PCLK,        DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_DE,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_HSYNC,       DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_VSYNC,       DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D0,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D1,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D2,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D3,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D4,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D5,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D6,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D7,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D8,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D9,          DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D10,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D11,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D12,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D13,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D14,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D15,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D16,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D17,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D18,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D19,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D20,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D21,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D22,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_D23,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),

	/* UART B : GPS */
	DEFAULT_PINMUX(UART2_RXD,       IRDA,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(UART2_TXD,       IRDA,            NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(UART2_RTS_N,     UARTB,           NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(UART2_CTS_N,     UARTB,           NORMAL,    NORMAL,     INPUT),

	/*UART C : BT */
	DEFAULT_PINMUX(UART3_TXD,       UARTC,           NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(UART3_RXD,       UARTC,           NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(UART3_CTS_N,     UARTC,           NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(UART3_RTS_N,     UARTC,           NORMAL,    NORMAL,     OUTPUT),

#ifdef T30_ON_T114
	/* interposer SPI4 */
	DEFAULT_PINMUX(GMI_A16,         SPI4,      NORMAL,    NORMAL,   INPUT),
	DEFAULT_PINMUX(GMI_A17,         SPI4,      NORMAL,    NORMAL,   INPUT),
	DEFAULT_PINMUX(GMI_A18,         SPI4,      NORMAL,    NORMAL,   INPUT),
	DEFAULT_PINMUX(GMI_A19,         SPI4,      NORMAL,    NORMAL,   INPUT),
#else
	/* UART D : DEBUG */
	DEFAULT_PINMUX(GMI_A16,         UARTD,     NORMAL,    NORMAL,  OUTPUT),
	DEFAULT_PINMUX(GMI_A17,         UARTD,     NORMAL,    NORMAL,  INPUT),
	DEFAULT_PINMUX(GMI_A18,         UARTD,     NORMAL,    NORMAL,  INPUT),
	DEFAULT_PINMUX(GMI_A19,         UARTD,     NORMAL,    NORMAL,  OUTPUT),
#endif

	/*  KBC keys */
	DEFAULT_PINMUX(KB_COL0,         KBC,             PULL_UP,   NORMAL,     INPUT),
	DEFAULT_PINMUX(KB_COL1,         KBC,             PULL_UP,   NORMAL,     INPUT),
	DEFAULT_PINMUX(KB_COL2,         KBC,             PULL_UP,   NORMAL,     INPUT),
	DEFAULT_PINMUX(KB_COL3,         KBC,             PULL_UP,   NORMAL,     INPUT),
	DEFAULT_PINMUX(KB_ROW0,         KBC,             PULL_UP,   NORMAL,     INPUT),
	DEFAULT_PINMUX(KB_ROW1,         KBC,             PULL_UP,   NORMAL,     INPUT),
	DEFAULT_PINMUX(KB_ROW2,         KBC,             PULL_UP,   NORMAL,     INPUT),

	/* I2S0 : for MODEM */
	DEFAULT_PINMUX(DAP1_FS,         I2S0,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(DAP1_DIN,        I2S0,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(DAP1_DOUT,       I2S0,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(DAP1_SCLK,       I2S0,            NORMAL,    NORMAL,     INPUT),

	/* I2S1 : for CODEC */
	DEFAULT_PINMUX(DAP2_FS,         I2S1,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(DAP2_DIN,        I2S1,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(DAP2_DOUT,       I2S1,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(DAP2_SCLK,       I2S1,            NORMAL,    NORMAL,     INPUT),

	/* I2S3 : for BT */
	DEFAULT_PINMUX(DAP4_FS,         I2S3,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(DAP4_DIN,        I2S3,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(DAP4_DOUT,       I2S3,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(DAP4_SCLK,       I2S3,            NORMAL,    NORMAL,     INPUT),

	/* SPI1 : touch */
	DEFAULT_PINMUX(SPI1_MOSI,       SPI1,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SPI1_SCK,        SPI1,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SPI1_CS0_N,      SPI1,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SPI1_MISO,       SPI1,            NORMAL,    NORMAL,     INPUT),

	/* SPIDIF */
	DEFAULT_PINMUX(SPDIF_IN,        SPDIF,           PULL_UP,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SPDIF_OUT,       SPDIF,           NORMAL,    NORMAL,     OUTPUT),

	/* Audience a2220 codec */
	DEFAULT_PINMUX(ULPI_DATA5,      SPI2,            PULL_UP,   NORMAL,     INPUT),
	DEFAULT_PINMUX(GMI_AD12,        GMI,             NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(KB_ROW3,         KBC,             PULL_DOWN, NORMAL,     INPUT),

	/* FIXED FUNCTION AND CONFIGURATION */
	DEFAULT_PINMUX(CLK_32K_OUT,     BLINK,           NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(SYS_CLK_REQ,     SYSCLK,          NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(OWR,             OWR,             NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GMI_AD4,         RSVD1,           NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(CLK1_OUT,        EXTPERIPH1,      NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(CLK2_OUT,        EXTPERIPH2,  NORMAL,    NORMAL, OUTPUT),
	DEFAULT_PINMUX(CLK3_OUT,        EXTPERIPH3,      NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(CLK2_REQ,        DAP,             NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(HDMI_INT,        RSVD0,           NORMAL,    TRISTATE,   INPUT),

	/* GPIO */
	/* POWER RAIL GPIO */
	DEFAULT_PINMUX(DAP3_FS,         I2S2,            NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GMI_AD14,        RSVD1,           PULL_DOWN, NORMAL,     OUTPUT),
	DEFAULT_PINMUX(SDMMC3_DAT5,     SDMMC3,          NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(KB_ROW6,         KBC,             NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(KB_ROW7,         KBC,             NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(LCD_M1,          DISPLAYA,        NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(LCD_PWR0,        DISPLAYA,        NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(LCD_PWR1,        DISPLAYA,        NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(LCD_PWR2,        DISPLAYA,        NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(KB_ROW8,         KBC,             NORMAL,    NORMAL,     OUTPUT),

	/* CAMERA */
	DEFAULT_PINMUX(CAM_MCLK,        VI_ALT2,         NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PCC1,       RSVD1,           NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PBB0,       RSVD1,           NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PBB3,       VGP3,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PBB5,       VGP5,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PBB6,       VGP6,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PBB7,       I2S4,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PCC2,       I2S4,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(KB_ROW4,         KBC,             NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(KB_ROW5,         KBC,             NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(KB_ROW9,         KBC,             NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(KB_ROW10,        KBC,             NORMAL,    NORMAL,     OUTPUT),

	/* MODEM */
	DEFAULT_PINMUX(GPIO_PV0,        RSVD,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PV1,        RSVD,            NORMAL,    NORMAL,     INPUT),

	/* GPS and BT */
	DEFAULT_PINMUX(GPIO_PU0,        UARTA,           NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GPIO_PU1,        UARTA,           NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PU2,        UARTA,           NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PU3,        UARTA,           NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GPIO_PU4,        PWM1,            NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GPIO_PU5,        PWM2,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GPIO_PU6,        RSVD1,           NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(KB_ROW14,        KBC,             NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(KB_COL6,         KBC,             PULL_DOWN, NORMAL,   OUTPUT),
	DEFAULT_PINMUX(KB_COL7,         KBC,             PULL_DOWN, NORMAL,   OUTPUT),
	DEFAULT_PINMUX(CLK3_REQ,        DEV3,            NORMAL,    NORMAL,   OUTPUT),

	/* LCD GPIO */
	DEFAULT_PINMUX(GMI_AD0,         RSVD1,           NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GMI_AD1,         RSVD1,           NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GMI_AD2,         RSVD1,           PULL_DOWN, NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GMI_AD3,         RSVD1,           PULL_DOWN, NORMAL,     OUTPUT),
#ifdef T30_ON_T114
	DEFAULT_PINMUX(GMI_AD5,         RSVD1,     PULL_DOWN, NORMAL,   OUTPUT),
	DEFAULT_PINMUX(GMI_AD6,         RSVD1,     PULL_DOWN, NORMAL,   OUTPUT),
	DEFAULT_PINMUX(GMI_AD7,         RSVD1,     PULL_DOWN, NORMAL,   OUTPUT),
#else
	DEFAULT_PINMUX(GMI_AD5,         SPI4,         NORMAL, NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_AD6,         SPI4,         NORMAL, NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_AD7,         SPI4,         NORMAL, NORMAL,    INPUT),
#endif
	DEFAULT_PINMUX(GMI_AD8,         PWM0,            NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GMI_AD9,         RSVD2,           PULL_DOWN, NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GMI_AD11,        PWM3,            NORMAL,    NORMAL,     OUTPUT),

	/* TOUCH */
	DEFAULT_PINMUX(GMI_WAIT,        RSVD1,           PULL_UP,   NORMAL,     INPUT),
	DEFAULT_PINMUX(GMI_WP_N,        RSVD1,           PULL_UP,   NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_SDOUT,       DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_DC1,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_WR_N,        DISPLAYA,        PULL_UP,   NORMAL,     INPUT),

	/* SDMMC */
	DEFAULT_PINMUX(GMI_IORDY,       RSVD1,           PULL_UP,   NORMAL,     INPUT),

	/* CODEC */
	DEFAULT_PINMUX(SPI2_SCK,        SPI2,            NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(SPI2_CS1_N,      SPI2,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GMI_CS2_N,       RSVD1,           NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(GMI_CS3_N,       RSVD1,           NORMAL,    NORMAL,     INPUT),

	/* OTHERS */
	DEFAULT_PINMUX(GMI_DQS,         RSVD1,           NORMAL,    NORMAL,     INPUT),

	DEFAULT_PINMUX(GMI_AD15,        RSVD1,           PULL_UP,   NORMAL,     OUTPUT),
	DEFAULT_PINMUX(GMI_CLK,         RSVD1,           PULL_UP,   NORMAL,     INPUT),

	DEFAULT_PINMUX(GMI_RST_N,       NAND,            PULL_UP,   NORMAL,     OUTPUT),
	DEFAULT_PINMUX(LCD_DC0,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_CS0_N,       DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_CS1_N,       DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_SCK,         DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(LCD_SDIN,        DISPLAYA,        NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(CRT_HSYNC,       CRT,             NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(CRT_VSYNC,       CRT,             NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(PEX_WAKE_N,      PCIE,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(PEX_L2_PRSNT_N,  PCIE,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(PEX_L2_RST_N,    PCIE,            NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(PEX_L2_CLKREQ_N, PCIE,            NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(HDMI_CEC,        CEC,             NORMAL,    NORMAL,     INPUT),

	DEFAULT_PINMUX(KB_ROW15,        KBC,             NORMAL,    NORMAL,     OUTPUT),
	DEFAULT_PINMUX(SPI2_CS2_N,      SPI2,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SPI2_MISO,       SPI2,            NORMAL,    NORMAL,     INPUT),
	DEFAULT_PINMUX(SPI2_MOSI,       SPI2,            NORMAL,    NORMAL,     INPUT),

	DEFAULT_PINMUX(KB_ROW11,        KBC,             PULL_UP,   TRISTATE,   INPUT),
	DEFAULT_PINMUX(KB_ROW12,        KBC,             NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(KB_ROW13,        KBC,             NORMAL,    TRISTATE,   OUTPUT),
};

/*Do not use for now*/
static __initdata struct tegra_pingroup_config unused_pins_lowpower[] = {
	DEFAULT_PINMUX(ULPI_CLK,        ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA0,      ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA1,      ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA2,      ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA3,      ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA4,      ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA6,      ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_DATA7,      ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_DIR,        ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_NXT,        ULPI,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(ULPI_STP,        ULPI,          NORMAL,    TRISTATE,   OUTPUT),

	DEFAULT_PINMUX(GMI_AD10,        PWM2,          NORMAL,    TRISTATE,   OUTPUT),

	DEFAULT_PINMUX(GMI_AD13,        RSVD1,         NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(CLK1_REQ,        DAP,           NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(GMI_ADV_N,       RSVD1,         NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(GMI_CS0_N,       RSVD1,         NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(GMI_CS1_N,       RSVD1,         NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(GMI_CS4_N,       RSVD1,         NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(GMI_CS6_N,       NAND,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(GMI_CS7_N,       NAND,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(GMI_OE_N,        RSVD1,         NORMAL,    TRISTATE,   OUTPUT),
#ifdef T30_ON_T114 /* SPI4 pinmux */
	DEFAULT_PINMUX(GMI_WR_N,        RSVD1,         NORMAL,    TRISTATE,   OUTPUT),
#else
	DEFAULT_PINMUX(GMI_WR_N,        SPI4,          NORMAL,    NORMAL,   INPUT),
#endif

	DEFAULT_PINMUX(PEX_L0_CLKREQ_N, PCIE,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(PEX_L0_PRSNT_N,  PCIE,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(PEX_L0_RST_N,    PCIE,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(PEX_L1_CLKREQ_N, PCIE,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(PEX_L1_PRSNT_N,  PCIE,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(PEX_L1_RST_N,    PCIE,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(GPIO_PV2,        OWR,           NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(GPIO_PV3,        RSVD1,         NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(HDMI_CEC,        CEC,           NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(KB_COL4,         KBC,           NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(KB_COL5,         KBC,           NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D0,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D1,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D10,          VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D11,          VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D2,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D3,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D4,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D5,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D6,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D7,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D8,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_D9,           VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_HSYNC,        VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_MCLK,         VI,            PULL_UP,    NORMAL,    INPUT),
	DEFAULT_PINMUX(VI_PCLK,         VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(VI_VSYNC,        VI,            NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(DAP3_DIN,        I2S2,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(DAP3_DOUT,       I2S2,          NORMAL,    TRISTATE,   OUTPUT),
	DEFAULT_PINMUX(DAP3_SCLK,       I2S2,          NORMAL,    TRISTATE,   OUTPUT),

};

static void __init pluto_pinmux_audio_init(void)
{
	gpio_request(TEGRA_GPIO_CDC_IRQ, "rt5640");
	gpio_direction_input(TEGRA_GPIO_CDC_IRQ);

}

static struct gpio_init_pin_info init_gpio_mode_pluto_common[] = {
	GPIO_INIT_PIN_MODE(TEGRA_GPIO_PDD7, false, 0),
	GPIO_INIT_PIN_MODE(TEGRA_GPIO_PCC6, false, 0),
	GPIO_INIT_PIN_MODE(TEGRA_GPIO_PCC7, false, 1),
};

#endif

static void __init pluto_gpio_init_configure(void)
{
	int len;
	int i;
	struct gpio_init_pin_info *pins_info;

	len = ARRAY_SIZE(init_gpio_mode_pluto_common);
	pins_info = init_gpio_mode_pluto_common;

	for (i = 0; i < len; ++i) {
		tegra_gpio_init_configure(pins_info->gpio_nr,
			pins_info->is_input, pins_info->value);
		pins_info++;
	}
}

int __init pluto_pinmux_init(void)
{
	struct board_info board_info;
	tegra_get_board_info(&board_info);
	/*BUG_ON(board_info.board_id != BOARD_E1565);*/

#if defined(CONFIG_ARCH_TEGRA_11x_SOC)
	tegra_pinmux_config_table(pluto_pinmux_set_nontristate,
					ARRAY_SIZE(pluto_pinmux_set_nontristate));
	pluto_gpio_init_configure();

	tegra_pinmux_config_table(pluto_pinmux_common, ARRAY_SIZE(pluto_pinmux_common));
	tegra_drive_pinmux_config_table(pluto_drive_pinmux,
					ARRAY_SIZE(pluto_drive_pinmux));
	tegra_pinmux_config_table(unused_pins_lowpower,
		ARRAY_SIZE(unused_pins_lowpower));

#else
	tegra30_default_pinmux();

	pluto_gpio_init_configure();

	tegra_pinmux_config_table(pluto_pinmux_common, ARRAY_SIZE(pluto_pinmux_common));
	tegra_drive_pinmux_config_table(pluto_drive_pinmux,
					ARRAY_SIZE(pluto_drive_pinmux));

	tegra_pinmux_config_table(unused_pins_lowpower,
		ARRAY_SIZE(unused_pins_lowpower));
	pluto_pinmux_audio_init();
#endif

	return 0;
}