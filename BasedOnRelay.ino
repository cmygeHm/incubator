#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// Set the LCD address to 0x27 for a 16 chars and 2 l+66ine display
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
#define SERVO_INTERVAL 3600000; // 60 * 60 * 1 * 1000 = один час

#define AERATION_PIN 6
#define AERATION_INTERVAL 10800000 // 60*60*3*1000 = 3 часа

unsigned long aerationStartTime = 0;
unsigned long aerationFinishTime = 0;

unsigned long servoSwitchTime = 0;
bool servoState = false;

void setup()
{
  lcd.begin();
  sensor.begin();
  dht.begin();

  aerationStartTime = millis() + AERATION_INTERVAL;
  servoSwitchTime = millis() + SERVO_INTERVAL;

  // Turn on the blacklight and print a message.
  lcd.backlight();
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

  switchAeration();
  switchServo();
  
  delay(1000);
}

void switchServo()
{
  unsigned long currentMillis = millis();
  if (servoSwitchTime < currentMillis) {
    servoSwitchTime = currentMillis + SERVO_INTERVAL;
    servoState = !servoState;
    digitalWrite(SERVO_PIN, servoState);
  }
}

void switchAeration()
{  
  unsigned long currentMillis = millis();
  if (aerationStartTime < currentMillis) {
    aerationFinishTime = currentMillis + 180000; // 60*3*1000 = 3 минуты
    aerationStartTime = aerationFinishTime + AERATION_INTERVAL;
    digitalWrite(AERATION_PIN, false);
  }

  if (aerationFinishTime < currentMillis) {
    digitalWrite(AERATION_PIN, true);
  }
}
