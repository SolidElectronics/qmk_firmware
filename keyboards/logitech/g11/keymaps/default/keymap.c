// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

// Hardware:
// 24 rows (3 x 73HC595 parallel-out shift registers)
// 32 columns (4 x 74HC165 parallel-in shift registers)
// Probably can't continue with QMK until the matrix has been fully mapped out
// TODO: Write program in arduino to scan matrix and output codes, load that into keyboard.json.

#include QMK_KEYBOARD_H
//#include "action.h"

#define HW_SW_LAYER     GP27
#define HW_IND_M1       GP26
#define HW_IND_M2       GP15
#define HW_IND_M3       GP14
#define HW_IND_MR       GP13

enum layers{
  _LAYER0
  //_LAYER1,
  //_LAYER2
};

// This is a forward declaration.  These functions exist, but aren't declared when this compiles.  This lets the compiler ignore that fact and fix it during linking.
void backlight_increase(void);
void backlight_decrease(void);

// Initialize (early)
void keyboard_pre_init_user(void) {
    setPinOutput(HW_IND_M1);
    setPinOutput(HW_IND_M2);
    setPinOutput(HW_IND_M3);
    setPinOutput(HW_IND_MR);
    setPinInputHigh(HW_SW_LAYER);
}

// Initialize (late)
void keyboard_post_init_user(void) {
  debug_enable=true;
  debug_matrix=true;
  debug_keyboard=true;
  debug_mouse=true;
  tap_code16(QK_BACKLIGHT_ON);
}

// Layer change handler
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case 0:
            writePinHigh(HW_IND_M1);
            writePinLow(HW_IND_M2);
            writePinLow(HW_IND_M3);
            writePinLow(HW_IND_MR);
            break;
        case 1:
            writePinLow(HW_IND_M1);
            writePinHigh(HW_IND_M2);
            writePinLow(HW_IND_M3);
            writePinLow(HW_IND_MR);
            break;
        case 2:
            writePinLow(HW_IND_M1);
            writePinLow(HW_IND_M2);
            writePinHigh(HW_IND_M3);
            writePinLow(HW_IND_MR);
            break;
        case 3:
            writePinLow(HW_IND_M1);
            writePinLow(HW_IND_M2);
            writePinLow(HW_IND_M3);
            writePinHigh(HW_IND_MR);
            break;
        default:
            writePinLow(HW_IND_M1);
            writePinLow(HW_IND_M2);
            writePinLow(HW_IND_M3);
            writePinLow(HW_IND_MR);
            break;
    }
    return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // If console is enabled, it will print the matrix position and status of each key pressed
#ifdef CONSOLE_ENABLE
    uprintf("KL: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
#endif
  return true;
}

// Scan routine
void matrix_scan_user(void) {
    static bool last_layer_switch = false;
    bool layer_switch = !readPin(HW_SW_LAYER);

    if (layer_switch != last_layer_switch) {
        if (layer_switch) {
            layer_on(1);
        } else {
            layer_off(1);
        }
        last_layer_switch = layer_switch;
    }
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    switch (index) {
        // Encoder number
        case 0:
            switch (get_highest_layer(layer_state)) {
                case _LAYER0:
                    (clockwise) ? tap_code(KC_VOLU) : tap_code(KC_VOLD);
                    break;
//                case _LAYER1:
//                    // This only works if you directly call the backlight functions.  tap_code16(BL_UP or BL_DOWN) doesn't work.
//                    (clockwise) ? backlight_increase() : backlight_decrease();
//                    break;
            }
            break;

    }
    return false;
}


