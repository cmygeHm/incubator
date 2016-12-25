#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Servo.h>
#include <Thread.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

OneWire oneWireTemperature(2);

DallasTemperature sensor(&oneWireTemperature);

DHT dht(3, DHT22);

#define TARGET_TEMP 27
#define TARGET_HUMIDITY 70

#define H_RELEY_PIN 4
#define T_RELEY_PIN 5

Servo servo; 

int minDegree = 6;
int maxDegree = 150;

Thread myThread = Thread();

void setup()
{
  
  lcd.begin();
  sensor.begin();
  dht.begin();

	// Turn on the blacklight and print a message.
	lcd.backlight();
  pinMode(H_RELEY_PIN, OUTPUT);
  pinMode(T_RELEY_PIN, OUTPUT);
  servo.attach(12);
  servo.write(minDegree);
  
  myThread.onRun(pushServo);
  myThread.setInterval(500);
}

void loop()
{

  if(myThread.shouldRun())
  myThread.run();
  
  lcd.clear();
	sensor.requestTemperatures();
  float t = sensor.getTempCByIndex(0);
  lcd.print("T: ");
  lcd.print(t);
  float h = dht.readHumidity();
  if (!isnan(h)) {
    lcd.setCursor(0, 1);
    lcd.print("H: ");
    lcd.print(h);
    
  }

  if (t < TARGET_TEMP) {
    digitalWrite(T_RELEY_PIN, true);
  } else {
    digitalWrite(T_RELEY_PIN, false);
  }

  if (!isnan(h)) {
    if (h < TARGET_HUMIDITY) {
      digitalWrite(H_RELEY_PIN, true);
    } else {
      digitalWrite(H_RELEY_PIN, false);
    }
  }  

  delay(2000);
}

void pushServo()
{
  int currentDegree = servo.read();
  static int clockwise = 5;
  if (currentDegree > maxDegree) {    
    clockwise = -5;
  }
  
  if (currentDegree < minDegree) {
    clockwise = 5;
  }

  servo.write(currentDegree + clockwise); 
}
