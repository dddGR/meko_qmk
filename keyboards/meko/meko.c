/*
Copyright 2022 aki27

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <math.h>

// Invert vertical scroll direction
#ifndef MEKO_SCROLL_INV_DEFAULT
#    define MEKO_SCROLL_INV_DEFAULT 1
#endif

// All the default values here refer to O-based array indexes
// not actual values

#ifndef MEKO_CPI_OPTIONS
#    define MEKO_CPI_OPTIONS {800, 1000, 1200, 1600}
#endif
#ifndef MEKO_CPI_DEFAULT
#    define MEKO_CPI_DEFAULT 0
#endif

#ifndef MEKO_SCROLL_DIVIDERS
#    define MEKO_SCROLL_DIVIDERS {1, 2, 3, 4, 5, 6}
#endif
#ifndef MEKO_SCROLL_DIV_DEFAULT
#    define MEKO_SCROLL_DIV_DEFAULT (1)
#endif

#ifndef MEKO_ROTATION_ANGLE
#    define MEKO_ROTATION_ANGLE {-60, -45, -30, -15, 0, 15, 30, 45, 60}
#endif
#ifndef MEKO_ROTATION_DEFAULT
#    define MEKO_ROTATION_DEFAULT (4)
#endif

meko_config_t meko_config;
uint16_t      cpi_array[]      = MEKO_CPI_OPTIONS;
uint16_t      scrl_div_array[] = MEKO_SCROLL_DIVIDERS;
int8_t        angle_array[]    = MEKO_ROTATION_ANGLE;
#define CPI_OPTION_SIZE (sizeof(cpi_array) / sizeof(uint16_t))
#define SCRL_DIV_SIZE (sizeof(scrl_div_array) / sizeof(uint16_t))
#define ANGLE_SIZE (sizeof(angle_array) / sizeof(int8_t))

// Trackball State
// bool     BurstState  = false; // init burst state for Trackball module
// uint16_t MotionStart = 0;     // Timer for accel, 0 is resting state

void pointing_device_init_kb(void) {
    // set the CPI.
    pointing_device_set_cpi(cpi_array[meko_config.cpi_idx]);
    // Configure ADNS5050 sensor hardware
    // adns5050_write_reg(0x22, 0b10000 | 0x80);
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    static float x_accumulator = 0.0;
    static float y_accumulator = 0.0;
    static float prev_x = 0.0, prev_y = 0.0;

    float sensitivity            = 0.6; // Movement sensitivity
    float smoothing_factor       = 0.7; // Smoothing factor
    float sensitivity_multiplier = 1.0; // Sensitivity adjustment multiplier

    // TODO: not using rotation angle for now
    // // Apply rotation angle adjustment
    // double rad       = (double)angle_array[meko_config.rotation_angle] * (M_PI / 180) * -1;
    // float  rotated_x = mouse_report.x * cos(rad) - mouse_report.y * sin(rad);
    // float  rotated_y = mouse_report.x * sin(rad) + mouse_report.y * cos(rad);
    // Apply smoothing to the rotated values
    // float smoothed_x = prev_x * smoothing_factor + rotated_x * (1.0 - smoothing_factor);
    // float smoothed_y = prev_y * smoothing_factor + rotated_y * (1.0 - smoothing_factor);
    float smoothed_x = prev_x * smoothing_factor + (float)mouse_report.x * (1.0 - smoothing_factor);
    float smoothed_y = prev_y * smoothing_factor + (float)mouse_report.y * (1.0 - smoothing_factor);
    prev_x           = smoothed_x;
    prev_y           = smoothed_y;

    // Apply sensitivity multiplier
    smoothed_x *= sensitivity_multiplier;
    smoothed_y *= sensitivity_multiplier;

    // Scroll mode handling
    if (meko_get_scroll_mode()) {
        static int h_acm = 0, v_acm = 0;

        // Determine scroll direction
        if (abs((int)smoothed_x) > abs((int)smoothed_y)) {
            smoothed_y = 0; // Horizontal scroll
        } else {
            smoothed_x = 0; // Vertical scroll
        }

        // Accumulate scroll values
        if (meko_config.scrl_inv > 0) {
            h_acm += smoothed_x;
            v_acm -= smoothed_y;
        } else {
            h_acm -= smoothed_x;
            v_acm += smoothed_y;
        }

        // Calculate scroll values with division factor
        int8_t h_scroll = h_acm >> scrl_div_array[meko_config.scrl_div];
        int8_t v_scroll = v_acm >> scrl_div_array[meko_config.scrl_div];

        // Apply scroll to mouse report
        if (h_scroll != 0) {
            mouse_report.h += h_scroll;
            h_acm -= h_scroll << scrl_div_array[meko_config.scrl_div];
        }
        if (v_scroll != 0) {
            mouse_report.v += v_scroll;
            v_acm -= v_scroll << scrl_div_array[meko_config.scrl_div];
        }

        // Reset X/Y movement in scroll mode
        mouse_report.x = 0;
        mouse_report.y = 0;
    } else {
        // Movement smoothing and accumulation for normal mode
        x_accumulator += smoothed_x * sensitivity;
        y_accumulator += smoothed_y * sensitivity;
        if (fabs(x_accumulator) >= 1.0) {
            mouse_report.x = (int8_t)x_accumulator;
            x_accumulator -= mouse_report.x;
        } else {
            mouse_report.x = 0;
        }
        if (fabs(y_accumulator) >= 1.0) {
            mouse_report.y = (int8_t)y_accumulator;
            y_accumulator -= mouse_report.y;
        } else {
            mouse_report.y = 0;
        }
    }

    return pointing_device_task_user(mouse_report);
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    // xprintf("KL: kc: %u, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);

    if (!process_record_user(keycode, record)) {
        return false;
    }

    // if (keycode == CPI_SW && record->event.pressed) {
    //     meko_config.cpi_idx = (meko_config.cpi_idx + 1) % CPI_OPTION_SIZE;
    //     eeconfig_update_kb(meko_config.raw);
    //     pointing_device_set_cpi(cpi_array[meko_config.cpi_idx]);
    // }

    // if (keycode == SCRL_SW && record->event.pressed) {
    //     meko_config.scrl_div = (meko_config.scrl_div + 1) % SCRL_DIV_SIZE;
    //     eeconfig_update_kb(meko_config.raw);
    // }

    // if (keycode == ROT_R15 && record->event.pressed) {
    //     meko_config.rotation_angle = (meko_config.rotation_angle + 1) % ANGLE_SIZE;
    //     eeconfig_update_kb(meko_config.raw);
    // }

    // if (keycode == ROT_L15 && record->event.pressed) {
    //     meko_config.rotation_angle = (ANGLE_SIZE + meko_config.rotation_angle - 1) % ANGLE_SIZE;
    //     eeconfig_update_kb(meko_config.raw);
    // }

    // if (keycode == SCRL_IN && record->event.pressed) {
    //     meko_config.scrl_inv = -meko_config.scrl_inv;
    //     eeconfig_update_kb(meko_config.raw);
    // }

    // if (keycode == SCRL_TO && record->event.pressed) {
    //     meko_config.scrl_mode ^= 1;
    // }

    // if (keycode == SCRL_MO) {
    //     meko_config.scrl_mode ^= 1;
    // }

    return true;
}

void eeconfig_init_kb(void) {
    meko_config.cpi_idx        = MEKO_CPI_DEFAULT;
    meko_config.scrl_div       = MEKO_SCROLL_DIV_DEFAULT;
    meko_config.rotation_angle = MEKO_ROTATION_DEFAULT;
    meko_config.scrl_inv       = MEKO_SCROLL_INV_DEFAULT;
    meko_config.scrl_mode      = false;
    eeconfig_update_kb(meko_config.raw);
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    // is safe to just read CPI setting since matrix init
    // comes before pointing device init.
    meko_config.raw = eeconfig_read_kb();
    if (meko_config.cpi_idx > CPI_OPTION_SIZE) // || meko_config.scrl_div > SCRL_DIV_SIZE || meko_config.rotation_angle > ANGLE_SIZE)
    {
        eeconfig_init_kb();
    }
    matrix_init_user();
}

bool meko_get_scroll_mode(void) {
    return meko_config.scrl_mode;
}

void meko_set_scroll_mode(bool mode) {
    meko_config.scrl_mode = mode;
}

// OLED utility
#ifdef OLED_ENABLE

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    render_logo();
    oled_write_layer_state();
    return false;
}

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    return OLED_ROTATION_180;
}

static const char PROGMEM meko_logo[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0};

void render_logo(void) {
    oled_write_P(meko_logo, false);
};

// This keep gcc happy
#    define snprintf_trunc(dst, size, ...) \
        do {                               \
            volatile size_t n = size;      \
            snprintf(dst, n, __VA_ARGS__); \
        } while (0)

void oled_write_layer_state(void) {
    oled_write_P(PSTR(" "), false);

    switch (get_highest_layer(layer_state | default_layer_state)) {
        case 0:
            oled_write_P(PSTR("Base "), false);
            break;
        case 1:
            oled_write_P(PSTR("Symbl"), false);
            break;
        case 2:
            oled_write_P(PSTR("Utils"), false);
            break;
        case 3:
            oled_write_P(PSTR("Nav  "), false);
            break;
        case 4:
            oled_write_P(PSTR("Sys  "), false);
            break;
        default:
            oled_write_P(PSTR("Undef"), false);
            break;
    }
    oled_write_P(PSTR("/"), false);
    if (meko_get_scroll_mode()) {
        oled_write_P(PSTR("S"), false);
    } else {
        oled_write_P(PSTR("C"), false);
    }

    char cpi[5];
    char scroll_div[3];
    char angle[4];

    snprintf_trunc(cpi, 5, "%4d", cpi_array[meko_config.cpi_idx]);
    snprintf_trunc(scroll_div, 3, "%2d", scrl_div_array[meko_config.scrl_div]);
    snprintf_trunc(angle, 4, "%3d", angle_array[meko_config.rotation_angle]);
    oled_write_P(PSTR("/"), false);
    oled_write(cpi, false);
    oled_write_P(PSTR("/"), false);
    oled_write(scroll_div, false);
    oled_write_P(PSTR("/"), false);
    oled_write(angle, false);
}

#endif
