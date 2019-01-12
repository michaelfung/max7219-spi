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
 * `cs_index` parameter (see `spi.cs*_gpio`). The device will be polled
 * for validity, upon success a new `struct mgos_max7219` is allocated and
 * returned. If the device could not be found, NULL is returned.
 */
struct mgos_max7219 *mgos_max7219_create(struct mgos_spi *spi, uint8_t cs_index);

/*
 * Destroy the data structure associated with a MAX7219 device. The reference
 * to the pointer of the `struct mgos_max7219` has to be provided, and upon
 * successful destruction, its associated memory will be freed and the pointer
 * set to NULL and true will be returned.
 */
bool mgos_max7219_destroy(struct mgos_max7219 **dev);

/*
 * Get/Set display intensity (brightness) as a value from 0..15.
 * Returns true on success or false on failure.
 */
bool mgos_max7219_get_intensity(struct mgos_max7219 *dev, uint8_t *intensity);
bool mgos_max7219_set_intensity(struct mgos_max7219 *dev, uint8_t intensity);

/*
 * Get/Set display digit at position `digit` 0..7, with a `value` from 0..255.
 * Returns true on success or false on failure.
 */
bool mgos_max7219_set_raw(struct mgos_max7219 *dev, uint8_t digit, uint8_t value);
bool mgos_max7219_get_raw(struct mgos_max7219 *dev, uint8_t digit, uint8_t *value);

/*
 * Get/Set display digit at position `digit` 0..7, with an actual 7-segment
 * display digit, with values from 0-9, 10='-', 11='E', 12='H', 13='L',
 * 14='P', 15=' ', called Code B decoding.
 * Returns true on success or false on failure.
 */
bool mgos_max7219_set_digit(struct mgos_max7219 *dev, uint8_t digit, uint8_t value);
bool mgos_max7219_get_digit(struct mgos_max7219 *dev, uint8_t digit, uint8_t *value);

#ifdef __cplusplus
}
#endif
