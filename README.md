# EC5-fuel-calibrator
Fuel Gauge recalibration project for the 8th-generation Mitsubishi Galant and Legnum VR4 (chassis codes EC5A and EC5W).  Likely, the project will also function for the Lancer Evolution, models 1 through 9 (I through IX).

The stock fuel gauge on these cars is extremely inaccurate due to the use of a simple *linear* analogue gauge that reacts to a change in resistance of two rheostats (upper and lower) in the fuel tank which are wired in-series.  The fuel tank however uses a saddle design where the middle of the tank has a "hump" in the bottom to allow for the driveshaft to reach the rear differential.  This results in *non-linear* fuel levels from the fuel senders.  The right side of the saddle design has the fuel pump and one of the fuel sender rheostats.  To avoid fuel starvation and fuel being stuck in the side of the tank where there is no fuel pump, the fuel delivery system makes use of a "jet pipe" (venturi pump) located in the left side of the tank.  

Note that there are upper and lower fuel sender rheostats because the fuel tank is also a "couch" design where when viewed from side-on, the fuel tank has the shape of a couch, with a flat lower section (where the saddle is) and an almost vertical upper section.  The fuel tank can be described as saddling the driveshaft, whilst also angling up beneath the base of the rear passenger seats.

As the electronic fuel pump in the tank draws fuel, it causes a venturi effect in the "jet pipe" which does what it can to constantly keep pumping fuel back over from the left side of the tank into the side where the fuel pump is.  Hard turns to the right when cornering can cause all this work to be undone as the fuel sloshes back over the saddle to the left side of the tank. 

From the perspective of measuring fuel remaining in the tank this system is imperfect, because the fuel gauge has no way of knowing how much fuel might be in the left side of the tank.  As a result, we can end up in one of a few less than optimal situations.  The fuel gauge may incorrectly report low fuel, or more fuel than is available, and may drop inconsistently.  The inconsistent fuel gauge behaviour may manifest as:
* Rapidly dropping when there has been no change in driving behaviour
* Not dropping for a long period of time (e.g., "hanging") even though a long distance has been travelled
* Hanging, followed by a sudden drop

In particular, "hanging" of the fuel gauge needle is common once the upper fuel sender rheostat has bottomed out.  This is because the remaining sender rheostat is in the right side of the fuel tank which is constantly being "topped up" by the jet pipe system which is pulling fuel from the left-side of the tank.

## Baseline code
The baseline code (simple-table) uses a simple table of value ranges that control a PWM signal.  The fuel sender rheostat signal is measured using a 5V voltage source and voltage divider connect to Arduino pin #0.  Knowing that the signals from the rheostats are not linear, the signal received by the Arduino is compared to a lookup table.  To make the gauge read (more) accurately than stock, each row in the lookup table is assigned a PWM value which drives the switching of a MOSFET connected to Arduino Pin #5.  The switching of this MOSFET controls the average current flowing through the fuel gauge, which allows the PWM control to accurately force the needle to a desired position.

This code is coupled with a custom Arduino Shield which interfaces the Arduino to the wiring harness of the car, for power + ground and intercepting the fuel gauge and rheostat connections.

## Enhanced code
The enhanced code will be written once the PCB prototype has been completed and we have obtained accurate readings from the low fuel sender rheostat.  This enhanced design intercepts additional signals which can be used for the purpose of further increasing gauge accuracy, and also allows for direct control over the low fuel warning light.  The low fuel warning light circuit on the VR4 is separate to to the fuel gauge circuit, allowing us to control this circuit.  The proposed enhanced design will monitor the "low fuel switch" located in the left side of the tank and the "low fuel rheostat" located in the right side of the tank. 

### Description of the low fuel light circuitry
The low fuel rheostat is part of the fuel pump assembly.  From factory, this rheostat is wired in series with the low fuel switch.  If the low fuel switch (essentially a float-controlled switch) on the left side of the tank is activated, the circuit is closed and the low fuel rheostat is now in-circuit with the low fuel warning light.  As the level of fuel drops, the changing resistance of the rheostat allows for more current to pass through the low fuel warning light, causing this light to shine at various brightness levels based on how little fuel is left.  A very bright light indicates very low fuel.

Because this low fuel light system is decoupled from the fuel gauge, its activation does not correlate with the gauge and, from the perspective of the driver, behave erratically.  In some cases, such as during cornering, the low fuel light might be on and then suddenly switch off completely rather than "dim out".  This is caused by fuel sloshing back over the tank saddle to the left side of the tank and causing the low fuel switch to disconnect.

### Enhanced calibration behaviour
The proposed enhanced design will add monitoring of the low fuel switch to instruct the Arduino to use an altered fuel table which will use a combination of signals from the lower fuel sendor rheostat and the low fuel rheostat.  The exact values in this table cannot be created until we source an entire fuel tank assembly and carefully monitor the behaviour of the low fuel switch and low fuel rheostat by filling the tank with preicesly measured amounts of fuel.  **This is work still to be done.**
