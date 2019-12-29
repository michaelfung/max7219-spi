/*
 * Copyright 2019 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "mgos.h"
#include "mgos_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mgos_max7219;

/*
 * Initialize a MAX7219 on the SPI bus `spi` using the chipselect specified in
 * `cs_index` parameter (see `spi.cs*_gpio`). Upon success a new
 * `struct mgos_max7219` is  allocated and returned.
 * If the device could not be found or initialized, NULL is returned.
 */
struct mgos_max7219 *mgos_max7219_create(struct mgos_spi *spi, uint8_t cs_index, uint8_t num_devices);

/*
 * Destroy the data structure associated with a MAX7219 device. The reference
 * to the pointer of the `struct mgos_max7219` has to be provided, and upon
 * successful destruction, its associated memory will be freed and the pointer
 * set to NULL and true will be returned.
 */
bool mgos_max7219_destroy(struct mgos_max7219 **dev);

/*
 * Set the number of devices in the daisychain (1 or more).
 * Returns true on success or false on failure.
 */
bool mgos_max7219_set_num_devices(struct mgos_max7219 *dev, uint8_t num_devices);

/*
 * Set display intensity (brightness) as a value from 0..15.
 * Returns true on success or false on failure.
 */
bool mgos_max7219_set_intensity(struct mgos_max7219 *dev, uint8_t intensity);


/* Set display mode, either 'raw' or 'digit':
 * - codeB_enabled: set 'false' for 'raw', or 'true' to enable 7-segment digits.
 * Returns true on success or false on failure.
 */
bool mgos_max7219_set_mode(struct mgos_max7219 *dev, bool codeB_enabled);

/*
 * Set display digit at position `digit` with a `value`
 * - deviceno: device to adddress, between 0 and num_devices-1;
 * - digit: must be between [0, 7]
 * - value: can be between [0, 255]
 * Returns true on success or false on failure.
 */
bool mgos_max7219_write_raw(struct mgos_max7219 *dev, uint8_t deviceno, uint8_t digit, uint8_t value);

/*
 * Set display digit at position `digit` with an 7-segment character `value`
 * - deviceno: device to adddress, between 0 and num_devices-1;
 * - digit: must be between [0, 7]
 * - value: must be between [0, 15]:
 *          0-9 for numbers, , 10='-', 11='E', 12='H', 13='L', 14='P', 15=' '
 *          called Code B decoding.
 * Returns true on success or false on failure.
 */
bool mgos_max7219_write_digit(struct mgos_max7219 *dev, uint8_t deviceno, uint8_t digit, uint8_t value);

/*
 * Write a line of values at position `digit` across all connected devices.
 * - digit: must be between [0, 7]
 * - value: An array of exactly num_devices bytes.
 *
 * Note: If the device is in mode codeB, the values should be between [0, 15].
 *
 * Returns true on success or false on failure.
 */
bool mgos_max7219_write_line(struct mgos_max7219 *dev, uint8_t digit, uint8_t *value);


#ifdef __cplusplus
}
#endif
