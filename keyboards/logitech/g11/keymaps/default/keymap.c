// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

// Hardware:
// 24 rows (3 x 73HC595 parallel-out shift registers)
// 32 columns (4 x 74HC165 parallel-in shift registers)
// Probably can't continue with QMK until the matrix has been fully mapped out
// TODO: Write program in arduino to scan matrix and output codes, load that into keyboard.json.

#include QMK_KEYBOARD_H
#include "action.h"

#define HW_SW_LAYER     GP13
#define HW_IND_M1       GP14
#define HW_IND_M2       GP15
#define HW_IND_M3       GP26
#define HW_IND_MR       GP27

void backlight_increase(void);
void backlight_decrease(void);


enum layers{
  _LAYER0
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_LAYER0] = LAYOUT(
		BL_UP, BL_DOWN
	/*),
	[_LAYER1] = LAYOUT(
		BL_TOGG, BL_STEP
	),
	[_LAYER2] = LAYOUT(
		KC_MPRV, KC_MPLY*/
	)
};


bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { // First encoder
        if (clockwise) {
            //tap_code(KC_0 + ((BL_UP >> 12) & 0xF));
            backlight_increase();
        } else {
            //tap_code(KC_0 + ((BL_DOWN >> 12) & 0xF));
            backlight_decrease();
        }
    }
    return false;
}


/*
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
*/
