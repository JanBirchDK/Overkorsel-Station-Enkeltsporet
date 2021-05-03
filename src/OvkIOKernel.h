/*
 * Projekt: Overkørsel st. enkeltsporet strækning
 * Produkt: Overkørsel IO kerne
 * Version: 0.2
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 26-04-2021
 * GNU General Public License version 3
 * This file is part of Overkørsel IO kerne.
 * 
 * "Overkørsel IO kerne" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Overkørsel IO kerne" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Overkørsel IO kerne".  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Noter:
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#include <Arduino.h>

#ifndef OvkIOKernel_h
#define OvkIOKernel_h

// Ansvar: Er grænseflade til input hardware drivere.
// value: Input værdi
// read(...): Udlæser knappens værdi
// doClockCycle(...): Gennemløber en klokcyklus
class t_DigitalInDrv {
protected:
  bool value;
public:
  t_DigitalInDrv(void) {}
  virtual void doClockCycle(void)=0;
  bool read(void) const {return value;}   
};

//----------

// Ansvar: Er grænseflade til output hardware drivere.
// value: Output værdi
// write(...): Indlæser værdi. Sørger for kun at opdatere arduino port ved behov
// sendOut(...): Sender værdi til aktuel driver
class t_DigitalOutDrv {
protected:
  bool value;
  virtual void sendOut(void)=0;
public:
  t_DigitalOutDrv(bool a_value = LOW) : value(a_value) {}
  void write(bool a_value);
};

void t_DigitalOutDrv::write(bool a_value) {
  if (a_value != value) {
    value = a_value;
    sendOut();
  }
}

//----------
/*
// Ansvar: Varetager fælles funktion for betjeningsenheder.
// member: beskrivelse
// metode(...): beskrivelse
class t_CrossingInput {
  
};

//----------

// Ansvar: Varetager fælles funktion for overkørslens ydre enheder.
// member: beskrivelse
// metode(...): beskrivelse
class t_CrossingOutput {
  
};
*/
#endif
