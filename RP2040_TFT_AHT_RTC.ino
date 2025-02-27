/*(c)Joyfab. By Joy. jan 2025.
  Two axis Azimut/Elevation Rx/Tx Suntracker with GPS.
  (RP2040-Zero) Joy jan 2022. updated jan 2025.
  Thanks to libraries used:
  earlephilhower pico library: https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
  Adafruit-GFX : https://github.com/adafruit/Adafruit-GFX-Library
  Adafruit-ST7735 : https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/Adafruit_ST7735.cpp
  AT24C256 eeprom : https://github.com/dantudose/AT24C256
  AHT sensor: https://github.com/enjoyneering/AHTxx
  DS3231 : https://github.com/NorthernWidget/DS3231
*/
#include <Arduino.h>
#include <Wire.h>
#include "PioSPI.h"
PioSPI spiBus(4, 3, 2, 5, SPI_MODE3, 10000000); //MOSI, MISO, SCK, CS, SPI_MODE, FREQUENCY
#include "at24c256.h"
at24c256 eeprom(0x50);
#include <AHT10.h>
AHT10 myAHT10(AHT10_ADDRESS_0X38);
uint8_t readStatus = 0;
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#define BK       3
#define TFT_CS   5
#define TFT_RST 10
#define TFT_DC   6
#define TFT_MOSI 4
#define TFT_SCLK 2
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#include <DS3231.h>
DS3231 Clock;
bool Century = false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
byte year, month, date, DoW, hour, minute, second;
int MinTemp;
int MaxTemp;
int MinHum;
int MaxHum;
int MinNeg;
char blop;
String input = "";
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  pinMode(BK, OUTPUT);
  digitalWrite(TFT_RST, HIGH);
  delay(1);
  digitalWrite(TFT_RST, LOW);
  delay(1);
  digitalWrite(TFT_RST, HIGH);
  delay(1);
  digitalWrite(BK, HIGH);
  Wire.setSDA(12);
  Wire.setSCL(13);
  Wire.setClock(40);
  Wire.begin();
  eeprom.init();
  MinTemp = eeprom.read(30000);
  MaxTemp = eeprom.read(30001);
  MinHum = eeprom.read(30002);
  MaxHum = eeprom.read(30003);
  MinNeg = eeprom.read(30004);
  myAHT10.begin();
  tft.initR(INITR_MINI160x80_PLUGIN);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(2, 5);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.print("RP2040z Horloge");
  tft.setCursor(2, 15);
  tft.print("Thermohygrometre");
  tft.setCursor(2, 30);
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  tft.print("Minima Maxima");
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  tft.setCursor(2, 45);
  tft.print("By Joy.");
  delay(4000);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(74, 54);
  tft.print("C");
  tft.setCursor(136, 54);
  tft.print("%");
  tft.setCursor(74, 64);
  tft.print("C");
  tft.setCursor(136, 64);
  tft.print("%");
  tft.setCursor(146, 34);
  tft.print("%");
  tft.setCursor(70, 34);
  tft.print("C");
  tft.setCursor(12, 54);
  tft.print("Maxi");
  tft.setCursor(12, 64);
  tft.print("Mini");
}
void displayTime() {
  tft.setTextSize(1);
  tft.setCursor(2, 2);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  switch (Clock.getDoW()) {
    case 1:
      tft.print("Dimanche ");
      break;
    case 2:
      tft.print("   Lundi ");
      break;
    case 3:
      tft.print("   Mardi ");
      break;
    case 4:
      tft.print("Mercredi ");
      break;
    case 5:
      tft.print("   Jeudi ");
      break;
    case 6:
      tft.print("Vendredi ");
      break;
    case 7:
      tft.print("  Samedi ");
      break;
    default: break;
  }
  tft.print(Clock.getDate());
  switch (Clock.getMonth(Century)) {
    case 1:
      tft.print(" Janvier   ");
      break;
    case 2:
      tft.print(" Fevrier   ");
      break;
    case 3:
      tft.print(" Mars      ");
      break;
    case 4:
      tft.print(" Avril     ");
      break;
    case 5:
      tft.print(" Mai       ");
      break;
    case 6:
      tft.print(" Juin      ");
      break;
    case 7:
      tft.print(" Juillet   ");
      break;
    case 8:
      tft.print(" Aout      ");
      break;
    case 9:
      tft.print(" Septembre ");
      break;
    case 10:
      tft.print(" Octobre   ");
      break;
    case 11:
      tft.print(" Novembre  ");
      break;
    case 12:
      tft.print(" Decembre  ");
      break;
    default: break;
  }
  tft.print("20");
  tft.print(Clock.getYear());
  tft.setTextSize(2);
  tft.setCursor(30, 14);
  tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
  if (Clock.getHour(h12, PM) <= 9)  {
    tft.print("0");
  }
  tft.print(Clock.getHour(h12, PM)); // GMT
  tft.print(":");
  if (Clock.getMinute() <= 9)  {
    tft.print("0");
  }
  tft.print(Clock.getMinute());
  tft.print(":");
  if (Clock.getSecond() <= 9)  {
    tft.print("0");
  }
  tft.print(Clock.getSecond());
  tft.fillRect(126, 10, 26, 18, ST77XX_BLACK);
}
void measure()  {
  readStatus = myAHT10.readRawData();
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < MinTemp) { //on teste si on  dépasse la consigne la 1e fois
    MinTemp = MinTemp - 1;
    if ((MinTemp - 1) >= 0) {
      eeprom.update(30000, MinTemp);
    }
    if ((MinTemp + 1) < 0) {
      eeprom.update(30000, abs(MinTemp));
      eeprom.update(30004, 1);
    }
  }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) > MaxTemp) { //on teste si on  dépasse la consigne la 1e fois
    MaxTemp = MaxTemp + 1;
    eeprom.update(30001, MaxTemp);
  }
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) < MinHum)  {
    MinHum = MinHum - 1;
    eeprom.update(30002, MinHum);
  }
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) > MaxHum)  {
    MaxHum = MaxHum + 1;
    eeprom.update(30003, MaxHum);
  }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) > 23)  {
    tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
  }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < 23)
    if (myAHT10.readTemperature(AHT10_USE_READ_DATA) >= 22)  {
      tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
    }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < 22)
    if (myAHT10.readTemperature(AHT10_USE_READ_DATA) >= 21)  {
      tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
    }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < 22)
    if (myAHT10.readTemperature(AHT10_USE_READ_DATA) >= 21)  {
      tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < 21)
    if (myAHT10.readTemperature(AHT10_USE_READ_DATA) >= 20)  {
      tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
    }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < 20)
    if (myAHT10.readTemperature(AHT10_USE_READ_DATA) >= 18)  {
      tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < 18)
    if (myAHT10.readTemperature(AHT10_USE_READ_DATA) >= 16)  {
      tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < 16)
    if (myAHT10.readTemperature(AHT10_USE_READ_DATA) >= 14)  {
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    }
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < 14)  {
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  }
  tft.setTextSize(2);
  if (myAHT10.readTemperature(AHT10_USE_READ_DATA) < 10.00)  {
  tft.fillRect(52, 32, 16, 18, ST77XX_BLACK);
  }
  tft.setCursor(16, 34);
  tft.print(myAHT10.readTemperature(AHT10_USE_READ_DATA), 1); 
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) >= 90)  {
    tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
  }
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) < 90)
    if (myAHT10.readHumidity(AHT10_USE_READ_DATA) >= 80)  {
      tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
    }
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) < 80)
    if (myAHT10.readHumidity(AHT10_USE_READ_DATA) >= 70)  {
      tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
    }
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) < 70)
    if (myAHT10.readHumidity(AHT10_USE_READ_DATA) >= 60)  {
      tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
    }
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) < 60)
    if (myAHT10.readHumidity(AHT10_USE_READ_DATA) >= 50)  {
      tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    }
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) < 50)
    if (myAHT10.readHumidity(AHT10_USE_READ_DATA) >= 40)  {
      tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
    }
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) < 40)
    if (myAHT10.readHumidity(AHT10_USE_READ_DATA) >= 30)  {
      tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    }
  if (myAHT10.readHumidity(AHT10_USE_READ_DATA) < 30)  {
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  }
  tft.setCursor(94, 34);
  tft.print(myAHT10.readHumidity(AHT10_USE_READ_DATA), 1);
  tft.setTextSize(1);
  if (MaxHum >= 99)  {
    MaxHum = 99;
  }
  tft.setTextSize(1);
  tft.setCursor(50, 64);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  if (MinNeg == 1) {
    tft.print("-");
  }
  tft.print(MinTemp);
  tft.print(" ");
  tft.setCursor(50, 54);
  if (MaxTemp < 10) {
    tft.print(" ");
  }
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  tft.print(MaxTemp);
  tft.fillRect(38, 50, 10, 22, ST77XX_BLACK);
  tft.fillRect(62, 50, 10, 22, ST77XX_BLACK);
  tft.setCursor(110, 54);
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  tft.print(MaxHum);
  tft.setCursor(110, 64);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  tft.print(MinHum);
}
void checkComms() {
  readStatus = myAHT10.readRawData();
  if (Serial2.available())                   //   BLE control
    while (Serial2.available() > 0) {
      char inChar = (char)Serial2.read();
      input += inChar;
    }
  if (input.length() >= 1) {
    {
      blop = Serial2.read();
    }
    if (input == "olon") {                   // Oled on
      digitalWrite(BK, HIGH);
      Serial2.println("");
      Serial2.println("LCD ON");
    }
    if (input == "olof") {                   // Oled off
      digitalWrite(BK, LOW);
      Serial2.println("");
      Serial2.println("LCD OFF");
    }
    if (input == "s") {
      Serial2.println("");
      Serial2.print("Set 30s");
      Clock.setSecond(32);
    }
    if (input == "m+") {
      Serial2.println("");
      Serial2.print("Set minute +");
      Clock.setMinute(Clock.getMinute() + 1);
    }
    if (input == "m-") {
      Serial2.println("");
      Serial2.print("Set minute -");
      Clock.setMinute(Clock.getMinute() - 1);
    }
    if (input == "h+") {
      Serial2.println("");
      Serial2.print("Set hour +");
      Clock.setHour(Clock.getHour(h12, PM) + 1);
    }
    if (input == "h-") {
      Serial2.println("");
      Serial2.print("Set hour -");
      Clock.setHour(Clock.getHour(h12, PM) - 1);
    }
    if (input == "rsto") {                 // Reset setting
      Serial2.println("");
      Serial2.print("reset Min/Max");
      MinTemp = (myAHT10.readTemperature(AHT10_USE_READ_DATA) + 10);
      MaxTemp = myAHT10.readTemperature(AHT10_USE_READ_DATA);
      MinHum = myAHT10.readHumidity(AHT10_USE_READ_DATA);
      MaxHum = myAHT10.readHumidity(AHT10_USE_READ_DATA);
      MinNeg = 0;
      eeprom.update(30000, MinTemp);
      eeprom.update(30001, MaxTemp);
      eeprom.update(30002, MinHum);
      eeprom.update(30003, MaxHum);
      eeprom.update(30004, MinNeg);
    }
    if (input == "ctrl") {
      Serial2.println(" ");
      if (Clock.getDoW() == 1 ) {
        Serial2.print("dimanche ");
      }
      if (Clock.getDoW() == 2 ) {
        Serial2.print("lundi ");
      }
      if (Clock.getDoW() == 3 ) {
        Serial2.print("mardi ");
      }
      if (Clock.getDoW() == 4 ) {
        Serial2.print("mercredi ");
      }
      if (Clock.getDoW() == 5 ) {
        Serial2.print("jeudi ");
      }
      if (Clock.getDoW() == 6 ) {
        Serial2.print("vendredi ");
      }
      if (Clock.getDoW() == 7 ) {
        Serial2.print("samedi ");
      }
      Serial2.print(Clock.getDate());
      Serial2.print(" ");
      if (Clock.getMonth(Century) == 1 ) {
        Serial2.print("janvier ");
      }
      if (Clock.getMonth(Century) == 2 ) {
        Serial2.print("fevrier ");
      }
      if (Clock.getMonth(Century) == 3 ) {
        Serial2.print("mars ");
      }
      if (Clock.getMonth(Century) == 4 ) {
        Serial2.print("avril ");
      }
      if (Clock.getMonth(Century) == 5 ) {
        Serial2.print("mai ");
      }
      if (Clock.getMonth(Century) == 6 ) {
        Serial2.print("juin ");
      }
      if (Clock.getMonth(Century) == 7 ) {
        Serial2.print("juillet ");
      }
      if (Clock.getMonth(Century) == 8 ) {
        Serial2.print("aout ");
      }
      if (Clock.getMonth(Century) == 9 ) {
        Serial2.print("septembre ");
      }
      if (Clock.getMonth(Century) == 10 ) {
        Serial2.print("octobre ");
      }
      if (Clock.getMonth(Century) == 11 ) {
        Serial2.print("novembre ");
      }
      if (Clock.getMonth(Century) == 12 ) {
        Serial2.print("decembre ");
      }
      Serial2.print(" 20");
      Serial2.print(Clock.getYear());
      Serial2.print("  ");
      if (Clock.getHour(h12, PM) <= 9) {
        Serial2.print("0");
      }
      Serial2.print(Clock.getHour(h12, PM));
      Serial2.print(":");
      if (Clock.getMinute() <= 9) {
        Serial2.print("0");
      }
      Serial2.print(Clock.getMinute());
      Serial2.print(":");
      if (Clock.getSecond() <= 9) {
        Serial2.print("0");
      }
      Serial2.println(Clock.getSecond());
      Serial2.print("Temperature :");
      Serial2.print(myAHT10.readTemperature(AHT10_USE_READ_DATA), 1);
      Serial2.println(" C");
      Serial2.print("Max T : ");
      Serial2.println(MaxTemp);
      Serial2.print("Min T :");
      if (MinNeg == 1) {
        Serial2.print("-");
      }
      if (MinNeg == 0) {
        Serial2.print(" ");
      }
      if (MinTemp <= 9) {
        Serial2.print(" ");
      }
      Serial2.println(MinTemp);
      Serial2.print(" Humidite :");
      Serial2.print(myAHT10.readHumidity(AHT10_USE_READ_DATA), 1);
      Serial2.println(" %");
      Serial2.print("Max % :");
      Serial2.println(MaxHum);
      Serial2.print("Min % :");
      Serial2.println(MinHum);
    }
    input = "";
  }
}
void loop() {
  displayTime();
  measure();
  checkComms();
  delay(400);
}
