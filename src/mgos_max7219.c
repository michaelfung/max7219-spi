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

// Set a reg/val pair in all connected devices.
static bool mgos_max7219_write_all(struct mgos_max7219 *dev, uint8_t reg, uint8_t val) {
  uint8_t *tx_data;
  uint8_t  i;
  bool     ret;

  if (!dev) {
    return false;
  }

  if (!(tx_data = malloc(2 * dev->num_devices))) {
    return false;
  }
  for (i = 0; i < dev->num_devices; i++) {
    tx_data[i * 2]     = reg;
    tx_data[i * 2 + 1] = val;
  }

  struct mgos_spi_txn txn = {
    .cs   = dev->cs_index,
    .mode = dev->spi_mode,
    .freq = dev->spi_freq,
  };
  txn.hd.tx_data   = tx_data;
  txn.hd.tx_len    = 2 * dev->num_devices;
  txn.hd.dummy_len = 0;
  txn.hd.rx_len    = 0;

  ret = mgos_spi_run_txn(dev->spi, false, &txn);
  free(tx_data);
  return ret;
}

// Set a reg/val pair in the device identified by deviceno (0 is the first device)
static bool mgos_max7219_write_one(struct mgos_max7219 *dev, uint8_t deviceno, uint8_t reg, uint8_t val) {
  uint8_t *tx_data;
  uint8_t  i;
  bool     ret;

  if (!dev) {
    return false;
  }
  if (!(tx_data = malloc(2 * dev->num_devices))) {
    return false;
  }

  // Send NOOP to all devices except the deviceno we're interested in
  for (i = 0; i < dev->num_devices; i++) {
    if (deviceno == i) {
      tx_data[(dev->num_devices - i - 1) * 2]     = reg;
      tx_data[(dev->num_devices - i - 1) * 2 + 1] = val;
    } else {
      tx_data[(dev->num_devices - i - 1) * 2]     = MGOS_MAX7219_REG_NOOP;
      tx_data[(dev->num_devices - i - 1) * 2 + 1] = 0x00;
    }
  }

  struct mgos_spi_txn txn = {
    .cs   = dev->cs_index,
    .mode = dev->spi_mode,
    .freq = dev->spi_freq,
  };
  txn.hd.tx_data   = tx_data;
  txn.hd.tx_len    = 2 * dev->num_devices;
  txn.hd.dummy_len = 0;
  txn.hd.rx_len    = 0;
  ret = mgos_spi_run_txn(dev->spi, false, &txn);
  free(tx_data);
  return ret;
}

// Set a reg with N values (one for each device) -- The length of `val` must be `dev->num_devices`
static bool mgos_max7219_write(struct mgos_max7219 *dev, uint8_t reg, uint8_t *val) {
  uint8_t *tx_data;
  uint8_t  i;
  bool     ret;

  if (!dev) {
    return false;
  }
  if (!(tx_data = malloc(2 * dev->num_devices))) {
    return false;
  }

  // Send NOOP to all devices before the deviceno we're interested in
  for (i = 0; i < dev->num_devices; i++) {
    tx_data[i * 2]     = reg;
    tx_data[i * 2 + 1] = val[dev->num_devices - i - 1];
  }

  struct mgos_spi_txn txn = {
    .cs   = dev->cs_index,
    .mode = dev->spi_mode,
    .freq = dev->spi_freq,
  };
  txn.hd.tx_data   = tx_data;
  txn.hd.tx_len    = 2 * dev->num_devices;
  txn.hd.dummy_len = 0;
  txn.hd.rx_len    = 0;
  ret = mgos_spi_run_txn(dev->spi, false, &txn);
  free(tx_data);
  return ret;
}

bool mgos_max7219_set_mode(struct mgos_max7219 *dev, bool codeB_enabled) {
  dev->codeB_enabled = codeB_enabled;
  return mgos_max7219_write_all(dev, MGOS_MAX7219_REG_DECODEMODE, codeB_enabled);
}

static bool mgos_max7219_reset(struct mgos_max7219 *dev) {
  if (!mgos_max7219_write_all(dev, MGOS_MAX7219_REG_SCANLIMIT, 7)) {
    return false;
  }
  if (!mgos_max7219_write_all(dev, MGOS_MAX7219_REG_DISPLAYTEST, 0)) {
    return false;
  }
  if (!mgos_max7219_set_intensity(dev, 5)) {
    return false;
  }
  if (!mgos_max7219_write_all(dev, MGOS_MAX7219_REG_SHUTDOWN, 1)) {
    return false;
  }

  return true;
}

struct mgos_max7219 *mgos_max7219_create(struct mgos_spi *spi, uint8_t cs_index, uint8_t num_devices) {
  struct mgos_max7219 *dev = NULL;

  if (!spi) {
    return NULL;
  }

  dev = calloc(1, sizeof(struct mgos_max7219));
  if (!dev) {
    return NULL;
  }

  memset(dev, 0, sizeof(struct mgos_max7219));
  dev->spi         = spi;
  dev->cs_index    = cs_index;
  dev->spi_mode    = 0;
  dev->spi_freq    = 1e6;
  dev->num_devices = num_devices;

  if (!mgos_max7219_reset(dev)) {
    LOG(LL_INFO, ("Could not reset MAX7219 at SPI cs=%u freq=%u mode=%u", dev->cs_index, dev->spi_freq, dev->spi_mode));
    free(dev);
    return NULL;
  }
  LOG(LL_INFO, ("%u MAX7219 devices initialized at SPI cs=%u freq=%u mode=%u", dev->num_devices, dev->cs_index, dev->spi_freq, dev->spi_mode));
  return dev;
}

bool mgos_max7219_set_num_devices(struct mgos_max7219 *dev, uint8_t num_devices) {
  if (!dev || num_devices > MGOS_MAX7219_MAX_DEVICES) {
    return false;
  }
  dev->num_devices = num_devices;
  return true;
}

bool mgos_max7219_set_intensity(struct mgos_max7219 *dev, uint8_t intensity) {
  if (!dev || intensity > 15) {
    return false;
  }

  return mgos_max7219_write_all(dev, MGOS_MAX7219_REG_INTENSITY, intensity & 0x0f);
}

bool mgos_max7219_write_raw(struct mgos_max7219 *dev, uint8_t deviceno, uint8_t digit, uint8_t value) {
  if (!dev || deviceno >= dev->num_devices || digit >= 8 || dev->codeB_enabled) {
    return false;
  }

  return mgos_max7219_write_one(dev, deviceno, MGOS_MAX7219_REG_DIGIT0 + digit, value);
}

bool mgos_max7219_write_digit(struct mgos_max7219 *dev, uint8_t deviceno, uint8_t digit, uint8_t value) {
  if (!dev || digit >= 8 || value >= 16 || deviceno >= dev->num_devices || !dev->codeB_enabled) {
    return false;
  }

  return mgos_max7219_write_one(dev, deviceno, MGOS_MAX7219_REG_DIGIT0 + digit, value);
}

bool mgos_max7219_write_line(struct mgos_max7219 *dev, uint8_t digit, uint8_t *value) {
  if (!dev || digit >= 8 || !value) {
    return false;
  }
  return mgos_max7219_write(dev, MGOS_MAX7219_REG_DIGIT0 + digit, value);
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
