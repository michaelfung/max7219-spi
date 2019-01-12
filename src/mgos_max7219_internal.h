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
#include "mgos_max7219.h"

#ifdef __cplusplus
extern "C" {
#endif

// Registers
#define MGOS_MAX7219_REG_NOOP           (0x0)
#define MGOS_MAX7219_REG_DIGIT0         (0x1)
#define MGOS_MAX7219_REG_DIGIT1         (0x2)
#define MGOS_MAX7219_REG_DIGIT2         (0x3)
#define MGOS_MAX7219_REG_DIGIT3         (0x4)
#define MGOS_MAX7219_REG_DIGIT4         (0x5)
#define MGOS_MAX7219_REG_DIGIT5         (0x6)
#define MGOS_MAX7219_REG_DIGIT6         (0x7)
#define MGOS_MAX7219_REG_DIGIT7         (0x8)
#define MGOS_MAX7219_REG_DECODEMODE     (0x9)
#define MGOS_MAX7219_REG_INTENSITY      (0xA)
#define MGOS_MAX7219_REG_SCANLIMIT      (0xB)
#define MGOS_MAX7219_REG_SHUTDOWN       (0xC)
#define MGOS_MAX7219_REG_DISPLAYTEST    (0xF)

struct mgos_max7219 {
  struct mgos_spi *spi;
  uint8_t          cs_index;
  uint8_t          spi_mode;
  uint32_t         spi_freq;
  uint8_t          codeB_enabled; // Set to 0 for raw, and 1 for codeB decoding
};

/* Mongoose OS initializer */
bool mgos_max7219_spi_init(void);

#ifdef __cplusplus
}
#endif
