#include "matrix.h"
//#include "atomic_util.h"
//#include <string.h>
#include "config.h"
#include "print.h"
#include "wait.h"


void matrix_init_custom(void) {
    // 74HC595 (row output) pins
    setPinOutput(ROW_DATA_PIN);
    setPinOutput(ROW_CLOCK_PIN);
    setPinOutput(ROW_LATCH_PIN);
    writePinLow(ROW_CLOCK_PIN);
    writePinLow(ROW_LATCH_PIN);

    // 74HC165 (column input) pins
    setPinInput(COL_DATA_PIN);
    setPinOutput(COL_CLK_PIN);
    setPinOutput(COL_SH_PIN);
    writePinLow(COL_CLK_PIN);
    writePinHigh(COL_SH_PIN);  // Load is active low

    // Independent pins
    setPinOutput(SW_A_PIN);
    setPinOutput(SW_B_PIN);
    writePinLow(SW_A_PIN);
    writePinLow(SW_B_PIN);
}

// Activate row pin
static void shift_out_rows(uint8_t data) {
    // Shift out data all rows - round down to multiples of 8
    for (int i = 0; i <= (MATRIX_ROWS & ~7); i++) {
        writePinLow(ROW_CLOCK_PIN);

        if (data == i) {
            writePinHigh(ROW_DATA_PIN);
        } else {
            writePinLow(ROW_DATA_PIN);
        }

        writePinHigh(ROW_CLOCK_PIN);
    }

    // Latch the data to outputs
    writePinLow(ROW_LATCH_PIN);
    writePinHigh(ROW_LATCH_PIN);
}

// Zero all row pins
static void unselect_rows(void) {
    writePinLow(SW_A_PIN);
    writePinLow(SW_B_PIN);

    // Shift out data all rows
    for (int i = 0; i <= (MATRIX_ROWS & ~7); i++) {
        writePinLow(ROW_CLOCK_PIN);
        writePinLow(ROW_DATA_PIN);
        writePinHigh(ROW_CLOCK_PIN);
    }

    // Latch the data to outputs
    writePinLow(ROW_LATCH_PIN);
    writePinHigh(ROW_LATCH_PIN);
}

bool matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    /* Start with a clear matrix row */
    matrix_row_t current_row_value = 0;

    // Select row
    // Rows 0-23 are via shift registers
    // Rows 24-25 are pins on MCU
    if (current_row <= 23) {
        writePinLow(SW_A_PIN);
        writePinLow(SW_B_PIN);
        shift_out_rows(current_row);
    }
    else if (current_row == 24) {
        unselect_rows();
        writePinHigh(SW_A_PIN);
    }
    else if (current_row == 25) {
        unselect_rows();
        writePinHigh(SW_B_PIN);
    }

    // Trigger parallel load of column data (active low pulse)
    writePinLow(COL_SH_PIN);
    wait_us(1);
    writePinHigh(COL_SH_PIN);
    wait_us(1);

    // Unselect row
    unselect_rows();

    // Read column data for this row
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index ++) {

        // Pulse clock
        writePinHigh(COL_CLK_PIN);
        wait_us(1);
        writePinLow(COL_CLK_PIN);
        wait_us(1);

        //uprintf("%d", (readPin(COL_DATA_PIN) ? 1 : 0));

        current_row_value |= (readPin(COL_DATA_PIN) ? 1 : 0) << (MATRIX_COLS - col_index);
    }
    //uprintf(")\n");

    // Update row in matrix
    if (current_row_value != current_matrix[current_row]) {
        current_matrix[current_row] = current_row_value;
        return true;
    }
    return false;
}


bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    //wait_ms(100);

    /* set row, read cols */
    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {
        matrix_row_t new_row_data = read_row(current_row);
        if (current_matrix[row] != new_row_data) {
            current_matrix[row] = new_row_data;
            changed = true;
        }

        //changed |= matrix_read_cols_on_row(curr_matrix, current_row);
    }

    return changed;
}

