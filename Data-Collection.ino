const int buttonPin = 2;
const int LowFuelSwitch = 3;
const int ledPin = 13;

int buttonState = 0;        
int lastButtonState = 0;     
int switchState = 0;

unsigned long startMillis; 
unsigned long currentMillis;
const unsigned long period = 10000;
bool reading = false;
int UpperRheostatLowest, UpperRheostatHighest, MainRheostatLowest, MainRheostatHighest;
float analog0, analog1;

// Constants for fixed resistors in the voltage divider
const float UpperRheostatFixedResistor = 33.0; // Ohms
const float MainRheostatFixedResistor = 22.0;  // Ohms

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LowFuelSwitch, INPUT_PULLUP);  
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  switchState = digitalRead(LowFuelSwitch);

  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      Serial.println("Button pressed");
      digitalWrite(ledPin, HIGH);
      startMillis = millis();
      reading = true;
      UpperRheostatLowest = 1023;
      UpperRheostatHighest = 0;
      MainRheostatLowest = 1023;
      MainRheostatHighest = 0;      
    }
    delay(50);
  }
  
  lastButtonState = buttonState;

  if (reading) {
    currentMillis = millis();

    if (currentMillis - startMillis <= period) {
      int rawA0 = analogRead(A0);
      int rawA1 = analogRead(A1);
      analog0 = rawA0 * 5.0 / 1024;
      analog1 = rawA1 * 5.0 / 1024;
      
      if (analog0 < UpperRheostatLowest) UpperRheostatLowest = analog0;
      if (analog0 > UpperRheostatHighest) UpperRheostatHighest = analog0;
      if (analog1 < MainRheostatLowest) MainRheostatLowest = analog1;
      if (analog1 > MainRheostatHighest) MainRheostatHighest = analog1;      
    } else {
      Serial.print("Upper Rheostat Lowest: ");
      Serial.println(UpperRheostatLowest * (5.0 / 1023.0));
      Serial.print("Upper Rheostat Highest: ");
      Serial.println(UpperRheostatHighest * (5.0 / 1023.0));
      // Calculate and print the derived resistance of the Upper rheostat
      float upperRheostatResistance = UpperRheostatHighest * (5.0 / 1023.0) / (5.0 - UpperRheostatHighest * (5.0 / 1023.0)) * UpperRheostatFixedResistor;
      Serial.print("Derived Upper Rheostat Resistance: ");
      Serial.println(upperRheostatResistance);
      
      Serial.print("Main Rheostat Lowest: ");
      Serial.println(MainRheostatLowest * (5.0 / 1023.0));
      Serial.print("Main Rheostat Highest: ");
      Serial.println(MainRheostatHighest * (5.0 / 1023.0));
      
      float mainRheostatResistance = MainRheostatHighest * (5.0 / 1023.0) / (5.0 - MainRheostatHighest * (5.0 / 1023.0)) * MainRheostatFixedResistor;
      Serial.print("Derived Main Rheostat Resistance: ");
      Serial.println(mainRheostatResistance);
      
      Serial.print("Low Fuel Switch State: ");
      if (switchState == HIGH) {
        Serial.println("OPEN");
      } else {
        Serial.println("CLOSED");
      }

      reading = false;
      digitalWrite(ledPin, LOW);
    }
  }
}
