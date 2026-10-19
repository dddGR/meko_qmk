
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

// Defines names for use in layer keycodes and the keymap
// enum layer_number { _BASE, _LOWER, _RAISE, _TRACKBALL };
enum layer_number { _BASE };

#define LW_MHEN LT(1, KC_INT5) // lower
#define RS_HENK LT(2, KC_INT4) // raise
#define DEL_ALT ALT_T(KC_DEL)

// clang-format off
// In the buttom row, KC_MS_BTN3 binds middle-click to the encoder's button press
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_TAB,     KC_Q,       KC_W,       KC_E,       KC_R,       KC_T,       KC_MUTE,
        KC_ESC,     KC_A,       KC_S,       KC_D,       KC_F,       KC_G,
        KC_LCTL,    KC_Z,       KC_X,       KC_C,       KC_V,       KC_B,       MS_BTN1,
                                            KC_LALT,    KC_LSFT,    KC_SPC,     KC_LCMD,

        KC_MPLY,    KC_Y,       KC_U,       KC_I,       KC_O,       KC_P,       KC_BSPC,
                    KC_H,       KC_J,       KC_K,       KC_L,       KC_SCLN,    KC_ENT,
        MS_BTN2,    KC_N,       KC_M,       KC_COMM,    KC_DOT,     KC_SLSH,    KC_RCTL,
        KC_RCMD,    KC_SPC,     KC_RSFT,    KC_RALT
    )
};
// clang-format on

// Same function on all layers for now.
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(KC_PGUP, KC_PGDN)},
    [1] = {ENCODER_CCW_CW(KC_PGUP, KC_PGDN)},
    [2] = {ENCODER_CCW_CW(KC_PGUP, KC_PGDN)},
    [3] = {ENCODER_CCW_CW(KC_PGUP, KC_PGDN)},
};
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    // switch (get_highest_layer(state)) {
    //     case _LOWER:
    //         meko_set_scroll_mode(true);
    //         break;
    //     case _RAISE:
    //         meko_set_scroll_mode(true);
    //         break;
    //     case _TRACKBALL:
    //         meko_set_scroll_mode(false);
    //         break;
    //     default:
    //         meko_set_scroll_mode(false);
    //         break;
    // }
    return state;
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    switch (index) {
        case 0 ... 2: {
            tap_code(clockwise ? MS_WHLD : MS_WHLU);
        } break;
        case 3: {
            tap_code_delay(clockwise ? KC_VOLU : KC_VOLD, 10);
        } break;
        case 4: {
            tap_code_delay(clockwise ? KC_MNXT : KC_MPRV, 10);
        } break;
        default: {
            /* do-some-thing-else */
        } break;
    }

    // FOR TESTING
    // tap_code(KC_MINUS);
    // switch (index) {
    //     case 0: {
    //         tap_code(KC_0);
    //         if (clockwise) {
    //             tap_code(KC_D);
    //         } else {
    //             tap_code(KC_U);
    //         }
    //     } break;
    //     case 1: {
    //         tap_code(KC_1);
    //         if (clockwise) {
    //             tap_code(KC_D);
    //         } else {
    //             tap_code(KC_U);
    //         }
    //     } break;
    //     case 2: {
    //         tap_code(KC_2);
    //         if (clockwise) {
    //             tap_code(KC_D);
    //         } else {
    //             tap_code(KC_U);
    //         }
    //     } break;
    //     default: {
    //         /* nothing */
    //     } break;
    // }

    return false;
}
