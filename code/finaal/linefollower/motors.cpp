#include "motors.h"
#include "config.h"
#include "settings.h"

struct Channel
{
  uint8_t in1, in2;
  bool in1Pwm, in2Pwm;
};

static const Channel CH[2] = {
  { PIN_A_IN1, PIN_A_IN2, PIN_A_IN1_PWM, PIN_A_IN2_PWM },   // kanaal A: OUT1/OUT2
  { PIN_B_IN3, PIN_B_IN4, PIN_B_IN3_PWM, PIN_B_IN4_PWM },   // kanaal B: OUT3/OUT4
};

// Zet een pin op een PWM-duty (0..255). Op een niet-PWM-pin wordt NOOIT
// analogWrite() gebruikt; daar zijn enkel 0 en 255 toegelaten.
static void writePin(uint8_t pin, bool pwmCapable, uint8_t duty)
{
  if (pwmCapable) analogWrite(pin, duty);          // analogWrite(0/255) = digitaal laag/hoog
  else digitalWrite(pin, duty >= 128 ? HIGH : LOW);
}

void motorChannelRaw(uint8_t channel, int16_t speed)
{
  const Channel &c = CH[channel];
  speed = constrain(speed, -255, 255);
  uint8_t s = (uint8_t)abs(speed);

  if (speed == 0)
  {
    writePin(c.in1, c.in1Pwm, 255);                // rem: beide hoog
    writePin(c.in2, c.in2Pwm, 255);
    return;
  }

  bool forward = speed > 0;
  // "aandrijvende" pin = IN1 bij vooruit, IN2 bij achteruit
  uint8_t drive    = forward ? c.in1 : c.in2;
  uint8_t other    = forward ? c.in2 : c.in1;
  bool drivePwm    = forward ? c.in1Pwm : c.in2Pwm;
  bool otherPwm    = forward ? c.in2Pwm : c.in1Pwm;

  bool slowDecay = otherPwm && (!params.fastDecay || !drivePwm);
  if (slowDecay)
  {
    // slow decay: aandrijvende pin hoog, andere pin geinverteerde PWM.
    // Heeft een lineairder verband PWM <-> toerental dan fast decay.
    writePin(drive, drivePwm, 255);
    writePin(other, true, 255 - s);
  }
  else
  {
    // fast decay: andere pin laag, PWM op de aandrijvende pin
    writePin(other, otherPwm, 0);
    writePin(drive, drivePwm, s);   // (zonder PWM-pin: enkel aan/uit)
  }
}

void motorsBegin()
{
  for (uint8_t i = 0; i < 2; i++)
  {
    pinMode(CH[i].in1, OUTPUT);
    pinMode(CH[i].in2, OUTPUT);
  }

  // Timer2 (D3/D11) staat standaard op phase-correct PWM (490 Hz) terwijl
  // Timer0 (D5/D6) op fast PWM (976 Hz) staat. Beide motoren op dezelfde
  // frequentie zetten zodat ze zich gelijk gedragen: Timer2 -> fast PWM.
  // Timer0 NIET aanpassen (millis()/micros() hangen ervan af).
  TCCR2A |= _BV(WGM21);
#if (PIN_B_IN4 == 10) || (PIN_B_IN4 == 9) || (PIN_A_IN1 == 9) || (PIN_A_IN1 == 10) || (PIN_A_IN2 == 9) || (PIN_A_IN2 == 10)
  TCCR1B |= _BV(WGM12);                           // Timer1 (D9/D10) ook naar 8-bit fast PWM
#endif
  motorsStop();
}

void motorsSet(int16_t left, int16_t right)
{
  left  = constrain(left,  -params.maxPwm, params.maxPwm);
  right = constrain(right, -params.maxPwm, params.maxPwm);

  // links/rechts-balans: + = linkermotor relatief sneller
  if (params.balance > 0) right = (int32_t)right * (100 - params.balance) / 100;
  else if (params.balance < 0) left = (int32_t)left * (100 + params.balance) / 100;

  if (params.invLeft)  left  = -left;
  if (params.invRight) right = -right;

  if (params.swapMotors) { motorChannelRaw(1, left); motorChannelRaw(0, right); }
  else                   { motorChannelRaw(0, left); motorChannelRaw(1, right); }
}

void motorsStop()
{
  motorChannelRaw(0, 0);
  motorChannelRaw(1, 0);
}

void motorsCoast()
{
  for (uint8_t i = 0; i < 2; i++)
  {
    writePin(CH[i].in1, CH[i].in1Pwm, 0);
    writePin(CH[i].in2, CH[i].in2Pwm, 0);
  }
}
