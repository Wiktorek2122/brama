
#include <Servo.h>
#include <IRremote.hpp>

// =================================
// DEFINICJE STANOW MASZYNY
// =================================

#define ST_ZAMKNIETA       1
#define ST_OTWIERA_SIE     2
#define ST_UCHYLONA_OTW    3
#define ST_OTWARTA         4
#define ST_ZAMYKA_SIE      5
#define ST_UCHYLONA_ZAM    6

// =================================
// ELEMENTY
// =================================

Servo barrera;

const int SERVO_PIN = 11;

const int pulsador1 = 7;
const int pulsador2 = 8;

const int LED = 2;
const int LED2 = 4;

const int IR_PIN = 12;

// =================================
// POZYCJE SERWA
// =================================

const int POZYCJA_ZAMKNIETA = 0;
const int POZYCJA_UCHYLONA = 45;
const int POZYCJA_OTWARTA = 90;

// =================================
// CZASY
// =================================

const unsigned long CZAS_RUCHU = 10000;
const unsigned long CZAS_LED = 500;
const unsigned long CZAS_DEBOUNCE = 35;

// =================================
// ZMIENNE
// =================================

int state = ST_ZAMKNIETA;

int aktualnyKat = POZYCJA_ZAMKNIETA;
int katStart = POZYCJA_ZAMKNIETA;
int katKoniec = POZYCJA_ZAMKNIETA;

unsigned long czasStart = 0;
unsigned long czasMigania = 0;

bool ledStan = false;

// =================================
// KODY PILOTA
// =================================

// Kody przykladowe.
// Sprawdz swoje kody w monitorze portu.

#define KOD_OTWORZ 0x18
#define KOD_ZAMKNIJ 0x52

// =================================
// OBSLUGA PRZYCISKOW
// =================================

// Przyciski podlaczone do GND.
// Uzywamy INPUT_PULLUP.

bool sprawdzPrzycisk(int pin)
{
  static bool ostatniOdczyt[2] = {HIGH, HIGH};
  static bool stabilnyStan[2] = {HIGH, HIGH};
  static unsigned long czasZmiany[2] = {0, 0};

  int indeks = (pin == pulsador1) ? 0 : 1;

  bool odczyt = digitalRead(pin);

  if (odczyt != ostatniOdczyt[indeks])
  {
    ostatniOdczyt[indeks] = odczyt;
    czasZmiany[indeks] = millis();
  }

  if (millis() - czasZmiany[indeks] >= CZAS_DEBOUNCE)
  {
    if (odczyt != stabilnyStan[indeks])
    {
      stabilnyStan[indeks] = odczyt;

      if (stabilnyStan[indeks] == LOW)
      {
        return true;
      }
    }
  }

  return false;
}

// =================================
// ROZPOCZECIE OTWIERANIA
// =================================

void rozpocznijOtwieranie()
{
  if (state == ST_ZAMKNIETA)
  {
    katStart = POZYCJA_ZAMKNIETA;
    katKoniec = POZYCJA_OTWARTA;

    czasStart = millis();
    czasMigania = millis();

    ledStan = false;

    digitalWrite(LED, LOW);
    digitalWrite(LED2, LOW);

    state = ST_OTWIERA_SIE;

    Serial.println("Rozpoczynam otwieranie");
  }
}

// =================================
// ROZPOCZECIE ZAMYKANIA
// =================================

void rozpocznijZamykanie()
{
  if (state == ST_OTWARTA)
  {
    katStart = POZYCJA_OTWARTA;
    katKoniec = POZYCJA_ZAMKNIETA;

    czasStart = millis();
    czasMigania = millis();

    ledStan = false;

    digitalWrite(LED, LOW);
    digitalWrite(LED2, LOW);

    state = ST_ZAMYKA_SIE;

    Serial.println("Rozpoczynam zamykanie");
  }
}

// =================================
// MIGANIE LED
// =================================

void miganieLED()
{
  if (millis() - czasMigania >= CZAS_LED)
  {
    czasMigania = millis();

    ledStan = !ledStan;

    if (state == ST_OTWIERA_SIE ||
        state == ST_UCHYLONA_OTW)
    {
      digitalWrite(LED2, ledStan);
      digitalWrite(LED, LOW);
    }

    if (state == ST_ZAMYKA_SIE ||
        state == ST_UCHYLONA_ZAM)
    {
      digitalWrite(LED, ledStan);
      digitalWrite(LED2, LOW);
    }
  }
}

