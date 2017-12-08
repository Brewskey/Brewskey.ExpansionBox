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
