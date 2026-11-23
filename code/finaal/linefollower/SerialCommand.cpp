// SerialCommand - zie SerialCommand.h voor de wijzigingen t.o.v. de template.
#include "SerialCommand.h"
#include <string.h>

static const char DELIMITERS[] = " =:\t";

SerialCommand::Callback SerialCommand::commandList[MAXSERIALCOMMANDS];
uint8_t SerialCommand::numCommand = 0;
void (*SerialCommand::defaultHandler)(char *) = NULL;
SerialCommand *SerialCommand::active = NULL;

SerialCommand::SerialCommand(Stream &stream)
  : serial(&stream), bufPos(0), overflow(false), echo(true), last(NULL)
{
  clearBuffer();
  if (active == NULL) active = this;
}

void SerialCommand::clearBuffer()
{
  memset(buffer, 0, sizeof(buffer));
  bufPos = 0;
  overflow = false;
}

char *SerialCommand::next()
{
  return strtok_r(NULL, DELIMITERS, &last);
}

void SerialCommand::addCommand(const char *command, void (*function)())
{
  if (numCommand < MAXSERIALCOMMANDS)
  {
    commandList[numCommand].command = command;
    commandList[numCommand].function = function;
    numCommand++;
  }
}

void SerialCommand::setDefaultHandler(void (*function)(char *))
{
  defaultHandler = function;
}

void SerialCommand::readSerial()
{
  // Maximaal een beperkt aantal tekens per oproep verwerken zodat de
  // regelkring nooit lang geblokkeerd wordt.
  uint8_t budget = 16;
  while (budget-- && serial->available() > 0)
  {
    char c = (char)serial->read();

    if (c == '\r' || c == '\n')
    {
      if (overflow)
      {
        serial->println(F("ERR regel te lang"));
        clearBuffer();
      }
      else if (bufPos > 0)
      {
        dispatch();
        clearBuffer();
      }
      continue;                               // lege regel (bv. '\n' na '\r') negeren
    }

    if (c == 0x08 || c == 0x7F)               // backspace / delete
    {
      if (bufPos > 0) buffer[--bufPos] = '\0';
      continue;
    }

    if (!isprint(c)) continue;                // overige stuurtekens negeren

    if (bufPos < SERIALCOMMANDBUFFER - 1)
    {
      buffer[bufPos++] = tolower(c);
      buffer[bufPos] = '\0';
    }
    else
    {
      overflow = true;                        // regel wordt verworpen bij Enter
    }
  }
}

void SerialCommand::dispatch()
{
  // Echo van de volledige regel NA Enter (niet per teken): per-teken-echo over
  // SoftwareSerial blokkeert de interrupts tijdens het zenden waardoor bij
  // plakken inkomende tekens verloren gaan.
  if (echo)
  {
    serial->print(F("> "));
    serial->println(buffer);
  }

  char *token = strtok_r(buffer, DELIMITERS, &last);
  if (token == NULL) return;

  active = this;
  for (uint8_t i = 0; i < numCommand; i++)
  {
    if (strcmp(token, commandList[i].command) == 0)
    {
      commandList[i].function();
      return;
    }
  }
  if (defaultHandler != NULL) defaultHandler(token);
}
