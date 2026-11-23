# Projekt Arduino – Automatyczna brama sterowana pilotem IR

## 1. Opis projektu

Projekt przedstawia automatyczną bramę sterowaną za pomocą pilota na podczerwień (IR). Do sterowania ruchem bramy wykorzystano serwomechanizm, a jej aktualny proces otwierania lub zamykania sygnalizowany jest za pomocą dwóch diod LED.

Program został napisany w języku C++ dla platformy Arduino i wykorzystuje maszynę stanów do kontrolowania działania bramy.

## 2. Wykorzystane elementy

- Arduino
- Serwomechanizm
- Odbiornik podczerwieni IR
- Pilot IR
- 2 diody LED
- Rezystory do diod LED
- Przewody połączeniowe

## 3. Połączenia

| Element | Pin Arduino |
|---|---|
| Serwomechanizm – sygnał | D11 |
| Odbiornik IR – OUT | D12 |
| LED 1 – sygnalizacja zamykania | D2 |
| LED 2 – sygnalizacja otwierania | D4 |

Zasilanie odbiornika IR należy podłączyć do 5V i GND. Serwomechanizm wymaga odpowiedniego zasilania i wspólnej masy z Arduino.

## 4. Zasada działania

Brama działa w oparciu o sześć stanów:

1. **ZAMKNIĘTA** – brama znajduje się w pozycji początkowej (0°).
2. **OTWIERA SIĘ** – serwomechanizm rozpoczyna otwieranie bramy.
3. **UCHYLONA OTW** – po 5 sekundach brama osiąga około 45°.
4. **OTWARTA** – po 10 sekundach brama osiąga 90°.
5. **ZAMYKA SIĘ** – rozpoczyna się proces zamykania.
6. **UCHYLONA ZAM** – po 5 sekundach brama osiąga około 45° podczas zamykania.

Po kolejnych 5 sekundach brama wraca do pozycji zamkniętej.

## 5. Sterowanie pilotem

Projekt wykorzystuje dwa przyciski pilota:

- **OTWÓRZ** – rozpoczyna otwieranie bramy.
- **ZAMKNIJ** – rozpoczyna zamykanie bramy.

Kody pilota są zdefiniowane w programie:

```cpp
#define KOD_OTWORZ 0x18
#define KOD_ZAMKNIJ 0x52
```

Są to przykładowe kody, które należy dostosować do używanego pilota.

## 6. Sygnalizacja LED

- LED podłączona do D4 miga podczas otwierania bramy.
- LED podłączona do D2 miga podczas zamykania bramy.
- Po zakończeniu ruchu obie diody zostają wyłączone.

Miganie odbywa się co 500 ms.

## 7. Wykorzystane biblioteki

- `Servo.h` – obsługa serwomechanizmu.
- `IRremote.hpp` – odbieranie i dekodowanie sygnałów z pilota IR.

## 8. Parametry czasowe i pozycje

| Parametr | Wartość |
|---|---|
| Czas otwierania | 10 sekund |
| Czas zamykania | 10 sekund |
| Czas do stanu uchylonego | 5 sekund |
| Czas migania LED | 500 ms |
| Pozycja zamknięta | 0° |
| Pozycja uchylona | 45° |
| Pozycja otwarta | 90° |

## 9. Uruchomienie projektu

1. Podłącz wszystkie elementy zgodnie ze schematem.
2. Zainstaluj biblioteki Servo i IRremote w Arduino IDE.
3. Otwórz plik z programem.
4. Sprawdź i w razie potrzeby zmień kody pilota.
5. Wgraj program na płytkę Arduino.
6. Steruj bramą za pomocą pilota IR.

## 10. Cel projektu

Celem projektu jest stworzenie automatycznego systemu sterowania bramą z wykorzystaniem maszyny stanów, serwomechanizmu oraz komunikacji bezprzewodowej IR. Projekt pozwala zapoznać się z obsługą czujników, sterowaniem urządzeniami wykonawczymi oraz programowaniem mikrokontrolerów Arduino.
