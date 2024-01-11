# EC5-fuel-calibrator
Fuel Gauge recalibration project for the 8th-generation Mitsubishi Galant and Legnum VR4 (chassis codes EC5A and EC5W).  This calibration circuitry provides litre-accurate fuel measurements which are then used to drive the fuel gauge and low fuel light.

Likely, the project will also function for the Lancer Evolution, models 1 through 9 (I through IX).  The project currently uses an [Arduino Nano Every](https://docs.arduino.cc/hardware/nano-every) coupled with a custom PCB shield.  The Nano Every has been selected as this board has its own on-board [monolithic power supply chip](https://www.monolithicpower.com/en/mpm3610.html) which reduces the number of components required on the PCB and works well in automotive environments.  It is also capable of providing 1.2 amps on its +5v output thanks to the power chip, which exceeds the capabilities of an LM7805 linear voltage regulator.

## Issues with the stock fuel gauge circuitry
The stock fuel gauge on these cars is extremely inaccurate due to the use of a simple *linear* analogue gauge that reacts to a change in resistance of two rheostats (upper and lower) in the fuel tank which are wired in-series.  The entire setup of the rheostats in series is a "quick and dirty" attempt to use the same fuel gauge as on the 2WD models of these cars, as those vehicles use a single rheostat with a range of 4-112 ohms.  The rheostats on the VR4 range from 2-41.5 ohms and 2-65.5 ohms, respectively, for a combined total of ~107 ohms in an attempt to make the gauge work.  

The combination of the following two design decisions results in *non-linear* fuel levels from the fuel senders:
- Rheostats in Series
- Saddle tank design

### Rheostats in series
The two rheostats in the fuel tank are labelled as "main" and "secondary" in the Mitsubishi wiring diagrams, which refers to the upper and lower rheostat.  There are upper and lower fuel sender rheostats because the fuel tank is a "couch" design where when viewed from side-on, the fuel tank has the shape of a couch, with a flat lower section and an almost vertical upper section.  The fuel tank can be described as saddling the driveshaft, whilst also angling up beneath the base of the rear passenger seats.  The rheostats are physically located in the right locations and individually can provide accurate measurement, but when wired in series their combined resistance behaves non-linearly due to their different resistance ranges.

#### Upper rheostat
The upper rheostat has full travel between the bottom of the tank (on the left side of the saddle) and the top of the tank.  There is overlap of data between the two rheostats, and this overlap further messes with the attempt to use twho rheostats to mimic the behaviour of one rheostat (as used on the 2WD cars).

### Saddle tank design
The VR4 fuel tank uses a saddle design where the middle of the tank has a "hump" in the bottom to allow for the driveshaft to reach the rear differential.  The right side of the saddle design has the fuel pump and one of the fuel sender rheostats. The left side of the saddle has the low fuel switch, the "upper" rheostat and the pickup for the jet pipe.

The saddle is confirmed to overflow at different levels for each side of the tank (29 litres for left side, 27 for the right side).

#### Jet pipe
To avoid fuel starvation and fuel being stuck in the side of the tank where there is no fuel pump, the fuel delivery system makes use of a "jet pipe" (venturi pump) located in the left side of the tank.  As fuel returns to the tank from the fuel pressure rail (after the fuel pressure regulator) the returning fuel passes by a venturi which connects to the jet pipe.  The vaccum generated by this venturi does what it can to constantly keep pumping fuel back over from the left side of the tank into the side where the fuel pump is.  Hard turns to the right when cornering can cause all this work to be undone as the fuel sloshes back over the saddle to the left side of the tank. Note that we do not know the flow rate of the jet pipe so it is difficult to determine how **much** of a problem this causes.  

*It is known that on high powered AWD Mitsubishi Lancers, the return line that passes by the venturi can be too restrictive, causing excessive fuel pressures in the fuel rail (e.g., 60 psi at idle).  Drilling out this line is a common change, but the venturi must not be over-drilled as this will stop the jet pipe from functioning and cause fuel starvation at the pump.*

### Low fuel light
The low fuel light circuit is separate from the fuel senders used and behaves inconsistently to the stock fuel gauge.  It is based on an NTC resistor mounted next to the fuel pump which allows current to flow through the Low Fuel bulb if it heats up in the absence of being cooled by fuel.  From factory, this resistor is wired in series with the low fuel switch.  If the low fuel switch (essentially a float-controlled switch) on the left side of the tank is activated, the circuit is closed and the low fuel resistor is now in-circuit with the low fuel warning light.  When this resistor is only partially immersed in fuel (or not at all), it will begin to heat up and reduce its resistance, causing the low fuel light to shine at various brightness levels based on how much the resistor is being cooled or not.  It is a very blunt instrument and not greatly useful.  A very bright light is supposed to generally indicate very low fuel.

Because this low fuel light system is decoupled from the fuel gauge, its activation does not correlate with the gauge and, from the perspective of the driver, behave erratically.  In some cases, such as during cornering, the low fuel light might be on and then suddenly switch off completely rather than "dim out".  This is caused by fuel sloshing back over the tank saddle to the left side of the tank and causing the low fuel switch to disconnect.

### Summary of issues
From the perspective of measuring fuel remaining in the tank this entire stock system is imperfect, because the fuel gauge has no way of knowing how much fuel might be in the left or side of the tank with any real accuracy.  As a result, we can end up in one of a few less than optimal situations.  The fuel gauge may incorrectly report low fuel, or more fuel than is available, and may drop inconsistently.  The inconsistent fuel gauge behaviour may manifest as:
* Rapidly dropping when there has been no change in driving behaviour
* Not dropping for a long period of time (e.g., "hanging") even though a long distance has been travelled
* Hanging, followed by a sudden drop

In particular, "hanging" of the fuel gauge needle is common once the upper fuel sender rheostat has bottomed out.  This is because the remaining sender rheostat is in the right side of the fuel tank which is constantly being "topped up" by the jet pipe system which is pulling fuel from the left-side of the tank.


## Design
The design uses an Arduino Nano Every to read the fuel senders individually and drive both the fuel gauge and low fuel light.  The fuel sender rheostat signals are measured using a 5V voltage source and voltage divider connect to Arduino pins A0 and A1.  Knowing that the signals from the rheostats are not linear, the signal received by the Arduino is compared to a lookup table for each fuel sender rheostat and assigns a "litre" rating to each reading.  The two readings are added together to calculate the total amount of fuel in the tank.

To make the gauge read extremely accurately than stock, each fuel level range is assigned a PWM value which drives the switching of a MOSFET connected to Arduino Pin D5.  The switching of this MOSFET controls the average current flowing through the fuel gauge, which allows the PWM control to accurately force the needle to a desired position.  The needle positions are calibrated to the major mark on the gauge (Empty, 1/4, Half, 3/4, Full) as well as the smaller "pips" in between.

The low fuel light is driven digitally ON or OFF based on the calculated fuel readings.  Different light behaviours (solid on, slow flash, fast flash) are applied based on the amount of fuel left in the tank.

### Circuit changes to stock wiring
The design has the following major deviations from the stock circuitry:
 - Decouple the upper and lower rheostats and read them individually
 - Remove all use of the stock low fuel light circuitry, except for driving the low fuel light based on software control

### Accuracy of enhanced calibration data
The calibration data provides litre-accurate measurements of the fuel in both sides of the tank at **any given time**.  This means that regardless of where the fuel is in the tank, we can provide an accurate total for driving the fuel gauge.

### Schematic
A basic schematic using rough guesses of suitable components (other than the Arduino and resistors) is shown below.
![Schematic](https://github.com/Kaldek/EC5-fuel-calibrator/blob/main/Enhanced%20schematic.png)https://github.com/Kaldek/EC5-fuel-calibrator/blob/main/Enhanced%20schematic.png
