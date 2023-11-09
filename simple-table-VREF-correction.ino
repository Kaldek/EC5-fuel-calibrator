const int Gauge = 5;
const int Sender = A0;
int SenderValue = 51;

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

struct Range {
  int low;
  int high;
  int pwm;
};

const Range ranges[] = {
  {0, 55, 250}, // Full tank
  {56, 88, 249},
  {89, 155, 238},
  {156, 217, 227},
  {218, 268, 216}, // Three quarter
  {269, 284, 204},
  {285, 303, 199},
  {304, 333, 194},
  {334, 358, 190},
  {359, 385, 186}, // Half
  {386, 417, 182},
  {418, 438, 177},
  {439, 458, 176},
  {459, 483, 174},
  {484, 532, 168}, // One Quarter\
  {533, 544, 166},
  {545, 550, 160},
  {551, 563, 154},
  {564, 569, 151},
  {570, 1024, 141} // Empty
};

void setup() {
  analogWrite(Gauge, ranges[0].pwm); // Initial setting using the first range
}

void loop() {
  long vcc = readVcc(); // Read actual Vcc
  int rawValue = analogRead(Sender); // Read raw sensor value
  SenderValue = rawValue * vcc / 1024; // Correct the reading based on actual Vcc

  for (int i = 0; i < sizeof(ranges) / sizeof(ranges[0]); ++i) {
    if (SenderValue >= ranges[i].low && SenderValue <= ranges[i].high) {
      analogWrite(Gauge, ranges[i].pwm);
      break;
    }
  }
}
