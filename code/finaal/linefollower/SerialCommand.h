/*******************************************************************************
SerialCommand - tokenize and parse commands received over a serial stream.
Original: Copyright (C) 2011-2013 Steven Cogswell (LGPL 2.1), zoals gebruikt in
de HOGENT basistemplate.

Aangepast voor de linefollower (zelfde API als de template):
  * Een commando eindigt op '\r' OF '\n' (PuTTY stuurt enkel '\r' bij Enter,
    de Arduino Serial Monitor vaak enkel '\n', plakken stuurt "\r\n").
    Een lege regel (bv. de '\n' na '\r') wordt genegeerd.
  * Backspace (0x08) en DEL (0x7F) wissen het laatste teken -> zelf typen in
    PuTTY werkt nu ook als je een tikfout verbetert.
  * Optionele echo: getypte tekens worden teruggestuurd (PuTTY toont bij een
    seriele verbinding standaard niets van wat je typt).
  * Te lange regel: de regel wordt verworpen en gemeld i.p.v. stilletjes
    rond te lopen in de buffer (dat gaf vroeger rare commando's).
  * Scheidingstekens: spatie, '=' en ':' -> "kp=0.5", "kp 0.5" en
    "set kp 0.5" werken alle drie.
  * Commando's zijn niet hoofdlettergevoelig ("SHOW" = "show").
  * De commandotabel is gedeeld (static) zodat USB en Bluetooth elk een eigen
    SerialCommand-object hebben zonder dubbel RAM-gebruik.
  * SerialCommand::reply() geeft de stream terug waarop het commando binnenkwam,
    zodat het antwoord naar de juiste poort gaat.
*******************************************************************************/
#ifndef SerialCommand_h
#define SerialCommand_h

#include <Arduino.h>

#define SERIALCOMMANDBUFFER 40
#define MAXSERIALCOMMANDS   20

class SerialCommand
{
  public:
    explicit SerialCommand(Stream &stream);

    void readSerial();                                  // elke lus oproepen
    char *next();                                       // volgend argument (of NULL)
    void clearBuffer();
    void setEcho(bool on) { echo = on; }
    Stream &stream() { return *serial; }

    static void addCommand(const char *command, void (*function)());
    static void setDefaultHandler(void (*function)(char *));
    static Stream &reply() { return *active->serial; }  // poort van het huidige commando
    static SerialCommand *activeCommand() { return active; }

  private:
    void dispatch();

    Stream *serial;
    char buffer[SERIALCOMMANDBUFFER];
    uint8_t bufPos;
    bool overflow;
    bool echo;
    char *last;

    struct Callback { const char *command; void (*function)(); };
    static Callback commandList[MAXSERIALCOMMANDS];
    static uint8_t numCommand;
    static void (*defaultHandler)(char *);
    static SerialCommand *active;
};

#endif
