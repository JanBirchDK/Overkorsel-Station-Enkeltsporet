/*
 * Projekt: Overkørsel st. enkeltsporet strækning
 * Produkt: Varslingsanlæg faste tider
 * Version: 0.1
 * Type: Program
 * Programmeret af: Jan Birch
 * Opdateret: 11-04-2021
 * GNU General Public License version 3
 * Noter:
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

// Der er behov for flere timere, det leverer biblioteket BlockNot 
#include <BlockNot.h>

// En knap elektriske forbindelse
enum contacType {NOPEN, NCLOSED};

// Ansvar: Denne klasse varetager al funktion af en trykknap. Software er et spejl af hardwarefunktion.
// Aflæsning af hardware port. Filtrering af kontaktprel. Grænseflade til software.
// Seqs: En knap løber igennem 2 trin, når der trykkes på den
// Udløbstid for timer til kontaktprel bliver sat til 30msek
// pin: Arduino portnr
// bounceTimer: Timer til kontaktprel
// value: Knappen er høj eller lav
// seq: Knappens trin
// polling(...): Gennemløb på tid
// read(...): Udlæses knappens værdi
class PushButton {
private:
  enum Seqs {STABLE, BOUNCE};
  enum {BOUNCTIME = 30};
  byte pin;
  BlockNot bounceTimer;
  bool value;
  Seqs seq;
public:
  PushButton(byte a_pin, contacType a_contact);
  void polling(void);
  bool read(void);
};

PushButton::PushButton(byte a_pin, contacType a_contact) : pin(a_pin), bounceTimer(BOUNCTIME), seq(STABLE) {
  if (a_contact= NCLOSED) pinMode(pin, INPUT_PULLUP);
  else pinMode(pin, INPUT);
  value = digitalRead(pin);
}

// Knap går fra fast stilling til undervejs mod ny stilling
// Der ventes en periode, for at udelukke kontaktprel
// IO portens værdi aflæses
void PushButton::polling(void){
  switch (seq) {
    case STABLE:
      if (value != digitalRead(pin)) {
        bounceTimer.reset();
        seq = BOUNCE;
      }
    break;
    case BOUNCE:
      if (bounceTimer.firstTrigger() == true) {
        value = digitalRead(pin);
        seq = STABLE;
      }
    break;
  }
}

bool PushButton::read(void){
  return value;
}

//----------

// Ansvar: Denne klasse varetager al funktion af simpel tændt eller slukket udgang. Software er et spejl af hardwarefunktion.
// Udlæsning til hardware port. Grænseflade til software.
// pin: Arduino portnr
// write(...): Indlæse værdi
class SimpleOnOff {
private:
  byte pin;              // Arduino pin
public:
  SimpleOnOff(byte a_pin, bool a_value);
  void write(bool a_value);
};

SimpleOnOff::SimpleOnOff(byte a_pin, bool a_value = LOW) : pin(a_pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, a_value);
}

void SimpleOnOff::write(bool a_value) {
  digitalWrite(pin, a_value);
}

//----------

// Ansvar: Denne klasse varetager blinkerfunktionen.
// Periodetid 1000msek
// timer: Beregner tid
// value: Blinkerens værdi lav eller høj
// isChanged: Sat hvis skiftet værdi
// polling(...): Gennemløb på tid
// hasUpdate(...): Viser om blinker har skiftet værdi
// read(...): Udlæses blinkerens værdi
// resetTrigger(...): Når brugere af blinker er færdige med opdatering, så resættes besked om trigger
class TheBlinker {
private:
  enum {PERIOD = 1000};
  BlockNot timer;
  bool value;
  bool isChanged;
public:
  TheBlinker(void);
  void polling(void);
  bool hasUpdate(void);
  bool read(void);
  void resetTrigger(void);
};

TheBlinker::TheBlinker(void) : timer(PERIOD), value(LOW), isChanged(false) {}

void TheBlinker::polling(void) {
  if((isChanged = timer.triggered()) == true) value = !value;
}

bool TheBlinker::hasUpdate(void) {
  return isChanged;
}

bool TheBlinker::read(void) {
  return value;
}

void TheBlinker::resetTrigger(void) {
  isChanged = false;
}

TheBlinker blinker;  // Når argumentliste er void skal der ikke paranteser på objekt

//----------

// Opsætning af konstanter og specifikationer til overkørsel
// Arduino pins
struct {
  const int ManuelKnap = 2;
  const int OUSignAB = 7;
  const int OUSignBA = 8;
  const int VejKlokker = 10;
  const int VejLys = 11;
} ARDPin;

// Knap
PushButton manuelKnap(ARDPin.ManuelKnap, NCLOSED); // Knappen er forbundet til 5V og normally closed

// Uordenssignaler
SimpleOnOff OUSignBA(ARDPin.OUSignBA, HIGH);
SimpleOnOff OUSignAB(ARDPin.OUSignAB, HIGH);
SimpleOnOff VejKlokker(ARDPin.VejKlokker);
SimpleOnOff VejLys(ARDPin.VejLys);

// Tilstandsmaskine
enum States {IKKESIKRET, SIKRET, OPLOES, BILISTTID};
States state;  // Aktuel tilstand
// Tider for tilstande
struct {
  const unsigned long Sikret = 15000, Oploes = 15000, Bilist = 10000;
} stateTimes;
BlockNot stateTimer(stateTimes.Sikret);

void setup() {
  // put your setup code here, to run once:
// Start tilstandsmaskine
  state = IKKESIKRET;
//  Serial.begin(9600);
// Serial.println("bouncedw");
}

void loop() {
  // put your main code here, to run repeatedly:
// Udfør næste polling
  blinker.polling();
  manuelKnap.polling();
// Kør tilstandsmaskine
  switch (state) {
    case IKKESIKRET:
      if (manuelKnap.read() == false) {
        OUSignAB.write(LOW);
        OUSignBA.write(LOW);
        stateTimer.setDuration(stateTimes.Sikret);
        state = SIKRET;
      }
    break;
    case SIKRET:
      if (stateTimer.triggered() == true) {
        OUSignAB.write(HIGH);
        OUSignBA.write(HIGH);
        stateTimer.setDuration(stateTimes.Oploes);
        state = OPLOES;      
      }
    break;
    case OPLOES:
      if (stateTimer.triggered() == true) {
        VejKlokker.write(LOW);
        VejLys.write(LOW);
        stateTimer.setDuration(stateTimes.Bilist);
        state = BILISTTID;
      }
    break;
    case BILISTTID:
      if (stateTimer.triggered() == true) state = IKKESIKRET;
    break;
  }
// Vejlys og klokker er tændt, når overkørsel er tændt
  if ((state == SIKRET) || (state == OPLOES)) {
    if (blinker.hasUpdate() == true) {
      VejKlokker.write(blinker.read());
      VejLys.write(blinker.read());
      blinker.resetTrigger();
    }    
  }
// Langsom polling er nyttig ved debug
//   delay(100);
}
