#include "controller.h"
#include "settings.h"
#include "sensors.h"
#include "motors.h"

ControlDebug ctrl;

static float integral;        // [mm*s]
static float prevError;       // [mm]
static float dFiltered;       // [mm/s]
static float lastLineError;   // laatste fout waarbij de lijn gezien werd
static uint32_t lostSince;    // [ms]

void controllerReset()
{
  integral = 0;
  prevError = 0;
  dFiltered = 0;
  lastLineError = 0;
  lostSince = 0;
  ctrl.state = ST_FOLLOW;
  ctrl.error = ctrl.p = ctrl.i = ctrl.d = ctrl.u = 0;
  ctrl.left = ctrl.right = 0;
}

const __FlashStringHelper *lineStateName(LineState s)
{
  switch (s)
  {
    case ST_FOLLOW:  return F("volgen");
    case ST_GAP:     return F("onderbreking");
    case ST_RECOVER: return F("herstel");
    default:         return F("verloren");
  }
}

bool controllerStep(float dt)
{
  // ---------------------------------------------------------- 1. lijnfout
  float error = lastLineError;
  float speedFactor = 1.0f;

  if (sensors.type == LINE_NORMAL)
  {
    error = sensors.position;
    lastLineError = error;
    ctrl.state = ST_FOLLOW;
  }
  else if (sensors.type == LINE_CROSSING)
  {
    // Kruispunt: positie is onbetrouwbaar (alles zwart) -> koers aanhouden.
    error = lastLineError;
    ctrl.state = ST_FOLLOW;
  }
  else // LINE_NONE
  {
    uint32_t now = millis();
    if (ctrl.state == ST_FOLLOW)
    {
      lostSince = now;
      // Lag de lijn nog ongeveer in het midden? Dan is het een onderbreking
      // (< 10 mm volgens de opdracht) -> rechtdoor. Anders is de robot uit
      // de bocht gevlogen -> naar die kant terugsturen.
      ctrl.state = (fabs(lastLineError) < 0.5f * POSITION_MAX_MM) ? ST_GAP : ST_RECOVER;
    }
    if (now - lostSince > params.lostTimeout)
    {
      ctrl.state = ST_LOST;
      return false;
    }
    if (ctrl.state == ST_GAP)
    {
      error = lastLineError;
      // na de halve timeout nog steeds niets: toch maar gaan zoeken
      if (now - lostSince > params.lostTimeout / 2) ctrl.state = ST_RECOVER;
    }
    if (ctrl.state == ST_RECOVER)
    {
      error = (lastLineError >= 0 ? 1.0f : -1.0f) * POSITION_MAX_MM;
      speedFactor = 0.5f;
    }
  }

  // ---------------------------------------------------------- 2. PID
  ctrl.error = error;
  ctrl.p = params.kp * error;

  float deriv = (error - prevError) / dt;
  prevError = error;
  dFiltered = params.dFilter * dFiltered + (1.0f - params.dFilter) * deriv;
  ctrl.d = params.kd * dFiltered;

  // Anti-windup: (1) I-bijdrage begrensd op iLimit, (2) niet verder
  // integreren als de uitgang al verzadigd is in dezelfde richting.
  float uLimit = 2.0f * params.maxPwm;
  float uNoI = ctrl.p + ctrl.d + params.ki * integral;
  bool saturated = fabs(uNoI) >= uLimit && (uNoI > 0) == (error > 0);
  if (params.ki > 0 && !saturated && ctrl.state == ST_FOLLOW)
  {
    integral += error * dt;
    float iMax = params.iLimit / params.ki;
    integral = constrain(integral, -iMax, iMax);
  }
  ctrl.i = params.ki * integral;

  ctrl.u = constrain(ctrl.p + ctrl.i + ctrl.d, -uLimit, uLimit);

  // ---------------------------------------------------------- 3. snelheid
  float ratio = fabs(error) / POSITION_MAX_MM;
  if (ratio > 1) ratio = 1;
  float base = params.power * (1.0f - params.slow * ratio) * speedFactor;

  // Verdeling zoals in de template: "diff" bepaalt welk deel van de correctie
  // bij het buitenwiel opgeteld wordt; de rest gaat van het binnenwiel af.
  float u = ctrl.u, left, right;
  if (u >= 0) { left = base + params.diff * u;  right = left - u; }
  else        { right = base - params.diff * u; left = right + u; }

  // Verschil (= stuurcorrectie) behouden als een wiel boven max uitkomt
  float maxPwm = params.maxPwm;
  float hi = max(left, right);
  if (hi > maxPwm) { left -= hi - maxPwm; right -= hi - maxPwm; }
  float lo = params.allowReverse ? -maxPwm : 0.0f;
  ctrl.left  = (int16_t)constrain(left,  lo, maxPwm);
  ctrl.right = (int16_t)constrain(right, lo, maxPwm);

  motorsSet(ctrl.left, ctrl.right);
  return true;
}