/*
static void shift_out_rows(uint8_t data) {
    for (int i = 7; i >= 0; i--) {
        writePinLow(ROW_CLOCK_PIN);

        if (data & (1 << i)) {
            writePinHigh(ROW_DATA_PIN);
        } else {
            writePinLow(ROW_DATA_PIN);
        }

        writePinHigh(ROW_CLOCK_PIN);
    }

    // Latch the data to outputs
    writePinLow(ROW_LATCH_PIN);
    writePinHigh(ROW_LATCH_PIN);
}


// Shift in data from 74HC165 (columns)
static uint8_t shift_in_cols(void) {
    uint8_t data = 0;

    // Trigger parallel load (active low pulse)
    writePinLow(COL_SH_PIN);
    wait_us(1);
    writePinHigh(COL_SH_PIN);
    wait_us(1);

    // Shift in 8 bits
    for (int i = 0; i < 8; i++) {
        data |= (readPin(COL_DATA_PIN) ? 1 : 0) << (7 - i);

        writePinHigh(COL_CLK_PIN);
        wait_us(1);
        writePinLow(COL_CLK_PIN);
        wait_us(1);
    }

    return data;
}


bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    matrix_row_t curr_matrix[MATRIX_ROWS] = {0};

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        // Select current row (set bit high for active row)
        shift_out_rows(1 << row);

        // Small delay for signal stabilization
        wait_us(10);

        // Read column states
        curr_matrix[row] = shift_in_cols();

        // Print what we're reading
        uprintf("Row %d: 0x%02X (binary: ", row, curr_matrix[row]);
        for (int i = 7; i >= 0; i--) {
            uprintf("%d", (curr_matrix[row] >> i) & 1);
        }
        uprintf(")\n");
    }

    // Deselect all rows
    shift_out_rows(0x00);

    // Check if matrix changed and update if needed
    bool changed = memcmp(current_matrix, curr_matrix, sizeof(curr_matrix)) != 0;
    if (changed) {
        memcpy(current_matrix, curr_matrix, sizeof(curr_matrix));
        uprintf("Matrix changed!\n");
    }
    print("Matrix scanned\n");
    wait_ms(250);
    return changed;
}
*/


