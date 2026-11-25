
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

#include "keymap_introspection.h"

// Defines names for use in layer keycodes and the keymap
// enum layer_number { _BASE, _LOWER, _RAISE, _TRACKBALL };

#define LW_MHEN LT(1, KC_INT5) // lower
#define RS_HENK LT(2, KC_INT4) // raise
#define DEL_ALT ALT_T(KC_DEL)

// Alias
#define SUPER KC_LCMD

/* ----------------------------------------------------- */
/* KEY OVERRIDE ---------------------------------------- */
// Right Ctrl + C -> Ctrl + Insert
const key_override_t copy_override = ko_make_basic(MOD_BIT_RCTRL, KC_C, C(KC_INS));
// Right Ctrl + V -> Shift + Insert
const key_override_t paste_override = ko_make_basic(MOD_BIT_RCTRL, KC_V, S(KC_INS));
// Right Ctrl + X -> Shift + Delete
const key_override_t cut_override = ko_make_basic(MOD_BIT_RCTRL, KC_X, S(KC_DEL));
// Right Ctrl + D -> Delete
const key_override_t ctrld_override = ko_make_basic(MOD_BIT_RCTRL, KC_D, KC_DEL);

// This globally defines all key overrides to be used
const key_override_t *key_overrides[] = {&copy_override, &paste_override, &cut_override, &ctrld_override};

/* ----------------------------------------------------- */
/* COMBO ----------------------------------------------- */
// Home + PageUp -> Ctrl + W (use in Nav layer)
const uint16_t PROGMEM close_tab_cbo[] = {KC_HOME, KC_PAGE_UP, COMBO_END};

combo_t key_combos[] = {
    COMBO(close_tab_cbo, C(KC_W)),
};
/* ----------------------------------------------------- */
/* TAP DANCE ------------------------------------------- */
enum {
    TD_MRMM, // tap: Right Mouse - double tap: Middle Mouse
    TD_CZSZ, // tap: Ctrl + Z (undo) - double tap: Ctrl + Shift + Z (redo)
    TD_SCCL, // tap: Semicolon - double tap: Colon
    TD_QTDQ, // tap: Quote - double tap: Double Quote
    TD_COLB, // tap: Comma - double tap: Left Angle Bracket
    TD_DORB, // tap: Dot - double tap: Right Angle Bracket
    TD_SLQS, // tap: Slash - double tap: Question Mark
};

