/*
 * Projekt: Overkørsel st. enkeltsporet strækning
 * Produkt: Varslingsanlæg faste tider
 * Version: 0.2
 * Type: Program
 * Programmeret af: Jan Birch
 * Opdateret: 03-05-2021
 * GNU General Public License version 3
 * This file is part of "Varslingsanlæg faste tider".
 * 
 * "Varslingsanlæg faste tider" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Varslingsanlæg faste tider" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Varslingsanlæg faste tider".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

// Kontakttyper for trykknap
enum {NOPEN, NCLOSED};
// Overkørslens betjeningsenheder konfigureres med en flanketype
enum {DOWNFLANK, UPFLANK};
// Overkørslens betjeningsenheder leverer
enum {OFF, ON};
// Overkørslens ydre enheder kan blive sat til
enum {BLOCK, PASS};

// Overkørslens erklæringer
const byte MaxNoInputs = 0;
const byte MaxNoOutputs = 0;
const byte MaxNoStates = 5;

// Overkørslens kernemoduler
#include "OvkTiming.h"
#include "OvkIOKernel.h"

// Hardware drivere til den overkørsel, som dette program leverer
#include "OvkTrykknap.h"
#include "OvkSimpleOnOff.h"

// Overkørsel IO til den overkørsel, som dette program leverer
// Reserveret til senere brug

// Tilstandsmaskine
// Reserveret til include: tilstandsmaskine

// Opsætning af overkørsel
// Reserveret til include: Overkørsel kerne

// Arduino pins
struct {
  const byte ManuelKnap = 2;
  const byte OUSignAB = 7;
  const byte OUSignBA = 8;
  const byte VejKlokker = 10;
  const byte VejLys = 11;
} ARDPin;

// Hardware drivere instantieres
t_PushButton manuelKnap(ARDPin.ManuelKnap, NCLOSED); // Knappen er forbundet til 5V og normally closed
t_SimpleOnOff OUSignAB(ARDPin.OUSignAB, HIGH);
t_SimpleOnOff OUSignBA(ARDPin.OUSignBA, HIGH);
t_SimpleOnOff VejKlokker(ARDPin.VejKlokker);
t_SimpleOnOff VejLys(ARDPin.VejLys);

// Hardware drivere kaldes via grænseflade
t_DigitalInDrv *p_manuelKnap;
t_DigitalOutDrv *p_OUSignAB;
t_DigitalOutDrv *p_OUSignBA;
t_DigitalOutDrv *p_VejKlokker;
t_DigitalOutDrv *p_VejLys;

// Styring af blink
  bool wasHigh = true;

// Tilstandsmaskine
enum {IKKESIKRET, FORRING, SIKRET, OPLOES, BILISTTID};
byte state;  // Aktuel tilstand
// Tider for tilstande
struct {
  const bool inSeconds = true;
  const unsigned long Forring = 1, Sikret = 15, Oploes = 15, Bilist = 10;
} stateTimes;
t_ClockWork stateTimer;

void setup() {
  // put your setup code here, to run once:
// Tilkobling af hardware drivere
  p_manuelKnap = &manuelKnap;
  p_OUSignAB = &OUSignAB;
  p_OUSignBA = &OUSignBA;
  p_VejKlokker = &VejKlokker;
  p_VejLys = &VejLys;
// Start tilstandsmaskine
  state = IKKESIKRET;
//  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Clock::pendulum();
  Blinker::doClockCycle();
  manuelKnap.doClockCycle();
// Kør tilstandsmaskine
  switch (state) {
    case IKKESIKRET:
      if (p_manuelKnap->read() == LOW) {
        stateTimer.setDuration(stateTimes.Forring, stateTimes.inSeconds);
        state = FORRING;
      }
    break;
    case FORRING:
      if (stateTimer.triggered() == true) {
        stateTimer.setDuration(stateTimes.Sikret, stateTimes.inSeconds);
        p_OUSignAB->write(LOW);
        p_OUSignBA->write(LOW);
        state = SIKRET;
      }
    break;
    case SIKRET:
      if (stateTimer.triggered() == true) {
        p_OUSignAB->write(HIGH);
        p_OUSignBA->write(HIGH);
        stateTimer.setDuration(stateTimes.Oploes, stateTimes.inSeconds);
        state = OPLOES;      
      }
    break;
    case OPLOES:
      if (stateTimer.triggered() == true) {
        p_VejKlokker->write(LOW);
        p_VejLys->write(LOW);
        stateTimer.setDuration(stateTimes.Bilist, stateTimes.inSeconds);
        state = BILISTTID;
      }
    break;
    case BILISTTID:
      if (stateTimer.triggered() == true) state = IKKESIKRET;
    break;
  }
// Vejlys og klokker er tændt, når overkørsel er tændt
  if ((state == FORRING) || (state == SIKRET) || (state == OPLOES)) {
    if (blinkerTriggered() == true) {
      p_VejKlokker->write(wasHigh);
      p_VejLys->write(wasHigh);
      wasHigh = !wasHigh;
    }
  }
}
