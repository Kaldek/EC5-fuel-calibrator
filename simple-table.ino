const int Gauge = 5;
const int Sender = A0;
int SenderValue = 51 ;

struct Range {
  int low;
  int high;
  int pwm;
};

const Range ranges[] = {
  {0, 55, 250},
  {56, 88, 249},
  {89, 155, 238},
  {156, 217, 227},
  {218, 268, 216},
  {269, 284, 204},
  {285, 303, 199},
  {304, 333, 194},
  {334, 358, 190},
  {359, 385, 186},
  {386, 417, 182},
  {418, 438, 177},
  {439, 458, 176},
  {459, 483, 174},
  {484, 532, 168},
  {533, 544, 166},
  {545, 550, 160},
  {551, 563, 154},
  {564, 569, 151},
  {570, 1024, 141}
};

void setup() {
  // Initial analogWrite to respond to the initial SenderValue
  for (int i = 0; i < sizeof(ranges) / sizeof(ranges[0]); ++i) {
    if (SenderValue >= ranges[i].low && SenderValue <= ranges[i].high) {
      analogWrite(Gauge, ranges[i].pwm);
      break;
    }
  }
}
void loop() {
  for (int i = 0; i < sizeof(ranges) / sizeof(ranges[0]); ++i) {
    if (SenderValue >= ranges[i].low && SenderValue <= ranges[i].high) {
      analogWrite(Gauge, ranges[i].pwm);
      break;
    }
  }
}
