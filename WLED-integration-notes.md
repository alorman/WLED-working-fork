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

once flashed through platform IO unclear how to open the browser window, or how it knows my wifi password (if it does?) it must be stored in eeprom and i hadnt dont an eeprom wipe. 

To enable serial debugging uncomment `#define WLED_DEBUG` in `wled.h`
How the actual hell does it know to connect to my wifi? but look for the little plug icon in the lower blue bar of the vscode

success in switching upload speed from 115200 to 921600 in `platformio.ini`(although oddly only worked on second iteration) this also seemed to reset the eeprom and it went into STATION wifi mode. default password is `wled1234` .
On second exploration this occationally seemed to cause a flash to go poorly and to get into boot loops.

adding the IP through station mode seems to work nicely. although it's not directly clear which IP it gets unless you look at the debug terminal scroll. 

per https://kno.wled.ge/advanced/custom-features/ to edit the statically baked html files you need to have nodejs installed. I installed from here: https://nodejs.org/en/download/ and installed chocolatey when prompted in the installer. i had to run the installer twice and then it completed. 

Test if npm is present by opening a new cmd window and typing `npm`. Then restart vscode and run it in the command line interface. 

use `npm install` then `npm run build` to rebuild the html

when wanting to live preview, you still need an actual wled isntance running. open the wled00/data/index.html file and then run `npm dev` in the command line. then point the browser to the wled hardware instance. 

To remove an effect:
- search FX.h and FX.cpp for it, comment out all references
- in FX.h adjust the `#define MODE_COUNT` to reflect changes
- in FX.h scroll to the `const char JSON_mode_names` field and remove the friendly name. it may be slightly different than the class name
- run `npm run build` to rebuild the html pages in progmem
- compile and flash

This can also be done by only removing the json mode names and rebuilding the html. 