/*
Matrix rows being driven by 74HC595 (parallel-out shift register)
Column rows being read by 74HC165 (parallel-in shift register)
*/
/*
#ifdef MATRIX_ROW_PINS
static pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
#endif // MATRIX_ROW_PINS
#ifdef MATRIX_COL_PINS
static pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
#endif // MATRIX_COL_PINS


static inline void gpio_atomic_set_pin_output_low(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        gpio_set_pin_output(pin);
        gpio_write_pin_low(pin);
    }
}

static inline void gpio_atomic_set_pin_output_high(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        gpio_set_pin_output(pin);
        gpio_write_pin_high(pin);
    }
}

static inline void gpio_atomic_set_pin_input_high(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        gpio_set_pin_input_high(pin);
    }
}

static inline uint8_t readMatrixPin(pin_t pin) {
    if (pin != NO_PIN) {
        return gpio_read_pin(pin);
    } else {
        return 1;
    }
}

void small_delay(volatile uint8_t timeout) {
    while (timeout--);
}

static void shiftOut(uint16_t dataOut) {
    ATOMIC_BLOCK_FORCEON {
        for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
            if (dataOut & 0x1) {
                gpio_write_pin_high(DATA_PIN_74HC595);
            } else {
                gpio_write_pin_low(DATA_PIN_74HC595);
            }
            dataOut = dataOut >> 1;
            gpio_write_pin_high(CLOCK_PIN_74HC595);
            small_delay(2);
            gpio_write_pin_low(CLOCK_PIN_74HC595);
        }
        gpio_write_pin_high(LATCH_PIN_74HC595);
        small_delay(2);
        gpio_write_pin_low(LATCH_PIN_74HC595);
    }
}

static void shiftOut_single(uint8_t data) {
    ATOMIC_BLOCK_FORCEON {
        if (data & 0x1) {
            gpio_write_pin_high(DATA_PIN_74HC595);
        } else {
            gpio_write_pin_low(DATA_PIN_74HC595);
        }
        gpio_write_pin_high(CLOCK_PIN_74HC595);
        small_delay(2);
        gpio_write_pin_low(CLOCK_PIN_74HC595);
        gpio_write_pin_high(LATCH_PIN_74HC595);
        small_delay(2);
        gpio_write_pin_low(LATCH_PIN_74HC595);
    }
}

static bool select_col(uint8_t col) {
    pin_t pin = col_pins[col];

    if (pin != NO_PIN) {
        gpio_atomic_set_pin_output_low(pin);
        return true;
    } else {
        if (col == PIN_START_74HC595) {
            shiftOut_single(0x00);
        }
        return true;
    }
    return false;
}

static void unselect_col(uint8_t col) {
    pin_t pin = col_pins[col];

    if (pin != NO_PIN) {
#ifdef MATRIX_UNSELECT_DRIVE_HIGH
        gpio_atomic_set_pin_output_high(pin);
#else
        gpio_atomic_set_pin_input_high(pin);
#endif
    } else {
        shiftOut_single(0x01);
    }
}

static void unselect_cols(void) {
    // unselect column pins
    for (uint8_t x = 0; x < MATRIX_COLS; x++) {
        pin_t pin = col_pins[x];

        if (pin != NO_PIN) {
#ifdef MATRIX_UNSELECT_DRIVE_HIGH
            gpio_atomic_set_pin_output_high(pin);
#else
            gpio_atomic_set_pin_input_high(pin);
#endif
        }
        if (x == PIN_START_74HC595)
            // unselect Shift Register
            shiftOut(0xFFFF);
    }
}

static void matrix_init_pins(void) {
    gpio_set_pin_output(DATA_PIN_74HC595);
    gpio_set_pin_output(CLOCK_PIN_74HC595);
    gpio_set_pin_output(LATCH_PIN_74HC595);
#ifdef MATRIX_UNSELECT_DRIVE_HIGH
    for (uint8_t x = 0; x < MATRIX_COLS; x++) {
        if (col_pins[x] != NO_PIN) {
            gpio_set_pin_output(col_pins[x]);
        }
    }
#endif
    unselect_cols();
    for (uint8_t x = 0; x < MATRIX_ROWS; x++) {
        if (row_pins[x] != NO_PIN) {
            gpio_atomic_set_pin_input_high(row_pins[x]);
        }
    }
}

static void matrix_read_rows_on_col(matrix_row_t current_matrix[], uint8_t current_col, matrix_row_t row_shifter) {
    bool key_pressed = false;

    // Select col
    if (!select_col(current_col)) { // select col
        return;                     // skip NO_PIN col
    }

    matrix_output_select_delay();

    // For each row...
    for (uint8_t row_index = 0; row_index < ROWS_PER_HAND; row_index++) {
        // Check row pin state
        if (readMatrixPin(row_pins[row_index]) == 0) {
            // Pin LO, set col bit
            current_matrix[row_index] |= row_shifter;
            key_pressed = true;
        } else {
            // Pin HI, clear col bit
            current_matrix[row_index] &= ~row_shifter;
        }
    }

    // Unselect col
    unselect_col(current_col);
    matrix_output_unselect_delay(current_col, key_pressed); // wait for all Row signals to go HIGH
}

void matrix_init_custom(void) {
    // initialize key pins
    matrix_init_pins();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    matrix_row_t curr_matrix[MATRIX_ROWS] = {0};

    // Set col, read rows
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t current_col = 0; current_col < MATRIX_COLS; current_col++, row_shifter <<= 1) {
        matrix_read_rows_on_col(curr_matrix, current_col, row_shifter);
    }

    bool changed = memcmp(current_matrix, curr_matrix, sizeof(curr_matrix)) != 0;
    if (changed) memcpy(current_matrix, curr_matrix, sizeof(curr_matrix));

    return changed;
}
*/
