/*
 * Projekt: Overkørsel st. enkeltsporet strækning
 * Produkt: Manuelt halvbom anlæg
 * Version: 1.0
 * Type: Program
 * Programmeret af: Jan Birch
 * Opdateret: 12-06-2021
 * GNU General Public License version 3
 * This file is part of "Manuelt halvbom anlæg hw test".
 * 
 * "Manuelt halvbom anlæg hw test" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Manuelt halvbom anlæg hw test" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Manuelt halvbom anlæg hw test".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: Programmet bliver brugt til at teste at hardware er monteret korrekt
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#include <Servo.h>
// Opsætning af konstanter og specifikationer

// Arduino pins
struct {
  const byte ManuelKnap = 2;
  const byte OUSignAB = 7;
  const byte OUSignBA = 8;
  const byte Vejbom = 9;
  const byte VejKlokker = 10;
  const byte VejLys = 11;
} ardPin;

// Manuel knap
  int manButtonValue = 0;
  boolean wasHigh = false;

// Servomotor
Servo servoMotor;
const int StartAngle = 10;
const int EndAngle = 100;
const int AnglePmsek = 20;
int pos = StartAngle;    // variabel til servo position

void setup() {
  // put your setup code here, to run once:
  pinMode(ardPin.ManuelKnap, INPUT_PULLUP);
  pinMode(ardPin.OUSignAB, OUTPUT);
  pinMode(ardPin.OUSignBA, OUTPUT);
  servoMotor.attach(ardPin.Vejbom);
  servoMotor.write(pos);
  pinMode(ardPin.VejKlokker, OUTPUT);
  pinMode(ardPin.VejLys, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  manButtonValue = digitalRead(ardPin.ManuelKnap);
  if (manButtonValue == LOW) {
    digitalWrite(ardPin.OUSignAB, HIGH);
    digitalWrite(ardPin.OUSignBA, HIGH);
    digitalWrite(ardPin.VejKlokker, HIGH);
    digitalWrite(ardPin.VejLys, HIGH);
    for (pos = StartAngle; pos <= EndAngle; pos += 1) {
      servoMotor.write(pos);
      delay(AnglePmsek);
    }
    wasHigh = true;
  }
  delay(1000);
  if (wasHigh == true) {
    digitalWrite(ardPin.OUSignAB, LOW);
    digitalWrite(ardPin.OUSignBA, LOW);
    digitalWrite(ardPin.VejKlokker, LOW);
    digitalWrite(ardPin.VejLys, LOW);
    for (pos = EndAngle; pos >= StartAngle; pos -= 1) {
      servoMotor.write(pos);
      delay(AnglePmsek);
    }
    wasHigh = false;   
  }
}
