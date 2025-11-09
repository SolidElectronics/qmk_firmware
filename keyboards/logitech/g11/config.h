#pragma once

/* Hardware pin definitions (RP2040 Zero)
 *
 * (Row outputs driven via 3 x 74HC595D)
 * GPIO5: Row serial data out
 * GPIO3: Row clock
 * GPIO4: Row store
 *
 * (Column pins scanned via 4 x 74HC165)
 * GPIO6: Column serial data in
 * GPIO7: Column clock
 * GPIO8: Column SH (?)
 *
 * GPIO10: Rotary encoder A pin
 * GPIO9: Rotary encoder B pin
 *
 * GPIO2: Num lock LED
 * GPIO1: Caps lock LED
 * GPIO0: Scroll lock LED
 *
 * GPIO11: Media control buttons column A
 * GPIO12: Media control buttons column B
 * GPIO27: Normal/game slider switch
 *
 * GPIO26: M1 indicator
 * GPIO15: M2 indicator
 * GPIO14: M3 indicator
 * GPIO13: MR indicator
 *
 * GPIO28: Backlight PWM output (to transistor driver)
 * GPIO29: Backlight RGB output (to solder pad)
 */


// This needs to match the size of the matrix_pins array in keyboard.json or it will complain.
// With a custom matrix scan, we don't need to declare pin numbers, we can just use NO_PIN and let the matrix code return the correct struct.
// See keyboards\keychron\v6\matrix.c example using 74HC595.
// See keyboards\handwired\dqz11n1g\matrix.c example using 74HC165.
#define MATRIX_ROWS     26
#define MATRIX_COLS     32

#define ROW_DATA_PIN    GP5
#define ROW_CLOCK_PIN   GP3
#define ROW_LATCH_PIN   GP4

#define COL_DATA_PIN    GP6
#define COL_CLK_PIN     GP7
#define COL_SH_PIN      GP8

#define SW_A_PIN        GP11
#define SW_B_PIN        GP12

//#define BACKLIGHT_PIN   GP28

#define DEBUG_ENABLE    true
#define DEBUG_MATRIX    true


#define DYNAMIC_KEYMAP_MACRO_COUNT 64

//#define ENCODER_DEFAULT_POS 0x3
