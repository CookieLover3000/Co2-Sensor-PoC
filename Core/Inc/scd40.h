#ifndef SCD40_H
#define SCD40_H

#include <stdint.h>

// This tells C++ compilers: "These functions use C-style naming"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint16_t co2;
  float temperature;
  float humidity;
} scd40Data_t;

void scd40_initSensor();

#ifdef __cplusplus
}
#endif

#endif /* SCD40_H */