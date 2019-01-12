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

#include "mgos_max7219_internal.h"

static bool mgos_max7219_read_reg_b(struct mgos_max7219 *dev, uint8_t reg, uint8_t *val) {
  uint8_t tx_data = 0x80 | reg;
  uint8_t rx_data;

  if (!dev || !val) {
    return false;
  }

  struct mgos_spi_txn txn = {
    .cs   = dev->cs_index,
    .mode = dev->spi_mode,
    .freq = dev->spi_freq,
  };
  txn.hd.tx_len    = 1;
  txn.hd.tx_data   = &tx_data;
  txn.hd.dummy_len = 0;
  txn.hd.rx_len    = 1;
  txn.hd.rx_data   = &rx_data;
  if (!mgos_spi_run_txn(dev->spi, false, &txn)) {
    return false;
  }
  *val = rx_data;
  return true;
}

static bool mgos_max7219_write_reg_b(struct mgos_max7219 *dev, uint8_t reg, uint8_t val) {
  uint8_t tx_data[2] = { reg, val };

  if (!dev) {
    return false;
  }

  struct mgos_spi_txn txn = {
    .cs   = dev->cs_index,
    .mode = dev->spi_mode,
    .freq = dev->spi_freq,
  };
  txn.hd.tx_data   = tx_data;
  txn.hd.tx_len    = sizeof(tx_data);
  txn.hd.dummy_len = 0;
  txn.hd.rx_len    = 0;
  return mgos_spi_run_txn(dev->spi, false, &txn);
}

static bool mgos_max7219_reset(struct mgos_max7219 *dev) {
  if (!mgos_max7219_write_reg_b(dev, MGOS_MAX7219_REG_SCANLIMIT, 7)) {
    return false;
  }
  if (!mgos_max7219_write_reg_b(dev, MGOS_MAX7219_REG_DECODEMODE, dev->codeB_enabled)) {
    return false;
  }
  if (!mgos_max7219_write_reg_b(dev, MGOS_MAX7219_REG_DISPLAYTEST, 0)) {
    return false;
  }
  if (!mgos_max7219_set_intensity(dev, 5)) {
    return false;
  }
  if (!mgos_max7219_write_reg_b(dev, MGOS_MAX7219_REG_SHUTDOWN, 1)) {
    return false;
  }

  return true;
}

struct mgos_max7219 *mgos_max7219_create(struct mgos_spi *spi, uint8_t cs_index) {
  struct mgos_max7219 *dev = NULL;

  if (!spi) {
    return NULL;
  }

  dev = calloc(1, sizeof(struct mgos_max7219));
  if (!dev) {
    return NULL;
  }

  memset(dev, 0, sizeof(struct mgos_max7219));
  dev->spi      = spi;
  dev->cs_index = cs_index;
  dev->spi_mode = 0;
  dev->spi_freq = 1e6;

  if (!mgos_max7219_reset(dev)) {
    LOG(LL_INFO, ("Could not reset MAX7219 at SPI cs=%u freq=%u mode=%u", dev->cs_index, dev->spi_freq, dev->spi_mode));
    free(dev);
    return NULL;
  }
  LOG(LL_INFO, ("MAX7219 initialized at SPI cs=%u freq=%u mode=%u", dev->cs_index, dev->spi_freq, dev->spi_mode));
  return dev;
}

bool mgos_max7219_set_intensity(struct mgos_max7219 *dev, uint8_t intensity) {
  if (!dev || intensity > 15) {
    return false;
  }

  return mgos_max7219_write_reg_b(dev, MGOS_MAX7219_REG_INTENSITY, intensity & 0x0f);
}

bool mgos_max7219_get_intensity(struct mgos_max7219 *dev, uint8_t *intensity) {
  if (!dev || !intensity) {
    return false;
  }

  return mgos_max7219_read_reg_b(dev, MGOS_MAX7219_REG_INTENSITY, intensity);
}

bool mgos_max7219_set_raw(struct mgos_max7219 *dev, uint8_t digit, uint8_t value) {
  if (!dev || digit > 7) {
    return false;
  }

  // We are in codeB for this digit, set to raw (by clearing the bit)
  if (dev->codeB_enabled & (1 << digit)) {
    dev->codeB_enabled &= ~(1 << digit);
    if (!mgos_max7219_write_reg_b(dev, MGOS_MAX7219_REG_DECODEMODE, dev->codeB_enabled)) {
      return false;
    }
  }
  return mgos_max7219_write_reg_b(dev, MGOS_MAX7219_REG_DIGIT0 + digit, value);
}

bool mgos_max7219_get_raw(struct mgos_max7219 *dev, uint8_t digit, uint8_t *value) {
  if (!dev || digit > 7 || !value) {
    return false;
  }

  // Do not return raw for digits in codeB
  if (dev->codeB_enabled & (1 << digit)) {
    return false;
  }

  return mgos_max7219_read_reg_b(dev, MGOS_MAX7219_REG_DIGIT0 + digit, value);
}

bool mgos_max7219_set_digit(struct mgos_max7219 *dev, uint8_t digit, uint8_t value) {
  if (!dev || digit > 7 || value > 15) {
    return false;
  }

  // We are not in codeB for this digit, set it.
  if (!(dev->codeB_enabled & (1 << digit))) {
    dev->codeB_enabled |= 1 << digit;
    if (!mgos_max7219_write_reg_b(dev, MGOS_MAX7219_REG_DECODEMODE, dev->codeB_enabled)) {
      return false;
    }
  }
  return mgos_max7219_write_reg_b(dev, MGOS_MAX7219_REG_DIGIT0 + digit, value);
}

bool mgos_max7219_get_digit(struct mgos_max7219 *dev, uint8_t digit, uint8_t *value) {
  if (!dev || digit > 7 || !value) {
    return false;
  }

  // Do not return raw for digits in raw mode
  if (!(dev->codeB_enabled & (1 << digit))) {
    return false;
  }

  return mgos_max7219_read_reg_b(dev, MGOS_MAX7219_REG_DIGIT0 + digit, value);
}

bool mgos_max7219_destroy(struct mgos_max7219 **dev) {
  if (!*dev) {
    return false;
  }
  mgos_max7219_reset(*dev);
  free(*dev);
  *dev = NULL;
  return true;
}

// Mongoose OS library initialization
bool mgos_max7219_spi_init(void) {
  return true;
}
