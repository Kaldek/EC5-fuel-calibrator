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
float analog0, analog1, vcc;

// Constants for fixed resistors in the voltage divider
const float UpperRheostatFixedResistor = 33.0; // Ohms
const float MainRheostatFixedResistor = 22.0;  // Ohms
const float VREF = 5.0; // VREF for Nano Every

// long readVcc() {
//  long result;
  // Read 1.1V reference against AVcc
//  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
//  delay(2); // Wait for Vref to settle
//  ADCSRA |= _BV(ADSC); // Convert
//  while (bit_is_set(ADCSRA, ADSC));
//  result = ADCL;
//  result |= ADCH << 8;
//  result = 1126400L / result; // Calculate Vcc (in mV); 1126400 = 1.1*1024*1000
//  return result;
// }

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(LowFuelSwitch, INPUT);  
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
    //  long vccMillivolts=readVcc();
    //  float vcc = vccMillivolts / 1000.0 // Convert Vcc to volts
      int rawA0 = analogRead(A0);
      int rawA1 = analogRead(A1);
      analog0 = rawA0 * VREF / 1024;
      analog1 = rawA1 * VREF / 1024;
      
      if (analog0 < UpperRheostatLowest) UpperRheostatLowest = analog0;
      if (analog0 > UpperRheostatHighest) UpperRheostatHighest = analog0;
      if (analog1 < MainRheostatLowest) MainRheostatLowest = analog1;
      if (analog1 > MainRheostatHighest) MainRheostatHighest = analog1;      
    } else {
      Serial.print("Upper Rheostat Lowest: ");
      Serial.println(UpperRheostatLowest * (VREF / 1023.0));
      Serial.print("Upper Rheostat Highest: ");
      Serial.println(UpperRheostatHighest * (VREF / 1023.0));
      // Calculate and print the derived resistance of the Upper rheostat
      float upperRheostatResistance = UpperRheostatHighest * (VREF / 1023.0) / (vcc - UpperRheostatHighest * (VREF / 1023.0)) * UpperRheostatFixedResistor;
      Serial.print("Derived Upper Rheostat Resistance: ");
      Serial.println(upperRheostatResistance);
      
      Serial.print("Main Rheostat Lowest: ");
      Serial.println(MainRheostatLowest * (VREF / 1023.0));
      Serial.print("Main Rheostat Highest: ");
      Serial.println(MainRheostatHighest * (VREF / 1023.0));
      
      float mainRheostatResistance = MainRheostatHighest * (VREF / 1023.0) / (vcc - MainRheostatHighest * (VREF / 1023.0)) * MainRheostatFixedResistor;
      Serial.print("Derived Main Rheostat Resistance: ");
      Serial.println(mainRheostatResistance);
      
      Serial.print("Low Fuel Switch State: ");
      if (switchState == HIGH) {
        Serial.println("CLOSED");
      } else {
        Serial.println("OPEN");
      }

      reading = false;
      digitalWrite(ledPin, LOW);
    }
  }
}
