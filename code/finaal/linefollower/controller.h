// =============================================================================
// controller.h - PID-regeling, snelheid in bochten en lijnverlies
// =============================================================================
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

enum LineState : uint8_t
{
  ST_FOLLOW = 0,   // normaal volgen (ook op een kruispunt: rechtdoor)
  ST_GAP,          // lijn even weg terwijl ze in het midden lag: rechtdoor verder (onderbreking)
  ST_RECOVER,      // lijn weg aan de zijkant: hard bijsturen naar de kant waar ze verdween
  ST_LOST          // te lang geen lijn: robot gestopt
};

struct ControlDebug
{
  float error, p, i, d, u;
  int16_t left, right;
  LineState state;
};

extern ControlDebug ctrl;

void controllerReset();                  // bij start: integrator en filters leegmaken
bool controllerStep(float dt);           // false = lijn definitief verloren -> stoppen
const __FlashStringHelper *lineStateName(LineState s);

#endif
