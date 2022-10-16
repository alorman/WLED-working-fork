# WLED Integration Notes
Work to integtrate the dc metro map logic and controls into the excellent WLED (wireless LED) project. https://github.com/Aircoookie/WLED/blob/main/usermods/EXAMPLE_v2/usermod_v2_example.h

## Build Environment
https://kno.wled.ge/advanced/compiling-wled/
installed vscode and platformio extension per directions. Used sourcetree to clone a personal fork of wled.

had to comment out `default_envs = travis_esp8266, travis_esp32` despite instructions 

only building for `esp32dev`

ton of libraries cloning and downloading

initial build failed for this reason https://github.com/platformio/platformio-vscode-ide/issues/2363 then resolved itself on second build

Stripped out all usermod folders. code compiles fine. 

look at /wled00/usermods_list.cpp for a ton of ifdef statements about usermods. 

To reference usemod name use the class (not public) of the usermod.h

once flashed through platform IO unclear how to open the browser window, or how it knows my wifi password (if it does?) it must be stored in eeprom and i hadnt done an eeprom wipe. 

To enable serial debugging uncomment `#define WLED_DEBUG` in `wled.h`
How the actual hell does it know to connect to my wifi? but look for the little plug icon in the lower blue bar of the vscode

success in switching upload speed from 115200 to 921600 in `platformio.ini`(although oddly only worked on second iteration) this also seemed to reset the eeprom and it went into STATION wifi mode. default password is `wled1234` .
On second exploration this occationally seemed to cause a flash to go poorly and to get into boot loops.

adding the IP through station mode seems to work nicely. although it's not directly clear which IP it gets unless you look at the debug terminal scroll. 

per https://kno.wled.ge/advanced/custom-features/ to edit the statically baked html files you need to have nodejs installed. I installed from here: https://nodejs.org/en/download/ and installed chocolatey when prompted in the installer. i had to run the installer twice and then it completed. 

Test if npm is present by opening a new cmd window and typing `npm`. Then restart vscode and run it in the command line interface. 

use `npm install` then `npm run build` to rebuild the html

when wanting to live preview, you still need an actual wled isntance running. open the wled00/data/index.html file and then run `npm dev` in the command line. then point the browser to the wled hardware instance. 

## To remove an effect:
- search FX.h and FX.cpp for it, comment out all references
- in FX.h adjust the `#define MODE_COUNT` to reflect changes
- in FX.h scroll to the `const char JSON_mode_names` field and remove the friendly name. it may be slightly different than the class name
- run `npm run build` to rebuild the html pages in progmem
- compile and flash

This can also be done by only removing the json mode names and rebuilding the html. 

## Build and Flash
In lower taskbar of vscode look for forward arrow
Remember to run `npm run build` to rebuild the static html

## Flash to a blank esp32
Go to install.wled.me and run the utility there. 
Set wifi credentials in chrome when prompted
then close chrome and open vscode and Flash then reboot esp32. It will create a wifi station called `WLED-AP`
connect to this and put in wifi controls, which should automatically appear.
If not dial it a 4.3.2.1
The wifi settings will be saved to the NV file system, (is then when using the chrome full flash version)
If the desired wifi net cant be found, it will revert to the station as before
Have had to hard reboot by usb when flashing

Changes do not seem to persist

## Setting up initial LEDs via webgui
in `Settings` (gear icon) scroll down to `LED Outputs`
We probably want to tick `Make a segment for each output` else WLED will (very cleverly) put everything into one concatenated segment.
I also disabled the pushbuttons
Make sure to adjust the total current limiter. If all LEDS are blank, this is probably what is kicking in

## interacting with littlefs
https://randomnerdtutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/
Use the ant symbol in the left of platform IO to do SPIFFS commands.
Also use IP-address/edit to see json

## setting up default led hardware (NOT segments)
set in the defines for FX_fcn.cpp

You'll get an error about pins not being able to be used if they're also defined in the usermod or the cfg.json file. 
cfg.json doesn't seem to make any difference, it seems to make a new one on the fly even if one exists.
