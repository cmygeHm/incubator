#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

OneWire oneWireTemperature(2);

DallasTemperature sensor(&oneWireTemperature);

DHT dht(3, DHT22);

#define TARGET_TEMP 38
#define TEMP_MODULE 1.0
#define TARGET_HUMIDITY 70

#define H_RELEY_PIN 4
#define T_RELEY_PIN 5
#define SERVO_PIN 12
#define AERATION_PIN 6

#define ONE_HOUR 60000 // минута
#define THREE_HOURS 180000 // 3 минуты
#define THREE_MINUTES 30000; // 30 секунд

#define ONE_HOUR 3600000 // 60 * 60 * 1 * 1000
#define THREE_HOURS 10800000 // 60*60*3*1000
#define THREE_MINUTES 180000; // 60*3*1000

unsigned long aerationStartTime = 0;
unsigned long aerationFinishTime = 0;

unsigned long servoSwitchTime = 0;
bool servoState = false;

void setup()
{
  lcd.begin();
  lcd.noBacklight();
  
  sensor.begin();
  dht.begin();

  aerationStartTime = millis() + THREE_HOURS;
  servoSwitchTime = millis() + ONE_HOUR;
  
  pinMode(H_RELEY_PIN, OUTPUT);
  pinMode(T_RELEY_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(AERATION_PIN, OUTPUT);
}

void loop()
{
  lcd.clear();
  sensor.requestTemperatures();
  float t = sensor.getTempCByIndex(0);
  lcd.print("t:");
  lcd.print(t);
  float h = dht.readHumidity();
  if (!isnan(h)) {
    lcd.setCursor(0, 1);
    lcd.print("h:");
    lcd.print(h);
  }

  int reostatValue = analogRead(A0);

  float targetTemp = TARGET_TEMP + (reostatValue - 512) * ( TEMP_MODULE / 512.0);
  lcd.setCursor(9, 0);
  lcd.print("T:");
  lcd.print(targetTemp);

  if (t < targetTemp) {
    digitalWrite(T_RELEY_PIN, false);
  } else {
    digitalWrite(T_RELEY_PIN, true);
  }

  if (!isnan(h)) {
    if (h < TARGET_HUMIDITY) {
      digitalWrite(H_RELEY_PIN, false);
    } else {
      digitalWrite(H_RELEY_PIN, true);
    }
  }

  lcd.setCursor(9, 1);
  lcd.print("M:");
  lcd.print(millis() / 60000 % 60);

  switchAeration();
  switchServo();
  
  delay(1000);
}

void switchServo()
{
  unsigned long currentMillis = millis();
  if (servoSwitchTime < currentMillis) {
    servoSwitchTime = currentMillis + ONE_HOUR;
    servoState = !servoState;
    digitalWrite(SERVO_PIN, servoState);
  }
}

void switchAeration()
{  
  unsigned long currentMillis = millis();
  if (aerationStartTime < currentMillis) {
    aerationFinishTime = currentMillis + THREE_MINUTES;
    aerationStartTime = aerationFinishTime + THREE_HOURS;
    digitalWrite(AERATION_PIN, false);
  }

  if (aerationFinishTime < currentMillis) {
    digitalWrite(AERATION_PIN, true);
  }
}
