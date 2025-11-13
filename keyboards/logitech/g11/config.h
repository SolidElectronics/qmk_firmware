#pragma once

/* Hardware pin definitions (RP2040 Zero)
 *
 * (24 row outputs driven via 3 x 74HC595D)
 * GPIO5: Row serial data out
 * GPIO3: Row clock
 * GPIO4: Row store
 *
 * GPIO11: Media control buttons row A
 * GPIO12: Media control buttons row B

 * (32 column pins scanned via 4 x 74HC165)
 * GPIO6: Column serial data in
 * GPIO7: Column clock
 * GPIO8: Column SH (?)
 *
 * GPIO10: Rotary encoder A pin
 * GPIO9: Rotary encoder B pin
 * GPIO27: Normal/game slider switch
 *
 * GPIO2: Num lock LED
 * GPIO1: Caps lock LED
 * GPIO0: Scroll lock LED
 *
 * GPIO26: M1 indicator
 * GPIO15: M2 indicator
 * GPIO14: M3 indicator
 * GPIO13: MR indicator
 *
 * GPIO28: Backlight PWM output (to transistor driver)
 * GPIO29: Backlight RGB output (to solder pad)
 */


// This needs to match the size of the matrix_pins array in keyboard.json.
// With a custom matrix scan, we don't need to declare pin numbers, we can just use NO_PIN and let the matrix code return the correct struct.
#define MATRIX_ROWS     26
#define MATRIX_COLS     32

#define WEAR_LEVELING_BACKING_SIZE 65536  // Max amount of memory available as flash on RP2040
#define DYNAMIC_MACRO_NO_NESTING
#define DISABLE_LEADER

// Matrix pins are defined here
#define ROW_DATA_PIN    GP5
#define ROW_CLOCK_PIN   GP3
#define ROW_LATCH_PIN   GP4

#define COL_DATA_PIN    GP6
#define COL_CLK_PIN     GP7
#define COL_SH_PIN      GP8

#define SW_A_PIN        GP11
#define SW_B_PIN        GP12

#define HW_SW_GAME      GP27
#define HW_IND_M1       GP26
#define HW_IND_M2       GP15
#define HW_IND_M3       GP14
#define HW_IND_MR       GP13

#define DEBUG_ENABLE    false
#define DEBUG_MATRIX    false

#define DYNAMIC_KEYMAP_LAYER_COUNT 3
#define DYNAMIC_KEYMAP_MACRO_COUNT 64
#define TAP_CODE_DELAY 10
#define MOUSEKEY_MOVE_DELTA 2

// Top-left M1 key for BootMagic
#define BOOTMAGIC_ROW 1
#define BOOTMAGIC_COLUMN 13

//#define ENCODER_DEFAULT_POS 0x3
