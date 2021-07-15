#include "platform.h"

// TODO i2c hardware
void i2c_start(uint_fast8_t bus) {}
void i2c_stop(uint_fast8_t bus) {}
uint_fast8_t i2c_read(uint_fast8_t bus) {}
uint_fast8_t i2c_write(uint_fast8_t bus, uint_fast8_t val) {}
int i2c_claim_bus(uint_fast8_t bus) {}
void i2c_release_bus(uint_fast8_t bus) {}
// TODO i2c bitbang
void i2c_set(uint_fast8_t bus, uint_fast8_t bits) {}
void i2c_clear(uint_fast8_t bus, uint_fast8_t bits) {}
uint_fast8_t i2c_sda(uint_fast8_t bus) {}
uint_fast8_t i2c_scl(uint_fast8_t bus) {}
