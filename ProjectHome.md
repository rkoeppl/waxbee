This hardware project aims at building a USB bridge for ADB or Serial-based Wacom/Calcomp tablets and effectively convert serial/adb tablets to USB.

This is a C++ based project meant to run on low cost AVR [Teensy](http://www.pjrc.com/teensy) hardware.  Java-based configuration utility running on Win32/64, Mac OS X & Linux.

NOTE: Soldering is required to perform a tablet conversion. This is done at your own risks.

# Projects #

| **Project**            | **Platform** | **Notes** |
|:-----------------------|:-------------|:----------|
| Serial/ADB to USB bridge    | Teensy 2.0   | Basic functionality working, on going|
| Waxbee Config          | Java         | Early version working, on going|
| ADB Sniffer            | Arduino UNO  | Done      |
| USB virtual serial port | Teensy 2.0   | Done|

### Native Tablet Support Status ###
| **ADB Ultrapad/Digitizer/DigitizerII/ArtZII** | UD-xxxx-A | basic pen functionality|
|:----------------------------------------------|:----------|:-----------------------|
| **ADB Intuos**                                | GD-xxxx-A | Non functional. Still not sure how to interpret the ADB data. See ADB Intuos section below. |
| **Serial Penenabled (TabletPC)**              | (various) | (ISDV4) Basic Pen functionality works |
| **Serial Penenabled TabletPC Touch**          | (various) | (ISDV4) early coding stage |
| **Serial Ultrapad/Digitizer II/ArtZII**       | UD-xxxx-R | (Protocol IV) is working (basic pen functionality). |
| **Serial ArtPad II**                          | KT-xxxx-R | (Protocol IV) basic pen functionality |
| **Serial Intuos**                             | GD-xxxx-R | (Protocol V) all tools supported, menu strip works when emulating USB Intuos 2|
| **Serial Intuos2**                            | XD-xxxx-R | (Protocol V) all tools supported, menu strip works when emulating USB Intuos 2 |
| **Serial Topaz**                              |           | Protocol implemented but largely untested |

### USB Emulation Status ###
| **Graphire 3** |  Basic functionality works |
|:---------------|:---------------------------|
| **Intuos2 12x18, 12x12, 9x12, 6x8, 4x5** | All tools and menu strip working. Note that Intuos2 does not have official Windows 8 drivers as of this writing. |
| **Intuos5**| Basic pen functionality. (Limited templates "UD-1212-R to Intuos5 S" only for now. See Intuos5 talk below. |
| **Bamboo Touch** | Early coding stage. Development not far (superseeded by Intuos5 work) |

Latest Template list: (bundled in the distribution zip)
http://code.google.com/p/waxbee/source/browse/#svn%2FWaxBeeConfig%2Fconfig_templates

### Waxbee Config tool status ###
Native libraries not well handled and specific to Windows for now. (code is present for other platforms but not compiled). Native libraries are not needed if you "flash" your Teensy using PRJC.com's Loader utility (use the "generate hex file" option).

### Credits ###
  * Portions of the code from the open source Linux Wacom driver
  * Intuos1/2 Tools & serial number support by Chris Dragon of [Dracoventions.com](http://www.dracoventions.com)
  * Thanks to all people helping out!

### Interfacing ADB ###

Some rough instructions to interface ADB Wacom tablets here: [InterfacingADB](InterfacingADB.md)

### USB Serial ###
This implements a virtual Serial port on the Teensy 2.0 specifically targeted to talk to tablets for testing purposes (original code courtesy of prjc.com - forked from V1.7). Please check out http://www.pjrc.com/teensy/usb_serial.html

![http://waxbee.googlecode.com/svn/wiki/udhack.jpg](http://waxbee.googlecode.com/svn/wiki/udhack.jpg)

### Wacom hid dump tool ###

In the download section, there is a tool (Win7 64 bits tested only) that will detect a wacom device and will dump the packets being received. Some packets are decoded. This is great for testing/debugging purposes.

### Helping/Developing ###
If you want to get your hands deep into the code, well, Chris Dragon attempted to makeup a simple procedure to have a working development environment.  See here: http://forum.bongofish.co.uk/index.php?topic=2059.msg16167#msg16167


### ADB Intuos ###
For the ADB Intuos (Model numbers GD-xxxx-A). We do not know how to interpret the ADB data. We think we correctly read the "bits", but interpreting them is far from conclusive. The weird thing is that there isn't that many bits to look at!
We talked to an internal Wacom person (that someone had a contact to). But Wacom stubbornly refuses to disclose the information for this old/unsupported technology.
If you want to jump in, be my guest, doing this really feels like "DaVinci Code". The answer appear so close, but yet, we do not get it.
edit: we actually cracked it.
### Intuos 5 ###
New emulation support for Intuos5. This will help enabling three things: 1- Windows 8 drivers. Official driver support for Windows 8 from Wacom. 2- Custom Buttons: Since Intuos5 have pads and lots of buttons it will be easy to eventually use that to map custom keys or devices. 3- Custom Touch: Intuos5 also have touch, so it would be feasible to convert a custom touch device into simulated Intuos5 touch packets.