// clang-format off
tap_dance_action_t tap_dance_actions[] = {
    [TD_MRMM] = ACTION_TAP_DANCE_DOUBLE(QK_MOUSE_BUTTON_2, QK_MOUSE_BUTTON_3),
    [TD_CZSZ] = ACTION_TAP_DANCE_DOUBLE(C(KC_Z), LCS(KC_Z)),
    [TD_SCCL] = ACTION_TAP_DANCE_DOUBLE(KC_SEMICOLON, KC_COLN),
    [TD_QTDQ] = ACTION_TAP_DANCE_DOUBLE(KC_QUOTE, KC_DQUO),
    [TD_COLB] = ACTION_TAP_DANCE_DOUBLE(KC_COMMA, KC_LABK),
    [TD_DORB] = ACTION_TAP_DANCE_DOUBLE(KC_DOT, KC_RABK),
    [TD_SLQS] = ACTION_TAP_DANCE_DOUBLE(KC_SLASH, KC_QUES),
};
// clang-format on

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_TAB,         KC_Q,       KC_W,       KC_E,       KC_R,       KC_T,       KC_MUTE,
        RCTL_T(KC_ESC), KC_A,       KC_S,       KC_D,       KC_F,       KC_G,
        KC_LCTL,        KC_Z,       KC_X,       KC_C,       KC_V,       KC_B,       MS_BTN1,
                                                SUPER,      KC_LALT,    LT(3, KC_SPC),RSFT_T(KC_SPC),

        KC_MPLY,        KC_Y,       KC_U,       KC_I,       KC_O,       KC_P,       KC_BSPC,
                        KC_H,       KC_J,       KC_K,       KC_L,       TD(TD_SCCL),TD(TD_QTDQ),
        TD(TD_MRMM),    KC_N,       KC_M,       TD(TD_COLB),TD(TD_DORB),TD(TD_SLQS),LSFT_T(KC_ENT),
        LT(2, KC_ENT),  LT(1, KC_SPC),SUPER,    ALT_T(KC_DEL)
    ),
    [1] = LAYOUT( // SYMBOL
        KC_GRV,     KC_TILD,    XXXXXXX,    KC_EXLM,    XXXXXXX,    KC_MINUS,   XXXXXXX,
        XXXXXXX,    KC_AT,      KC_ASTR,    KC_DLR,     XXXXXXX,    KC_UNDS,
        _______,    XXXXXXX,    XXXXXXX,    KC_PLUS,    KC_AMPR,    KC_EQL,     XXXXXXX,
                                            _______,    _______,    MO(4),    _______,

        XXXXXXX,    KC_LCBR,    KC_LBRC,    KC_RBRC,    KC_RCBR,    KC_PERC,    _______,
                    KC_HASH,    KC_LPRN,    KC_RPRN,    KC_PIPE,    KC_COLN,    KC_DQUO,
        XXXXXXX,    KC_EXLM,    KC_CIRC,    KC_LABK,    KC_RABK,    KC_QUES,    KC_BSLS,
        KC_ENT,     _______,    XXXXXXX,    QK_LLCK
    ),
    [2] = LAYOUT( // UTILS
        KC_F11,     KC_F1,      KC_F2,      KC_F3,      KC_F4,      KC_F5,      XXXXXXX,
        KC_F12,     KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,
        _______,    TD(TD_CZSZ),S(KC_DEL),  C(KC_INS),  S(KC_INS),  A(KC_LEFT), XXXXXXX,
                                            _______,    _______,    XXXXXXX,    KC_INS,

        XXXXXXX,    KC_1,       KC_2,       KC_3,       KC_4,       KC_5,       _______,
                    KC_6,       KC_7,       KC_8,       KC_9,       KC_0,       KC_DEL,
        XXXXXXX,    A(KC_RIGHT),XXXXXXX,    KC_COMM,    KC_DOT,     XXXXXXX,    XXXXXXX,
        _______,    XXXXXXX,    XXXXXXX,    QK_LLCK
    ),
    [3] = LAYOUT( // NAVIGATION
        C(KC_TAB),  G(KC_1),    G(KC_2),    G(KC_3),    G(KC_4),    G(KC_5),    _______,
        A(KC_TAB),  G(KC_6),    G(KC_7),    G(KC_8),    G(KC_9),    G(KC_0),
        _______,    KC_LSFT,    S(KC_DEL),  C(KC_INS),  S(KC_INS),  A(KC_LEFT), _______,
                                            QK_LLCK,    _______,    _______,    _______,

        _______,    C(KC_LEFT), C(KC_PGUP), C(KC_PGDN), C(KC_RIGHT),KC_HOME,    KC_PGUP,
                    KC_LEFT,    KC_DOWN,    KC_UP,      KC_RGHT,    KC_END,     KC_PGDN,
        _______,    A(KC_RIGHT),XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    KC_RCTL,
        KC_ENT,     MO(4),      SUPER,      C(KC_W)
    ),
    [4] = LAYOUT( // SYSTEM
        KC_F11,     KC_F1,      KC_F2,      KC_F3,      KC_F4,      KC_F5,      XXXXXXX,
        KC_F12,     KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,
        XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,
                                            XXXXXXX,    XXXXXXX,    _______,    XXXXXXX,

        XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    KC_PRINT_SCREEN,KC_DEL,
                    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,
        XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,
        XXXXXXX,    _______,    XXXXXXX,    QK_BOOT
    ),
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

/* ----------------------------------------------------- */
/* AUTO SHIFT ------------------------------------------ */
/* Usage: tap to send key, hold a bit longer to send different key.
   Override defaut auto shift keys, no other keys will auto shifted
   except those defined here.
    - Backspace: hold will Ctrl+BS (delete whole words)
    - Mouse 1: hold will lock click (use for drag or select)
*/
bool get_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_BACKSPACE:
        case MS_BTN1:
            return true;
        default:
            return get_custom_auto_shifted_key(keycode, record);
    }
}

#define REG_KEY(key, key_shifted)                         \
    case key:                                             \
        register_code16(shifted ? (key_shifted) : (key)); \
        break

#define UNREG_KEY(key, key_shifted)                         \
    case key:                                               \
        unregister_code16(shifted ? (key_shifted) : (key)); \
        break

void autoshift_press_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    switch (keycode) {
        REG_KEY(KC_BSPC, C(KC_BSPC));
        REG_KEY(MS_BTN1, MS_BTN1);
        default:
            if (shifted) add_weak_mods(MOD_BIT(KC_LSFT));
            // & 0xFF gets the Tap key for Tap Holds, required when using Retro Shift
            register_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
    }
}

void autoshift_release_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    switch (keycode) {
        UNREG_KEY(KC_BSPC, C(KC_BSPC));
        UNREG_KEY(MS_BTN1, KC_NO); // tap: 1 click, hold abit longer to lock mouse click
        default:
            // & 0xFF gets the Tap key for Tap Holds, required when using Retro Shift
            // The IS_RETRO check isn't really necessary here, always using
            // keycode & 0xFF would be fine.
            unregister_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
    }
}
