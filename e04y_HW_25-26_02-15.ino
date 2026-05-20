#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
// Pins
const int trigPin = 6;    
const int echoPin = 7;    
const int relayPin = 8;   
const int led_pin1 = 9;   //  LED Υψηλής Στάθμης
const int led_pin2 = 10;  // LED Χαμηλής Στάθμης
const int pot_pin1 = A0;  
const int pot_pin2 = A1; 

// Μεταβλητές
int upper_value;
int lower_value; 
int pumpStarts = 0; 
const float tankHeight = 0.18;
 const float tankRadius = 0.11; 
  const float PI_VAL = 3.14159;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led_pin1, OUTPUT);
  pinMode(led_pin2, OUTPUT);
  pinMode(relayPin, OUTPUT); 
  lcd.init();
   lcd.backlight(); 
   lcd.clear();
}

int potentiometro1() {
  int output = analogRead(A0);
  upper_value = map(output, 0, 1023, 0, 22); 
  return upper_value;
}

int potentiometro2() {
  int output2 = analogRead(A1);
  lower_value = map(output2, 0, 1023, 0, 22);
  return lower_value;
}

int ultrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}
float calculateVolume(int distance_cm) { 
  float waterHeight = tankHeight - (distance_cm / 100.0);
   if (waterHeight < 0) waterHeight = 0; 

   if (waterHeight > tankHeight) waterHeight = tankHeight;

   return PI_VAL * tankRadius * tankRadius * waterHeight; }

void loop() {
  int cur_dist = ultrasonic();
  int high_limit = potentiometro1(); // Πάνω όριο 
  int low_limit = potentiometro2();  // Κάτω όριο 

  Serial.print("Dist:"); Serial.print(cur_dist);
  Serial.print(" | High Lim:"); Serial.print(high_limit);
  Serial.print(" | Low Lim:"); Serial.println(low_limit);
 // LED 1
  if (cur_dist <= high_limit) {
    digitalWrite(led_pin1, HIGH);
    digitalWrite(relayPin, LOW); // Κλείσε 
  } else {
    digitalWrite(led_pin1, LOW);
  }

  // LED 2
  if (cur_dist >= low_limit) {
    digitalWrite(led_pin2, HIGH);
    digitalWrite(relayPin, HIGH); // Ξεκίνα 
  } else {
    digitalWrite(led_pin2, LOW);
  }
  if (cur_dist < low_limit && cur_dist > high_limit ) 
  {
    digitalWrite(led_pin1 , LOW);
    digitalWrite(led_pin2 , LOW);
  }
  float volume_m3 = calculateVolume(cur_dist);
  float volume_L = volume_m3 * 1000.0;

  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print("Level:");
  lcd.print(cur_dist); 
  lcd.print("cm");
  lcd.setCursor(0, 1);
  lcd.print(volume_L, 2); 
  lcd.print(" L");

  delay(500);
}

