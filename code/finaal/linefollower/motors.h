// =============================================================================
// motors.h - DRV8833 aansturing
// =============================================================================
// DRV8833 waarheidstabel per H-brug (TI datasheet, tabel "H-Bridge Logic"):
//   xIN1 xIN2 | xOUT1 xOUT2 | functie
//    0    0   |   Z     Z   | coast (vrijloop)
//    0    1   |   L     H   | achteruit
//    1    0   |   H     L   | vooruit
//    1    1   |   L     L   | rem (slow decay)
// PWM-snelheidsregeling (TI datasheet, tabel "PWM Control of Motor Speed"):
//   vooruit, fast decay : IN1 = PWM,          IN2 = 0
//   vooruit, slow decay : IN1 = 1,            IN2 = PWM (geinverteerd)
//   achteruit, fast     : IN1 = 0,            IN2 = PWM
//   achteruit, slow     : IN1 = PWM (inv.),   IN2 = 1
// Kanaal B heeft maar 1 PWM-pin (IN3 = D3, IN4 = D2): de richting "IN4 hoog"
// wordt daarom met slow decay geregeld, de richting "IN4 laag" met fast decay.
// =============================================================================
#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

void motorsBegin();
void motorsSet(int16_t left, int16_t right);   // -255..255, + = vooruit (na swap/invert)
void motorsStop();                             // beide motoren remmen
void motorsCoast();                            // beide motoren vrijloop
void motorChannelRaw(uint8_t channel, int16_t speed);  // 0 = kanaal A, 1 = kanaal B (voor tests)

#endif
