// Written by Nick Gammon
// May 2012

#include <Arduino.h>
#include <Wire.h>

template <typename T> uint8_t I2C_writeAnything (const T& value)
  {
    const byte * p = (const byte*) &value;
    uint8_t i;
    for (i = 0; i < sizeof value; i++)
          Wire.write(*p++);
    return i;
  }  // end of I2C_writeAnything

template <typename T> uint8_t I2C_readAnything(T& value)
  {
    byte * p = (byte*) &value;
    uint8_t i;
    for (i = 0; i < sizeof value; i++)
          *p++ = Wire.read();
    return i;
  }  // end of I2C_readAnything


