#pragma once

/* Hardware pin definitions (RP2040 Zero)
 *
 * (Row outputs driven via 3 x 74HC595D)
 * GPIO0: Row serial data out
 * GPIO1: Row clock
 * GPIO2: Row store
 *
 * (Column pins scanned via 4 x 74HC165)
 * GPIO3: Column serial data in
 * GPIO4: Column clock
 * GPIO5: Column SH (?)
 *
 * GPIO6: Rotary encoder A pin
 * GPIO7: Rotary encoder B pin
 *
 * GPIO8: Num lock LED
 * GPIO9: Caps lock LED
 * GPIO10: Scroll lock LED
 *
 * GPIO11: Media control buttons column A
 * GPIO12: Media control buttons column B
 * GPIO13: Normal/game slider switch
 *
 * GPIO14: M1 indicator
 * GPIO15: M2 indicator
 * GPIO26: M3 indicator
 * GPIO27: MR indicator
 *
 * GPIO28: Backlight PWM output (to transistor driver)
 * GPIO29: Backlight RGB output (to solder pad)
 */


// This needs to match the size of the matrix_pins array in keyboard.json or it will complain.
// With a custom matrix scan, we don't need to declare pin numbers, we can just use NO_PIN and let the matrix code return the correct struct.
// See keyboards\keychron\v6\matrix.c example using 74HC595.
// See keyboards\handwired\dqz11n1g\matrix.c example using 74HC165.
//#define MATRIX_ROWS 24
//#define MATRIX_COLS 32

#define DYNAMIC_KEYMAP_MACRO_COUNT 64

//#define BACKLIGHT_PIN       GP12

//#define ENCODER_DEFAULT_POS 0x3
