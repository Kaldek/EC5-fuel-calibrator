#include <float.h>

// Set PWM output pin for fuel gauge and light
const int Gauge = 5;
const int Light = 6;
const int PrimarySender = A1;
const int SecondarySender = A0;
const float ReferenceVoltage = 4.99; // Nano Every +5V output
const int LowFuelSwitch = 3;
int switchState = 0;

int lightFlag;

// Set up the fuel level ranges
struct FuelLevelRange {
    int level;
    float low;
    float high;
};

unsigned long lastUpdateTime = 0; // Stores the last time lightFlag was updated
int lastFuelLevel = -1; // Stores the last fuel level to detect changes
const long interval = 5000; // 5 seconds interval

// EMA variables
float emaFuelLevel = 0; // Exponential Moving Average of the fuel level
const float alpha = 0.15; // Smoothing factor (adjust based on consumption rate)
bool emaInitialized = false; // Flag for initial EMA setup

// Initialize an array of FuelLevelRange structures for Primary and Secondary sources.
FuelLevelRange primaryFuelLevelRanges[] = {
    {0, 3.33, 5.00},
    {4, 3.29, 3.32},
    {6, 3.22, 3.28},
    {8, 3.13, 3.21},
    {10, 3.06, 3.12},
    {12, 2.99, 3.05},
    {14, 2.81, 2.98},
    {16, 2.69, 2.80},
    {18, 2.57, 2.70},
    {20, 2.40, 2.56},
    {22, 2.20, 2.39},
    {24, 2.02, 2.19},
    {26, 1.57, 2.01},
    {28, 1.20, 1.56},
    {30, 0.74, 1.19},
    {32, 0.39, 0.73}
};

FuelLevelRange secondaryFuelLevelRanges[] = {
    {0, 3.25, 5.00},
    {4, 3.21, 3.24},
    {6, 3.12, 3.20},
    {8, 2.98, 3.11},
    {10, 2.86, 2.99},
    {12, 2.72, 2.87},
    {14, 2.57, 2.73},
    {16, 2.38, 2.58},
    {18, 2.19, 2.39},
    {20, 1.88, 2.20},
    {22, 1.68, 1.87},
    {24, 1.40, 1.69},
    {26, 1.20, 1.39},
    {28, 0.44, 1.19}
};

struct GaugeRange {
  int LowestLitre;
  int HighestLitre;
  int pwm;
};

// Fuel Gauge needle position table
const GaugeRange GaugeRanges[] = {
  {0, 3, 141},  // EMPTY (0-3 litres in tank)
  {4, 6, 151},
  {8, 10, 160},
  {12, 14, 166},
  {16, 18, 172},  // Quarter tank
  {20, 22, 176},
  {24, 26, 178},
  {28, 32, 183},  // Half tank
  {34, 36, 194},
  {38, 40, 199},
  {42, 44, 204},
  {46, 48, 208},  // Three Quarter tank
  {50, 52, 215},
  {54, 56, 224},
  {58, 60, 246}   // FULL
};

const int numPrimaryLevels = sizeof(primaryFuelLevelRanges) / sizeof(FuelLevelRange);
const int numSecondaryLevels = sizeof(secondaryFuelLevelRanges) / sizeof(FuelLevelRange);

int getFuelLevelFromReading(float reading, FuelLevelRange ranges[], int numRanges) {
    for (int i = 0; i < numRanges; i++) {
        if (reading >= ranges[i].low && reading <= ranges[i].high) {
            return ranges[i].level; // Return as soon as a match is found
        }
    }
    return ranges[numRanges - 1].level; // Return the last range's level as default
}

void setup() {
  pinMode(LowFuelSwitch, INPUT);  
  pinMode(Light, OUTPUT);
  digitalWrite(Light, HIGH);
  analogWrite(Gauge, 255);
  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    // Do nothing, just wait for 10 seconds
    // You can add code here if needed in the future
  }
  // Serial.begin(9600);
}

void loop() {
  switchState = digitalRead(LowFuelSwitch);
  analogRead(PrimarySender);
  delayMicroseconds(20);
  int primaryRawReading = analogRead(PrimarySender);
  delayMicroseconds(20);
  int secondaryRawReading = analogRead(SecondarySender);
  float primaryReading = primaryRawReading * (ReferenceVoltage / 1023.0);
  float secondaryReading = secondaryRawReading * (ReferenceVoltage / 1023.0);
  

  // Round the results to two decimal places
  primaryReading = round(primaryReading * 100) / 100.0;
  secondaryReading = round(secondaryReading * 100) / 100.0;

   /*
    Serial.print("\033[2J\033[H");
    Serial.print("Main: ");
    Serial.println(primaryReading);
    Serial.print("Secondary: ");
    Serial.println(secondaryReading);
   */

  int primaryFuelLevel = getFuelLevelFromReading(primaryReading, primaryFuelLevelRanges, numPrimaryLevels);
  int secondaryFuelLevel = getFuelLevelFromReading(secondaryReading, secondaryFuelLevelRanges, numSecondaryLevels);
  /*
  Serial.print("Main Fuel: ");
    Serial.println(primaryFuelLevel);
    Serial.print("Secondary Fuel: ");
    Serial.println(secondaryFuelLevel);
  */
  // Get the combined fuel level
  int currentFuelLevel = primaryFuelLevel + secondaryFuelLevel;
  /*
    Serial.print("Total Fuel: ");
    Serial.println(currentFuelLevel);
     delay(150);
  */
  // EMA calculation
  if (!emaInitialized) {
    emaFuelLevel = currentFuelLevel; // Initialize EMA with the first reading
    emaInitialized = true;
  } else {
    emaFuelLevel = (alpha * currentFuelLevel) + ((1 - alpha) * emaFuelLevel);
  }

  // Cast EMA to an integer for actual gauge/lights logic
  int fuelLevel = int(emaFuelLevel);

  unsigned long currentTime = millis();

  // Update light flag every interval
  if ((fuelLevel != lastFuelLevel) && (currentTime - lastUpdateTime >= interval)) {
    if ((fuelLevel <= 11) && (switchState == LOW)) {
      lightFlag = 1; // Steady on
    } else {
      lightFlag = 0; // Light Off
    }
    lastFuelLevel = fuelLevel;
    lastUpdateTime = currentTime;
  }

   
  // Handle the light based on the lightFlag
  switch (lightFlag) {
    case 1:
      digitalWrite(Light, HIGH); break;
    
    default:
      digitalWrite(Light, LOW); break;
  }

  // Set the gauge needle position based on smoothed fuel level
  for (int i = 0; i < sizeof(GaugeRanges) / sizeof(GaugeRanges[0]); ++i) {
    if (fuelLevel >= GaugeRanges[i].LowestLitre && fuelLevel <= GaugeRanges[i].HighestLitre) {
      analogWrite(Gauge, GaugeRanges[i].pwm);
      /*
      Serial.print("Gauge write: ");
      Serial.println(GaugeRanges[i].pwm);
      delay(150);
      */
      break;
    }
  }
}
