#include <float.h>

// Set PWM output pin for fuel gauge
const int Gauge = 5;
const int Light = 6;
const int PrimarySender = A1;
const int SecondarySender = A0;
const float ReferenceVoltage = 4.99; // Nano Every +5V output

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

// Initialize an array of FuelLevelRange structures for Primary and Secondary sources.
// Both sources use ADC integers converted to voltage values.  This allows for alteration of the reference voltage without further changes to the code.
FuelLevelRange primaryFuelLevelRanges[] = {
    {0, 3.18, 5.10},
    {4, 3.15, 3.17},
    {5, 3.11, 3.14},
    {6, 3.08, 3.10},
    {7, 3.06, 3.07},
    {8, 3.04, 3.05},
    {9, 2.99, 3.03},
    {10, 2.96, 2.98},
    {11, 2.91, 2.95},
    {12, 2.88, 2.90},
    {13, 2.82, 2.87},
    {14, 2.77, 2.81},
    {15, 2.71, 2.76},
    {16, 2.65, 2.70},
    {17, 2.53, 2.64},
    {18, 2.48, 2.52},
    {19, 2.42, 2.47},
    {20, 2.34, 2.41},
    {21, 2.22, 2.33},
    {22, 2.05, 2.21},
    {23, 1.99, 2.04},
    {24, 1.84, 1.98},
    {25, 1.73, 1.83},
    {26, 1.66, 1.72},
    {27, 1.61, 1.65},
    {28, 1.56, 1.60},
    {29, 1.45, 1.55},
    {30, 1.42, 1.44},
    {31, 1.18, 1.41},
    {32, 0.00, 1.17}
};

FuelLevelRange secondaryFuelLevelRanges[] = {
    {0, 3.16, 5.10},
    {4, 3.07, 3.15},
    {5, 3.05, 3.06},
    {6, 3.02, 3.04},
    {7, 2.97, 3.01},
    {8, 2.92, 2.98},
    {9, 2.87, 2.91},
    {10, 2.81, 2.86},
    {11, 2.77, 2.80},
    {12, 2.68, 2.76},
    {13, 2.65, 2.67},
    {14, 2.57, 2.64},
    {15, 2.48, 2.56},
    {16, 2.43, 2.47},
    {17, 2.36, 2.42},
    {18, 2.24, 2.35},
    {19, 2.22, 2.23},
    {20, 2.10, 2.21},
    {21, 2.04, 2.09},
    {22, 1.87, 2.03},
    {23, 1.84, 1.86},
    {24, 1.61, 1.83},
    {25, 1.45, 1.60},
    {26, 1.42, 1.44},
    {27, 1.37, 1.41},
    {28, 1.29, 1.36},
    {29, 1.21, 1.30},
    {30, 1.01, 1.20},
    {31, 0.79, 1.00},
    {32, 0.00, 0.78}
};

struct GaugeRange {
  int LowestLitre;
  int HighestLitre;
  int pwm;
};

// Fuel Gauge needle position table
const GaugeRange GaugeRanges[] = {
  {0, 7, 141},  // EMPTY (0-7 litres in tank)
  {8, 9, 151},
  {10, 12, 160},
  {13, 14, 166},
  {15, 15, 171},  // Quarter tank
  {16, 18, 174},
  {19, 21, 176},
  {22, 24, 177},
  {25, 27, 181},
  {28, 30, 183},  // Half tank
  {31, 33, 190},
  {34, 36, 194},
  {37, 39, 199},
  {40, 42, 204},
  {43, 45, 208},  // Three Quarter tank
  {46, 49, 227},
  {50, 53, 238},
  {54, 57, 249},
  {58, 64, 250}   // FULL (58-64 litres)
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
  pinMode(Light, OUTPUT);
  digitalWrite(Light, HIGH);
  analogWrite(Gauge,186);
  delay(1000);
  // Enable below if debugging
  // Serial.begin(9600);
}

void loop() {
  int primaryRawReading = analogRead(PrimarySender);
  int secondaryRawReading = analogRead(SecondarySender);
  float primaryReading = primaryRawReading * (ReferenceVoltage / 1023.0);
  float secondaryReading = secondaryRawReading * (ReferenceVoltage / 1023.0);
  // Round the results to two decimal places
  primaryReading = round(primaryReading * 100) / 100.0;
  secondaryReading = round(secondaryReading * 100) / 100.0;

  int primaryFuelLevel, secondaryFuelLevel;

  // Get the total fuel level for the given readings
  primaryFuelLevel = getFuelLevelFromReading(primaryReading, primaryFuelLevelRanges, numPrimaryLevels);
  secondaryFuelLevel = getFuelLevelFromReading(secondaryReading, secondaryFuelLevelRanges, numSecondaryLevels);
  
  int fuelLevel = primaryFuelLevel + secondaryFuelLevel;

  unsigned long currentTime = millis();

// Check if the fuel level has changed and at least 5 seconds have passed
  if ((fuelLevel != lastFuelLevel) && (currentTime - lastUpdateTime >= interval)) {
    // Update light flag based on fuel level
    if (fuelLevel <= 6) {
      lightFlag = 3; // Light fast flash
    } else if (fuelLevel <= 8) {
      lightFlag = 2; // Light slow flash
    } else if (fuelLevel <= 11) {
      lightFlag = 1; // Light steady on
    } else {
      lightFlag = 0; // Light off
    }

    // Update the last fuel level and last update time
    lastFuelLevel = fuelLevel;
    lastUpdateTime = currentTime;
  }

// Control light based on lightFlag
  switch (lightFlag) {
    case 1: // Solid light
      digitalWrite(Light, HIGH);
      break;
    case 2: // Slow flashing light
      digitalWrite(Light, HIGH);
      delay(600);
      digitalWrite(Light, LOW);
     delay(600);
      break;
    case 3: // Fast flashing light
      digitalWrite(Light, HIGH);
      delay(200);
      digitalWrite(Light, LOW);
      delay(200);
      break;
   default:
      digitalWrite(Light, LOW);
  }

  // Drive the Fuel Gauge using PWM based on the level of fuel in the tank
  for (int i = 0; i < sizeof(GaugeRanges) / sizeof(GaugeRanges[0]); ++i) {
   if (fuelLevel >= GaugeRanges[i].LowestLitre && fuelLevel <= GaugeRanges[i].HighestLitre) {
   analogWrite(Gauge, GaugeRanges[i].pwm);
   // break;
    }
  
  // Enable the below if debugging
/*
  Serial.print("\033[2J\033[H");
  Serial.print("The total fuel level is: ");
  Serial.println(fuelLevel);
  Serial.print("Main Rheostat Fuel Level:");
  Serial.println(primaryFuelLevel);
  Serial.print("Secondary Rheostat Fuel Level:");
  Serial.println(secondaryFuelLevel);
  delay(1000);
*/ 
  }  
}
