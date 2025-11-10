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


// Write 24 bits to the 74HC595 shift registers to select a row
static void select_row(uint8_t row) {
    // Rows 24-25 are on invidual MCU pins.  For those, we can still do the shift register to zero it out.
    if (row == 24) {
        writePinHigh(SW_A_PIN);
        writePinLow(SW_B_PIN);
    } else if (row == 25) {
        writePinLow(SW_A_PIN);
        writePinHigh(SW_B_PIN);
    } else {
        writePinLow(SW_A_PIN);
        writePinLow(SW_B_PIN);
    }

    // We'll send 24 bits (3 bytes) with only one bit HIGH
    // The bit position corresponds to the row number

    writePinLow(ROW_LATCH_PIN);  // Prepare to shift data

    // Send 24 bits, MSB first
    for (int8_t i = 23; i >= 0; i--) {
        // Set data pin HIGH only for the selected row
        if (i == row) {
            writePinHigh(ROW_DATA_PIN);
        } else {
            writePinLow(ROW_DATA_PIN);
        }

        // Clock the bit in
        writePinHigh(ROW_CLOCK_PIN);
        wait_us(1);
        writePinLow(ROW_CLOCK_PIN);
        wait_us(1);
    }

    // Latch the data to the output pins
    writePinHigh(ROW_LATCH_PIN);
    wait_us(1);
    writePinLow(ROW_LATCH_PIN);
    wait_us(5);  // Give the row line time to stabilize
}


// Read 32 bits from the 74HC165 shift registers (column states)
static matrix_row_t read_cols(void) {
    matrix_row_t cols = 0;

    // Trigger parallel load - capture current state of all inputs
    writePinLow(COL_SH_PIN);
    wait_us(1);
    writePinHigh(COL_SH_PIN);
    wait_us(1);

    // Read 32 bits serially (4 shift registers × 8 bits)
    for (int i = 0; i < 32; i++) {
        // Read the current bit and shift it into position
        // Reading MSB first (bit 31 down to bit 0)
        cols |= (readPin(COL_DATA_PIN) ? 1UL : 0UL) << (31 - i);

        // Clock to shift next bit
        writePinHigh(COL_CLK_PIN);
        wait_us(1);
        writePinLow(COL_CLK_PIN);
        wait_us(1);
    }

    return cols;
}


bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    /* set row, read cols */
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        select_row(row);
        wait_us(10);
        matrix_row_t cols = read_cols();

        if (current_matrix[row] != cols) {
            current_matrix[row] = cols;
            changed = true;
        }

        //changed |= matrix_read_cols_on_row(curr_matrix, current_row);
    }

    // Release all row pins
    select_row(0xFF);

    return changed;
}
