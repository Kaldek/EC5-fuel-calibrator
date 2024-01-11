## Isn't this dangerous?
No.  The stock circuitry is all +12v and is immersed in the fuel, whereas this solution uses +5v for all current passing through the fuel sender rheostats.  It's no worse than the stock circuitry, and the current is also lower.

## How did you make the fuel gauge accurate to measurements of fuel?
The fuel tank from an EC5W (Legnum VR4 Wagon) was removed and emptied completely.  The two rheostats were separately wired up to an Arduino and their resistance measure using a voltage divider.  For each side of the tank, one litre of fuel was added at a time and measurements of the Arduino ADC pins were taken after each litre was added.  Once the fuel reached the level of the saddle, the other side of the fuel tank was added fuel, also one litre at a time.

Once both sides of the fuel tank were just at the point of the saddle, fuel was added one litre at a time again but this time taking measurements from both rheostats.

### Since you took such accurate measurements, what size is the fuel tank?
When fitted to the car, the fuel tank can hold 67.5 litres of total fuel.  Of this 67.5 litres, about 6 litres is unusable due to the levels of fuel in the tank being too low for the fuel pump to pick up or the jet pipe to move from one side of the tank to the other.  Ergo, you can absolutely get 61.5 litres of fuel and possibly a couple more on a good day.  Some fuel remains in the tank for the purpose of sediment fallout and is not used.

## How come PWM drive works on the fuel gauge?
The fuel gauge in these cars is an air core gauge, essentially two coils of wire.  The outer coil sets a magnetic field and the inner coil is mounted to the needle.  As current through the coil changes, it causes the needle to change position within the magnetic field.

Using a PWM drive we effectively control the average current passing throug the coil.  As the fuel gauge is so slow, these pulses don't affect the gauge and it behaves just as it does when the current is controlled by a rheostat.

## Why didn't you use the low fuel light circuitry?
We control the low fuel light based on our now accurate measurements of fuel in the tank.  The stock low fuel light circuitry is only activated when the left side of the tank drops below 9 litres and the right side of the tank is also low.  Its behaviour was inconsistent at best, but with our accurate measurements of fuel it is better to digitally control this light.

## Can I add my own digital fuel readout?
Absolutely.  You can alter the Arduino sketch to include OLED display drivers and display the amount of fuel remaining in the tank.  If you do this, we recommend that you add some serious "smoothing" to the fuel level displayed.  Whilst the gauge itself responds slowly, the Arduino completes hundreds of loops per second and the reading of each rheostat will range up and down as you go over bumps and through corners.  Smoothing this information out to something like a 20 second average reading will make the digital display much more useful.
