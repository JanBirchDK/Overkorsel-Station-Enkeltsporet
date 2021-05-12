/*
 * Projekt: Overkørsel st. enkeltsporet strækning
 * Produkt: Varslingsanlæg fast sekvens
 * Version: 0.3
 * Type: Program
 * Programmeret af: Jan Birch
 * Opdateret: 10-05-2021
 * GNU General Public License version 3
 * This file is part of "Varslingsanlæg faste tider".
 * 
 * "Varslingsanlæg fast sekvens" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Varslingsanlæg fast sekvens" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Varslingsanlæg fast sekvens".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

// Kontakttyper for trykknap
enum {NOPEN, NCLOSED};
// Overkørslens betjeningsenheder magasin kan konfigureres
enum {BISTABLE, ONESHOT};
// Overkørslens betjeningsenheder leverer
enum {OFF, ON};
// Overkørslens ydre enheder kan blive sat til
enum {BLOCK, PASS};
// Tilstandsmaskine
enum {IKKESIKRET, FORRING, SIKRET, OPLOES, BILISTTID};

// Overkørslens moduler
#include "OvkTiming.h"
#include "OvkHWDrivere.h"
#include "OvkCtrl.h"
#include "OvkDevice.h"

// Arduino pins
struct {
  const byte ManuelKnap = 2;
  const byte OUSignAB = 7;
  const byte OUSignBA = 8;
  const byte VejKlokker = 10;
  const byte VejLys = 11;
} ARDPin;

// Hardware drivere til den overkørsel, som dette program leverer
t_PushButton manuelKnapDrv(ARDPin.ManuelKnap, NCLOSED); // Knappen er forbundet til 5V og normally closed
t_SimpleOnOff OUSignABDrv(ARDPin.OUSignAB, HIGH);
t_SimpleOnOff OUSignBADrv(ARDPin.OUSignBA, HIGH);
t_SimpleOnOff vejKlokkerDrv(ARDPin.VejKlokker);
t_SimpleOnOff vejLysDrv(ARDPin.VejLys);

// Overkørslens betjenings- og ydre enheder
t_CrossingCtrl manuelBetj;
t_RailSignal OUSignAB;
t_RailSignal OUSignBA;
t_RoadSignal vejKlokker(PASS);
t_RoadSignal vejLys(PASS);

// Tilstandsmaskine
byte state;  // Aktuel tilstand
// Tider for tilstande
struct {
  const bool inSeconds = true;
  const unsigned long Forring = 2, Sikret = 15, Oploes = 15, Bilist = 10;
} stateTimes;
t_ClockWork stateTimer;

void setup() {
  // put your setup code here, to run once:
// Drivere kobles til betjenings- og ydre enheder
  manuelBetj.setDriver(&manuelKnapDrv);
  OUSignAB.setDriver(&OUSignABDrv);
  OUSignBA.setDriver(&OUSignBADrv);
  vejKlokker.setDriver(&vejKlokkerDrv);
  vejLys.setDriver(&vejLysDrv);
// Start tilstandsmaskine
  state = IKKESIKRET;
}

void loop() {
  // put your main code here, to run repeatedly:
  Clock::pendulum();
  Blinker::doClockCycle();
  manuelBetj.doClockCycle();
  vejKlokker.doClockCycle();
  vejLys.doClockCycle();
// Kør tilstandsmaskine
  switch (state) {
    case IKKESIKRET:
      if (manuelBetj.status()==OFF) {
        stateTimer.setDuration(stateTimes.Forring, stateTimes.inSeconds);
        vejKlokker.to(BLOCK);
        vejLys.to(BLOCK);
        state = FORRING;     
      }
    break;
    case FORRING:
      if (stateTimer.triggered() == true) {
        stateTimer.setDuration(stateTimes.Sikret, stateTimes.inSeconds);
        OUSignAB.to(PASS);
        OUSignBA.to(PASS);
        state = SIKRET;
      }
    break;
    case SIKRET:
      if (stateTimer.triggered() == true) {
        OUSignAB.to(BLOCK);
        OUSignBA.to(BLOCK);
        stateTimer.setDuration(stateTimes.Oploes, stateTimes.inSeconds);
        state = OPLOES;      
      }    
    break;
    case OPLOES:
      if (stateTimer.triggered() == true) {
        vejKlokker.to(PASS);
        vejLys.to(PASS);
        stateTimer.setDuration(stateTimes.Bilist, stateTimes.inSeconds);
        state = BILISTTID;
      }    
    break;
    case BILISTTID:
      if (stateTimer.triggered() == true) state = IKKESIKRET;    
    break; 
  }  
}
