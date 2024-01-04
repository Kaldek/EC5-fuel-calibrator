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
const int numSamples = 50; // Number of samples to average. Adjust as desired, noting more samples takes longer.

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
      // Remove line below if not using ANSI terminal for serial monitor
      Serial.print("\033[2J\033[H"); // Clear screen and move cursor to home position in PuTTY
      Serial.println("Button pressed - Collecting samples for 10 seconds");
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
      long sumA0 = 0;
      long sumA1 = 0;
      int rawA0Lowest = 1023;
      int rawA0Highest = 0;
      int rawA1Lowest = 1023;
      int rawA1Highest = 0;

      for (int i = 0; i < numSamples; i++) {
        int rawA0 = analogRead(A0);
        int rawA1 = analogRead(A1);

        sumA0 += rawA0;
        sumA1 += rawA1;

        if (rawA0 < rawA0Lowest) rawA0Lowest = rawA0;
        if (rawA0 > rawA0Highest) rawA0Highest = rawA0;
        if (rawA1 < rawA1Lowest) rawA1Lowest = rawA1;
        if (rawA1 > rawA1Highest) rawA1Highest = rawA1;

        delay(10); // Small delay between readings
      }

      // Calculate the averages
      float averageA0 = (float)sumA0 / numSamples;
      float averageA1 = (float)sumA1 / numSamples;

      // Update the lowest and highest if the current average is a new low or high
      if (averageA0 < MainRheostatLowest) MainRheostatLowest = averageA0;
      if (averageA0 > MainRheostatHighest) MainRheostatHighest = averageA0;
      if (averageA1 < UpperRheostatLowest) UpperRheostatLowest = averageA1;
      if (averageA1 > UpperRheostatHighest) UpperRheostatHighest = averageA1;
    } else {
      Serial.println();
      Serial.println("-------------------------------------------------");
      Serial.println("New Data");
      Serial.println();
      Serial.print("Upper Rheostat Lowest: ");
      Serial.println(UpperRheostatLowest * (ReferenceVoltage / 1023.0));
      
      Serial.print("Upper Rheostat Highest: ");
      Serial.println(UpperRheostatHighest * (ReferenceVoltage / 1023.0));     
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
