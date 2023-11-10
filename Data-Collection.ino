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
int UpperRheostatLowest, UpperRheostatHighest, MainRheostatLowest, MainRheostatHighest, LowFuelRheostatLowest, LowFuelRheostatHighest;
float analog0, analog1, analog2, vcc;

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Calculate Vcc (in mV); 1126400 = 1.1*1024*1000
  return result;
}

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
      LowFuelRheostatLowest = 1023;
      LowFuelRheostatHighest = 0;
    }
    delay(50);
  }
  
  lastButtonState = buttonState;

  if (reading) {
    currentMillis = millis();

    if (currentMillis - startMillis <= period) {
      long vccMillivolts=readVcc();
      float vcc = vccMillivolts / 1000.0 // Convert Vcc to volts
      int rawA0 = analogRead(A0);
      int rawA1 = analogRead(A1);
      int rawA2 = analogRead(A2);
      analog0 = rawA0 * vcc / 1024;
      analog1 = rawA1 * vcc / 1024;
      analog2 = rawA2 * vcc / 1024;
      

      if (analog0 < UpperRheostatLowest) UpperRheostatLowest = analog0;
      if (analog0 > UpperRheostatHighest) UpperRheostatHighest = analog0;
      if (analog1 < MainRheostatLowest) MainRheostatLowest = analog1;
      if (analog1 > MainRheostatHighest) MainRheostatHighest = analog1;
      if (analog2 < LowFuelRheostatLowest) LowFuelRheostatLowest = analog2;
      if (analog2 > LowFuelRheostatHighest) LowFuelRheostatHighest = analog2;
    } else {
      Serial.print("Upper Rheostat Lowest: ");
      Serial.println(UpperRheostatLowest * (vcc / 1023.0));
      Serial.print("Upper Rheostat Highest: ");
      Serial.println(UpperRheostatHighest * (vcc / 1023.0));
      Serial.print("Main Rheostat Lowest: ");
      Serial.println(MainRheostatLowest * (vcc / 1023.0));
      Serial.print("Main Rheostat Highest: ");
      Serial.println(MainRheostatHighest * (vcc / 1023.0));
      Serial.print("Low Fuel Rheostat Lowest: ");  
      Serial.println(LowFuelRheostatLowest * (vcc / 1023.0));
      Serial.print("Low Fuel Rheostat Highest: ");  
      Serial.println(LowFuelRheostatHighest * (vcc / 1023.0));

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
