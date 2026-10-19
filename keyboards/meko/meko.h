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

#ifndef _MEKO_H_
#define _MEKO_H_

#include "quantum.h"

typedef union {
    uint32_t raw;
    struct {
        uint8_t        cpi_idx;
        uint8_t        scrl_div;
        uint8_t        rotation_angle;
        int8_t         scrl_inv;
        bool           scrl_mode;  // Not saved to EEPROM
        report_mouse_t last_mouse; // Not saved to EEPROM
    };
} meko_config_t;

extern meko_config_t meko_config;

bool meko_get_scroll_mode(void);
void meko_set_scroll_mode(bool mode);

void render_logo(void);
void oled_write_layer_state(void);

#endif // _MEKO_H_