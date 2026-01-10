#ifndef SCD40_H
#define SCD40_H

#include <stdint.h>

typedef struct {
  uint16_t co2;
  float temperature;
  float humidity;
} scd40Data_t;

void scd40_initSensor();

#endif /* SCD40_H */