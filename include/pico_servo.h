#ifndef PICO_SERVO_H
#define PICO_SERVO_H

#include "pico/stdlib.h"

int servo_init();
int servo_attach(uint);
int servo_enable();
int servo_move_to(uint, uint);

#endif /* PICO_SERVO_H */