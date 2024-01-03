const int buttonPin = 2;
const int LowFuelSwitch = 3;
const int ledPin = 13;

int buttonState = 0;        
int lastButtonState = HIGH;     
int switchState = 0;

unsigned long startMillis; 
unsigned long currentMillis;
const unsigned long period = 10000;
bool reading = false;
int UpperRheostatLowest, UpperRheostatHighest, MainRheostatLowest, MainRheostatHighest;

// Constants for fixed resistors in the voltage divider
const float UpperRheostatFixedResistor = 33.0; // Ohms
const float MainRheostatFixedResistor = 22.0;  // Ohms
const float ReferenceVoltage = 4.96; // Nano Every +5V output is 4.96 volts

// Set up averaging for reading the ADC pins
const int numSamples = 10; // Number of samples to average. Adjust as desired, noting more samples takes longer.

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LowFuelSwitch, INPUT);  
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  switchState = digitalRead(LowFuelSwitch);
  int rawA0;
  int rawA1;
  
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
      
      if (rawA1 < UpperRheostatLowest) UpperRheostatLowest = rawA1;
      if (rawA1 > UpperRheostatHighest) UpperRheostatHighest = rawA1;
      
      if (rawA0 < MainRheostatLowest) MainRheostatLowest = rawA0;
      if (rawA0 > MainRheostatHighest) MainRheostatHighest = rawA0;      
    } else {
      Serial.print("Upper Rheostat Lowest: ");
      Serial.println(UpperRheostatLowest * (ReferenceVoltage / 1023.0));
      
      Serial.print("Upper Rheostat Highest: ");
      Serial.println(UpperRheostatHighest * (ReferenceVoltage / 1023.0));
      Serial.print("Upper Rheostat Raw: ");
      Serial.println(analogRead(A1));
            
      // Calculate and print the derived resistance of the Upper rheostat
      float upperRheostatResistance = UpperRheostatLowest * (ReferenceVoltage / 1023.0) / (ReferenceVoltage - UpperRheostatLowest * (ReferenceVoltage / 1023.0)) * UpperRheostatFixedResistor;
      Serial.print("Derived Upper Rheostat Resistance: ");
      Serial.println(upperRheostatResistance);
      
      Serial.print("Main Rheostat Lowest: ");
      Serial.println(MainRheostatLowest * (ReferenceVoltage / 1023.0));
      Serial.print("Main Rheostat Highest: ");
      Serial.println(MainRheostatHighest * (ReferenceVoltage / 1023.0));
      
      float mainRheostatResistance = MainRheostatLowest * (ReferenceVoltage / 1023.0) / (ReferenceVoltage - MainRheostatLowest * (ReferenceVoltage / 1023.0)) * MainRheostatFixedResistor;
      Serial.print("Derived Main Rheostat Resistance: ");
      Serial.println(mainRheostatResistance);
      
      Serial.print("Low Fuel Switch State: ");
      if (switchState == LOW) {
        Serial.println("OPEN");
      } else {
        Serial.println("CLOSED");
      }

      reading = false;
      digitalWrite(ledPin, LOW);
    }
  }
}
