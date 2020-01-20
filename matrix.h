#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"

#if (MATRIX_COLS <= 8)
typedef  uint8_t    matrix_row_t;
#elif (MATRIX_COLS <= 16)
typedef  uint16_t   matrix_row_t;
#elif (MATRIX_COLS <= 32)
typedef  uint32_t   matrix_row_t;
#else
#error "MATRIX_COLS: invalid value"
#endif

#define MATRIX_IS_ON(row, col)  (matrix_get_row(row) && (1<<col))

#ifdef __cplusplus
extern "C" {
#endif

	/* number of matrix rows */
	uint8_t matrix_rows(void);
	/* number of matrix columns */
	uint8_t matrix_cols(void);
	/* should be called at early stage of startup before matrix_init.(optional) */
	void matrix_setup(void);
	/* intialize matrix for scaning. */
	void matrix_init(void);
	/* scan all key states on matrix */
	uint8_t matrix_scan(void);
	/* whether modified from previous scan. used after matrix_scan. */
	bool matrix_is_modified(void) __attribute__ ((deprecated));
	/* whether a switch is on */
	bool matrix_is_on(uint8_t row, uint8_t col);
	/* matrix state on row */
	matrix_row_t matrix_get_row(uint8_t row);
	/* print matrix for debug */
	void matrix_print(void);

	/* power control */
	void matrix_power_up(void);
	void matrix_power_down(void);

	/* executes code for Quantum */
	void matrix_init_quantum(void);
	void matrix_scan_quantum(void);

	void matrix_init_kb(void);
	void matrix_scan_kb(void);

	void matrix_init_user(void);
	void matrix_scan_user(void);

#ifdef __cplusplus
}
#endif

#endif
