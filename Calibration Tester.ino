#include <float.h>

// Set PWM output pin for fuel gauge
const int Gauge = 5;
const int PrimarySender = A0;
const int SecondarySender = A1;

// Constants for fixed resistors in the voltage divider
const float UpperRheostatFixedResistor = 33.0; // Ohms
const float MainRheostatFixedResistor = 22.0;  // Ohms
const float ReferenceVoltage = 4.96; // Nano Every +5V output is 4.96 volts

// Set up averaging for reading the ADC pins
const int numSamples = 50; // Number of samples to average. Adjust as desired, noting more samples takes longer.


// Set up the fuel level ranges
struct FuelLevelRange {
    int level;
    float low;
    float high;
};

// Initialize an array of FuelLevelRange structures for Primary and Secondary sources.
FuelLevelRange primaryFuelLevelRanges[] = {
    {0, 3.18, 3.21},
    {4, 3.15, 3.16},
    {5, 3.11, 3.12},
    {6, 3.08, 3.09},
    {7, 3.06, 3.07},
    {8, 3.04, 3.04},
    {9, 2.99, 3.03},
    {10, 2.96, 2.97},
    {11, 2.91, 2.92},
    {12, 2.88, 2.89},
    {13, 2.82, 2.83},
    {14, 2.77, 2.78},
    {15, 2.71, 2.72},
    {16, 2.65, 2.66},
    {17, 2.53, 2.55},
    {18, 2.48, 2.49},
    {19, 2.42, 2.42},
    {20, 2.34, 2.35},
    {21, 2.22, 2.25},
    {22, 2.05, 2.08},
    {23, 1.99, 2.00},
    {24, 1.84, 1.86},
    {25, 1.73, 1.74},
    {26, 1.66, 1.70},
    {27, 1.61, 1.63},
    {28, 1.56, 1.58},
    {29, 1.45, 1.47},
    {30, 1.42, 1.44},
    {31, 1.36, 1.41},
    {32, 1.18, 1.25}
};

FuelLevelRange secondaryFuelLevelRanges[] = {
    {0, 3.15, 3.16},
    {4, 3.07, 3.09},
    {5, 3.05, 3.06},
    {6, 3.02, 3.03},
    {7, 2.97, 2.98},
    {8, 2.92, 2.93},
    {9, 2.87, 2.88},
    {10, 2.81, 2.83},
    {11, 2.77, 2.80},
    {12, 2.68, 2.69},
    {13, 2.65, 2.66},
    {14, 2.57, 2.59},
    {15, 2.48, 2.49},
    {16, 2.43, 2.44},
    {17, 2.36, 2.37},
    {18, 2.24, 2.26},
    {19, 2.22, 2.23},
    {20, 2.10, 2.12},
    {21, 2.04, 2.04},
    {22, 1.87, 1.89},
    {23, 1.84, 1.86},
    {24, 1.61, 1.63},
    {25, 1.45, 1.48},
    {26, 1.42, 1.44},
    {27, 1.37, 1.38},
    {28, 1.29, 1.30},
    {29, 1.21, 1.13},
    {30, 1.01, 1.03},
    {31, 0.79, 0.80},
    {32, 0.50, 0.52}
};

struct GaugeRange {
  int LowestLitre;
  int HighestLitre;
  int pwm;
};

const GaugeRange GaugeRanges[] = {
  {0, 7, 141},
  {8, 9, 151},
  {10, 12, 160},
  {13, 14, 166},
  {15, 15, 168},
  {16, 18, 174},
  {19, 21, 176},
  {22, 24, 177},
  {25, 27, 182},
  {28, 30, 186},
  {31, 33, 190},
  {34, 36, 194},
  {37, 39, 199},
  {40, 42, 204},
  {43, 45, 216},
  {46, 49, 227},
  {50, 53, 238},
  {54, 57, 249},
  {58, 64, 250}
};

const int numPrimaryLevels = sizeof(primaryFuelLevelRanges) / sizeof(FuelLevelRange);
const int numSecondaryLevels = sizeof(secondaryFuelLevelRanges) / sizeof(FuelLevelRange);

// Function to determine the closest fuel level based on reading and a range array.
int getClosestFuelLevelFromReading(float reading, FuelLevelRange ranges[], int numRanges) {
    float closestDiff = FLT_MAX;
    int closestLevel = -1;
    for (int i = 0; i < numRanges; i++) {
        if (reading >= ranges[i].low && reading <= ranges[i].high) {
            return ranges[i].level; // Exact match found
        } else {
            // Find the range with the smallest difference to the reading
            float diff = min(abs(reading - ranges[i].low), abs(reading - ranges[i].high));
            if (diff < closestDiff) {
                closestDiff = diff;
                closestLevel = ranges[i].level;
            }
        }
    }
    return closestLevel; // Return the closest level if no exact match is found
}


// Function to determine the total fuel level based on primary and secondary readings.
int getTotalFuelLevel(float primaryReading, float secondaryReading) {
    int primaryFuelLevel = getClosestFuelLevelFromReading(primaryReading, primaryFuelLevelRanges, numPrimaryLevels);
    int secondaryFuelLevel = getClosestFuelLevelFromReading(secondaryReading, secondaryFuelLevelRanges, numSecondaryLevels);
    
	return primaryFuelLevel + secondaryFuelLevel;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
    
  float primaryReading = analogRead(PrimarySender);
  float secondaryReading = analogRead(SecondarySender);

  // Get the total fuel level for the given readings
  int fuelLevel = getTotalFuelLevel(primaryReading, secondaryReading);
  
  // Drive the Fuel Gauge using PWM based on the level of fuel in the tank
  for (int i = 0; i < sizeof(GaugeRanges) / sizeof(GaugeRanges[0]); ++i) {
    if (fuelLevel >= GaugeRanges[i].LowestLitre && fuelLevel <= GaugeRanges[i].HighestLitre) {
      analogWrite(Gauge, GaugeRanges[i].pwm);
      break;
    }
  }  

  // Output the result
  Serial.print("The total fuel level is: ");
  Serial.println(fuelLevel);
  delay(5000);
  
}
