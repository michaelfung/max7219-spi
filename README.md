# MAX7219/MAX7221 SPI Driver

A Mongoose library for MAX7219, an 8-digit LED Display Driver.

## Implementation details

The chip is straight forwardly implemented, and does snot allow for daisy
chaining at the moment. After initialization, bytes can be written to
digits `0-7`, each of which have 8 LEDs -- either in a matrix (8x8) or as
8x 7-segment displays and a dot.

## API Description

Create the object with `mgos_max7219_create()` after which writes to a certain
digit (ie block of 8 LEDs) can be performed with `mgos_max7219_set_raw()`, while
digis can be read back with `mgos_max7219_get_raw()`.

Intensity van be set with `mgos_max7219_set_intensity()`, where the argument
is between 0 (very dim) to 15 (very bright). `mgos_max7219_get_intensity()`
returns the current setting.

## Example application

# Disclaimer

This project is not an official Google project. It is not supported by Google
and Google specifically disclaims all warranties as to its quality,
merchantability, or fitness for a particular purpose.
