 this is preliminary information and is subject to change

# What it takes #

  * Teensy 2.0 board - (that's an ATMega32U4 16Mhz board: probably any other board with the same CPU should do like Adafruit's ATMega32U4 Breakout Board+).
  * USB cable (mini-usb to normal USB)
  * S-Video cable OR any 4-pin mini-din connector + wires (either female or male connector will do) OR nothing by directly soldering the wires inside the ADB board.
  * a resistor between 1k and 5k Ohms (5V TTL pull-up).
  * basic electronic soldering equipment and preferably a multi-meter to test out stuff
  * Java installed.
  * The WaxBee software
  * The Teensy Loader (for Teensy board) -- unnecessary on Windows
  * The appropriate driver for the USB board you are emulating (Wacom Intuos2 typically)

# What to do #

  * Before soldering anything, check that the Teensy board alone works correctly by plugging it into a USB port. Note that the Teensy ships with a firwmare that blinks the LED as well).

If you have a problem connecting, try running the Teensy loader application (from prjc.com) and upload something just to check that it works (one of the slow/fast Blinky are good test candidates).

  * You have to **solder** 3 wires from the mini-din connector. Essentially:
    * the ADB +5v (red) to any of the "VCC" on the Teensy
    * the ADB ground (black) to any of the "GND" on the Teensy
    * the ADB data bus (green) to "C7" (in the corner) on the Teensy
    * the last ADB pin (PSW/Power On) must be left unconnected.
  * In addition you have to solder the pull-up (1k..5k) resistor between the data bus (C7) and (VCC).

> Check out the ADB pinout on wikipedia:
> http://en.wikipedia.org/wiki/Apple_Desktop_Bus

> Since each ADB device is meant to be on a "bus" they all sports a male and female mini-din connector. We can connect to any one without a problem.

> Triple-check all your connections and that there is no "cross-overs", etc. A bad short circuit has the potential of permanently damaging the tablet, the Teensy or even your computer.

  * Run the WaxBee configuration tool, follow the instructions (read the bundled html for more help).

  * The Wacom driver of the emulated USB tablet can be installed/re-installed at any time. Always pick the latest drivers. (NOTE: pick the latest driver that supports the emulated hardware (for your OS), that is, an Intuos 2 as of this writing.)