# EC5-fuel-calibrator
Fuel Gauge recalibration project for the 8th-generation Mitsubishi Galant and Legnum VR4.

The stock fuel gauge on these cars is extremely inaccurate due to the use of a simple analogue gauge that reacts to a change in resistance of two rheostats in the fuel tank which are wired in-series.  The fuel tank however uses a saddle design where the middle of the tank has a "hump" in the bottom to allow for the driveshaft to reach the rear differential.  The right side of the saddle design has the fuel pump and one of the fuel sender rheostats.  To avoid fuel starvation and fuel being stuck in the side of the tank where there is no fuel pump, the the fuel delivery system makes use of a "jet pipe" (venturi pump) located in the left side of the tank.

As the electronic fuel pump in the tank draws fuel, it causes a venturi effect in the "jet pipe" which does what it can to constantly keep pumping fuel back over from the left side of the tank into the side where the fuel pump is.  Hard turns to the right when cornering can cause all this work to be undone as the fuel sloshes back over the saddle to the left side of the tank. 

From the perspective of measuring fuel remaining in the tank this system is imperfect, because the fuel gauge has no way of knowing how much fuel might be in the left side of the tank.  As a result, we can end up in one of a few less than optimal situations.  The fuel gauge may incorrectly report low fuel, or more fuel than is available, and may drop inconsistently.  The inconsistent fuel gauge behaviour may manifest as:
* Rapidly dropping when there has been no change in driving behaviour
* Not dropping for a long period of time (e.g., "hanging") even though a long distance has been travelled
* Hanging, followed by a sudden drop

In particular, "hanging" of the fuel gauge needle is common once the upper fuel sender rheostat has bottomed out.  This is because the remaining sender rheostat is in the right side of the fuel tank which is constantly being "topped up" by the jet pipe system which is pulling fuel from the left-side of the tank.

The baseline code uses a simple table of value ranges that control a PWM signal.  The fuel sender rheostat signal is measured using a 5V voltage source and voltage divider connect to Arduino pin #0.  Knowing that the signals from the rheostats are not linear, the signal received by the Arduino is compared to a lookup table.  To make the gauge read (more) accurately than stock, each row in the lookup table is assigned a PWM value which drives the switching of a MOSFET connected to Arduino Pin #5.  The switching of this MOSFET controls the average current flowing through the fuel gauge, which allows the PWM control to accurately force the needle to a desired position.
