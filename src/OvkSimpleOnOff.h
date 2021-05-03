/*
 * Projekt: Overkørsel st. enkeltsporet strækning
 * Produkt: Overkørsel simpelt tænd sluk
 * Version: 1.0
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 26-04-2021
 * GNU General Public License version 3
 * This file is part of Overkørsel trykknap.
 * 
 * "Overkørsel simpelt tænd sluk" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Overkørsel simpelt tænd sluk" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Overkørsel simpelt tænd sluk".  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Noter:
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#include <Arduino.h>

#ifndef OvkSimpleOnOff_h
#define OvkSimpleOnOff_h

// Ansvar: Denne klasse varetager al funktion af simpel tændt eller slukket udgang. Software er et spejl af hardwarefunktion.
// Udlæsning til parallel hardware port. Grænseflade til software.
// pin: Arduino portnr
// write(...): Indlæser værdi. Sørger for kun at opdatere arduino port ved behov
// sendOut(...): Sender værdi til port
class t_SimpleOnOff: public t_DigitalOutDrv {
private:
  byte pin;
  void sendOut(void) {digitalWrite(pin, value);}
public:
  t_SimpleOnOff(byte a_pin, bool a_value);
};

t_SimpleOnOff::t_SimpleOnOff(byte a_pin, bool a_value = LOW) : t_DigitalOutDrv(a_value), pin(a_pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, a_value);
}

#endif