// =================================
// RUCH SERWA
// =================================

void sterowanieSerwem()
{
  unsigned long czas = millis() - czasStart;

  float postep = (float)czas / CZAS_RUCHU;

  if (postep > 1.0)
  {
    postep = 1.0;
  }

  aktualnyKat = katStart +
    (katKoniec - katStart) * postep;

  barrera.write(aktualnyKat);
}

// =================================
// AKTUALIZACJA RUCHU
// =================================

void aktualizujRuch()
{
  unsigned long czas = millis() - czasStart;

  // Otwieranie

  if (state == ST_OTWIERA_SIE ||
      state == ST_UCHYLONA_OTW)
  {
    sterowanieSerwem();
    miganieLED();

    if (czas >= CZAS_RUCHU / 2 &&
        state == ST_OTWIERA_SIE)
    {
      state = ST_UCHYLONA_OTW;
      Serial.println("Brama uchylona - otwieranie");
    }

    if (czas >= CZAS_RUCHU)
    {
      aktualnyKat = POZYCJA_OTWARTA;
      barrera.write(POZYCJA_OTWARTA);

      digitalWrite(LED, LOW);
      digitalWrite(LED2, LOW);

      state = ST_OTWARTA;

      Serial.println("Brama otwarta");
    }
  }

  // Zamykanie

  if (state == ST_ZAMYKA_SIE ||
      state == ST_UCHYLONA_ZAM)
  {
    sterowanieSerwem();
    miganieLED();

    if (czas >= CZAS_RUCHU / 2 &&
        state == ST_ZAMYKA_SIE)
    {
      state = ST_UCHYLONA_ZAM;
      Serial.println("Brama uchylona - zamykanie");
    }

    if (czas >= CZAS_RUCHU)
    {
      aktualnyKat = POZYCJA_ZAMKNIETA;
      barrera.write(POZYCJA_ZAMKNIETA);

      digitalWrite(LED, LOW);
      digitalWrite(LED2, LOW);

      state = ST_ZAMKNIETA;

      Serial.println("Brama zamknieta");
    }
  }
}

// =================================
// SETUP
// =================================

void setup()
{
  Serial.begin(9600);

  // Serwo

  barrera.attach(SERVO_PIN);

  barrera.write(POZYCJA_ZAMKNIETA);

  // Przyciski

  pinMode(pulsador1, INPUT_PULLUP);
  pinMode(pulsador2, INPUT_PULLUP);

  // Diody

  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED, LOW);
  digitalWrite(LED2, LOW);

  // Odbiornik IR

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  // Stan poczatkowy

  state = ST_ZAMKNIETA;

  Serial.println("============================");
  Serial.println("MASZYNA STANOW - BRAMA");
  Serial.println("Arduino gotowe do pracy");
  Serial.println("============================");
}

// =================================
// LOOP
// =================================

void loop()
{
  // PRZYCISK OTWIERANIA

  if (sprawdzPrzycisk(pulsador1))
  {
    rozpocznijOtwieranie();
  }

  // PRZYCISK ZAMYKANIA

  if (sprawdzPrzycisk(pulsador2))
  {
    rozpocznijZamykanie();
  }

  // PILOT IR

  if (IrReceiver.decode())
  {
    uint8_t kod = IrReceiver.decodedIRData.command;

    Serial.print("Kod pilota: 0x");
    Serial.println(kod, HEX);

    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT))
    {
      if (kod == KOD_OTWORZ)
      {
        rozpocznijOtwieranie();
      }

      if (kod == KOD_ZAMKNIJ)
      {
        rozpocznijZamykanie();
      }
    }

    IrReceiver.resume();
  }

  // MASZYNA STANOW

  switch (state)
  {
    case ST_ZAMKNIETA:
      digitalWrite(LED, LOW);
      digitalWrite(LED2, LOW);
      break;

    case ST_OTWIERA_SIE:
    case ST_UCHYLONA_OTW:
    case ST_ZAMYKA_SIE:
    case ST_UCHYLONA_ZAM:
      aktualizujRuch();
      break;

    case ST_OTWARTA:
      digitalWrite(LED, LOW);
      digitalWrite(LED2, LOW);
      break;
  }
}