// Declare macros as custom keycodes, otherwise there is a limit of 32 QK_MACRO_## codes.
enum custom_keycodes {
    KC_CUSTOM_START = SAFE_RANGE,
    LAYER_SW,
    G11_M19,
    G11_M20,
    G11_M21,
    G11_M22,
    G11_M23,
    G11_M24,
    G11_M25,
    G11_M26,
    G11_M27,
    G11_M28,
    G11_M29,
    G11_M30,
    G11_M31,
    G11_M32,
    G11_M33,
    G11_M34,
    G11_M35,
    G11_M36,
    G11_M37,
    G11_M38,
    G11_M39,
    G11_M40,
    G11_M41,
    G11_M42,
    G11_M43,
    G11_M44,
    G11_M45,
    G11_M46,
    G11_M47,
    G11_M48,
    G11_M49,
    G11_M50,
    G11_M51,
    G11_M52,
    G11_M53,
    G11_M54
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * [M1] [M2] [M3] [MR]                        [Back|Fwd|Stop|Play]      [Mute] [Backlight]
     * ┌───┬───┬───┐
     * |G1 |G2 |G3 | ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┐
     * ├───┼───┼───┤ │Esc│   │F1 │F2 │F3 │F4 │ │F5 │F6 │F7 │F8 │ │F9 │F10│F11│F12│ │PSc│Scr│Pse│
     * |G4 |G5 |G6 | └───┘   └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┘
     * └───┴───┴───┘ ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐ ┌───┬───┬───┐ ┌───┬───┬───┬───┐
     * ┌───┬───┬───┐ │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Backsp│ │Ins│Hom│PgU│ │Num│ / │ * │ - │
     * |G7 |G8 |G9 | ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤ ├───┼───┼───┤ ├───┼───┼───┼───┤
     * ├───┼───┼───┤ │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │  \  │ │Del│End│PgD│ │ 7 │ 8 │ 9 │   │
     * |G10|G11|G12| ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤ └───┴───┴───┘ ├───┼───┼───┤ + │
     * └───┴───┴───┘ │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │  Enter │               │ 4 │ 5 │ 6 │   │
     * ┌───┬───┬───┐ ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────────┤     ┌───┐     ├───┼───┼───┼───┤
     * |G13|G14|G15| │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │    Shift │     │ ↑ │     │ 1 │ 2 │ 3 │   │
     * ├───┼───┼───┤ ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤ ┌───┼───┼───┐ ├───┴───┼───┤Ent│
     * |G16|G17|G18| │Ctrl│GUI │Alt │                        │ Alt│ GUI│Menu│Ctrl│ │ ← │ ↓ │ → │ │   0   │ . │   │
     * └───┴───┴───┘ └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘ └───┴───┴───┘ └───────┴───┴───┘
     * ANSI 104 + 18 G-keys + 10 other functions = 132 keys
     */

/*
// Full-size definitions
    [_LAYER0] = LAYOUT_ansi_104(
        _______,     _______,     _______,      _______,                                                       KC_MNXT, KC_MPRV, KC_MSTP, KC_MPLY,                   KC_MUTE,    BL_STEP,
        QK_MACRO_1,  QK_MACRO_2,  QK_MACRO_3,   KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,     KC_PSCR, KC_SCRL, KC_PAUS,
        QK_MACRO_4,  QK_MACRO_5,  QK_MACRO_6,   KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,    KC_INS,  KC_HOME, KC_PGUP,    KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        QK_MACRO_7,  QK_MACRO_8,  QK_MACRO_9,   KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,    KC_DEL,  KC_END,  KC_PGDN,    KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        QK_MACRO_10, QK_MACRO_11, QK_MACRO_12,  KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,                                   KC_P4,   KC_P5,   KC_P6,
        QK_MACRO_13, QK_MACRO_14, QK_MACRO_15,  KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,             KC_UP,               KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        QK_MACRO_16, QK_MACRO_17, QK_MACRO_18,  KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, KC_RGUI, KC_APP,  KC_RCTL,    KC_LEFT, KC_DOWN, KC_RGHT,    KC_P0,            KC_PDOT
    ),
    [_LAYER1] = LAYOUT_ansi_104(
        _______,     _______,     _______,      _______,                                                       KC_MNXT, KC_MPRV, KC_MSTP, KC_MPLY,                   KC_MUTE,    BL_STEP,
        G11_M19,     G11_M20,     G11_M21,      KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,     KC_PSCR, KC_SCRL, KC_PAUS,
        G11_M22,     G11_M23,     G11_M24,      KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,    KC_INS,  KC_HOME, KC_PGUP,    KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        G11_M25,     G11_M26,     G11_M27,      KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,    KC_DEL,  KC_END,  KC_PGDN,    KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        G11_M28,     G11_M29,     G11_M30,      KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,                                   KC_P4,   KC_P5,   KC_P6,
        G11_M31,     G11_M32,     G11_M33,      KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,             KC_UP,               KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        G11_M34,     G11_M35,     G11_M36,      KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, KC_RGUI, KC_APP,  KC_RCTL,    KC_LEFT, KC_DOWN, KC_RGHT,    KC_P0,            KC_PDOT
    ),
    [_LAYER2] = LAYOUT_ansi_104(
        _______,     _______,     _______,      _______,                                                       KC_MNXT, KC_MPRV, KC_MSTP, KC_MPLY,                   KC_MUTE,    BL_STEP,
        G11_M37,     G11_M38,     G11_M39,      KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,     KC_PSCR, KC_SCRL, KC_PAUS,
        G11_M40,     G11_M41,     G11_M42,      KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,    KC_INS,  KC_HOME, KC_PGUP,    KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        G11_M43,     G11_M44,     G11_M45,      KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,    KC_DEL,  KC_END,  KC_PGDN,    KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        G11_M46,     G11_M47,     G11_M48,      KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,                                   KC_P4,   KC_P5,   KC_P6,
        G11_M49,     G11_M50,     G11_M51,      KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,             KC_UP,               KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        G11_M52,     G11_M53,     G11_M54,      KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, KC_RGUI, KC_APP,  KC_RCTL,    KC_LEFT, KC_DOWN, KC_RGHT,    KC_P0,            KC_PDOT
    )
};
*/
/*
// Test definitions
    [_LAYER0] = LAYOUT(
		BL_UP, BL_DOWN
	),
	[_LAYER1] = LAYOUT(
		BL_TOGG, BL_STEP
	),
	[_LAYER2] = LAYOUT(
		KC_MPRV, KC_MPLY
	)
};
*/
/*
    [_LAYER0] = LAYOUT(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,
        KC_ESC,  KC_TAB,  KC_LSFT, KC_LCTL, KC_LALT, KC_SPC,  KC_ENT,  KC_BSPC,
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,
        KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_HOME, KC_END,  KC_PGUP, KC_PGDN,
        KC_P0,   KC_P1,   KC_P2,   KC_P3,   KC_P4,   KC_P5,   KC_P6,   KC_P7
    )
*/
    [_LAYER0] = LAYOUT(
        KC_A,       KC_B,   KC_C,   KC_D,   KC_MUTE,    BL_TOGG,
        QK_BOOT,    KC_1,   KC_2
    )
};
