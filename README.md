# Brewskey.ExpansionBox
C code for running the 4 tap expansion box

## About
We do not support flashing your own board nor do we recommend it.  There is a good chance you will fry your expansion board. Brewskey will not replace or fix your board if you do. Continue at your own risk.

## Tools
[To compile we recommend Atmel Studio](http://www.atmel.com/tools/atmelstudio.aspx)
[AVR Loader](assets/avrloader.7z) - We've included the binary in this repo. If you'd rather download externally, [you can grab it here](http://www.electroschematics.com/9431/flash-burning-process-program-explanation/).

You'll also need a USBASP programmer and adapter to flash the board. Here are some cheap ones we've used:
[USBASP programmer](https://www.amazon.com/gp/product/B00AX4WQ00/ref=ox_sc_act_title_1?smid=A30QSGOJR8LMXA&psc=1)
[Adapter](https://www.amazon.com/gp/product/B00CW80MP6/ref=ox_sc_act_title_2?smid=A28PCZ3GGXGBES&psc=1)

If you use the USBASP programmer, you'll need to [install USB drivers](https://rayshobby.net/dead-simple-driver-installation-for-usbasp-and-usbtiny-on-windows/)

## Flashing
After compiling the source using Atmel Studio, you should see a `.hex` file in the debug/release binaries.
**It is very important that you do not have the expansion box plugged into the Brewskey box while flashing. If you do, there is a good chance you will fry your board**

1. Make sure you are targeting ATMega 8
2. Select the hex file 

![Flash setup](assets/flash.PNG?raw=true)
  
3. I recommend that you verify before trying to flash. This will make sure that you've plugged in the USBASP programmer correctly onto the expansion board.

4. Click the Edit fuse button. Edit the fuses to look like this:

![Fuse setup](assets/fuses.png?raw=true)

5. Write the fuses and close the fuses window
6. Click the write button under the `Flash` section of the main window.

If you did everyting correctly the expansion box should now be flashed.

## Packet format
We have built a specific packet format for communication between the Brewskey box and the Expansion box.

[Here is the documentation on the format](assets/SerialProtocol.pdf?raw=true)

---
### Solenoid/Flow Sensor Settings Flags
The solenoids are set on/off and flow sensor are reset by sending bit flags. These flags are also used for sending back the current solenoid status to the Brewskey box.
* **Tap 1** - 0x03
* **Tap 2** - 0x0C
* **Tap 3** - 0x30
* **Tap 4** - 0xC0

### V1
#### Data From Brewskey Box:
The Brewskey box will periodically ping the Expansion box with this packet to trigger a response.
* [0] - To Address
* [1] - Source
* [2] - Packet Type - **0x22**
* [3] - Solenoids On - *uses settings flags*
* [4] - Solenoids Off - *uses settings flags*
* [5] - Reset Flow Sensors - *uses settings flags*
* [6] - Checksum

*<small><space> </space> Expansion boxes for the Brewskey Beta expect an additional byte between 2 & 3 for the Packet Version. This value is 0x01.</small>*

#### Data To Brewskey Box
The data sent back to the Brewskey box includes the state of the solenoids and the number of total pulses on each tap. *Pulses are sent as a 4 byte unsigned value stored in little endian format.*
* [00] - To Address
* [01] - Source
* [02] - Packet Type - **0x33**
* [03] - Solenoid Status -*uses settings flags*
* [04] - **Tap 1 pulses**
* [05]
* [06]
* [07]
* [08] - **Tap 2 pulses**
* [09]
* [10]
* [11]
* [12] - **Tap 3 pulses**
* [13]
* [14]
* [15]
* [16] - **Tap 4 pulses**
* [17]
* [18]
* [19]
* [20] - Checksum

*<small><space> </space> Expansion boxes for the Brewskey Beta expect an additional byte between 2 & 3 for the Packet Version. This value is 0x01.</small>*

