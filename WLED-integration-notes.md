# WLED Integration Notes
Work to integrate the dc metro map logic and controls into the excellent WLED (wireless LED) project. https://github.com/Aircoookie/WLED/blob/main/usermods/EXAMPLE_v2/usermod_v2_example.h

## Build Environment
https://kno.wled.ge/advanced/compiling-wled/
installed vscode and platformio extension per directions. Used sourcetree to clone a personal fork of wled.

Had to install git https://git-scm.com/download/win this allowed platform IO to download the requisite arduino libraries like math.h

had to comment out `default_envs = travis_esp8266, travis_esp32` despite instructions 

only building for `esp32dev`

ton of libraries cloning and downloading

initial build failed for this reason https://github.com/platformio/platformio-vscode-ide/issues/2363 then resolved itself on second build

Stripped out all usermod folders. code compiles fine. 

look at /wled00/usermods_list.cpp for a ton of ifdef statements about usermods. 

To reference usermod name use the class (not public) of the usermod.h

once flashed through platform IO unclear how to open the browser window, or how it knows my wifi password (if it does?) it must be stored in eeprom and i hadn't done an eeprom wipe. 

To enable serial debugging uncomment `#define WLED_DEBUG` in `wled.h`
How the actual hell does it know to connect to my wifi? but look for the little plug icon in the lower blue bar of the vscode

success in switching upload speed from 115200 to 921600 in `platformio.ini`(although oddly only worked on second iteration) this also seemed to reset the eeprom and it went into STATION wifi mode. default password is `wled1234` .
On second exploration this occationally seemed to cause a flash to go poorly and to get into boot loops.

adding the IP through station mode seems to work nicely. although it's not directly clear which IP it gets unless you look at the debug terminal scroll. 

per https://kno.wled.ge/advanced/custom-features/ to edit the statically baked html files you need to have nodejs installed. I installed from here: https://nodejs.org/en/download/ and installed chocolatey when prompted in the installer. i had to run the installer twice and then it completed. I also installed the recommended additional tools. 

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

## Setting trains to be preset
Set each segment to the correct line FX and desired color. Then Press `Save Preset` give it preset number 1. Then go to `Config > Default preset` to 1 and set `show prset on boot`. You may also have to disable `Turn LEDs on after power up/reset`.

## interacting with littlefs
https://randomnerdtutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/
Use the ant symbol in the left of platform IO to do SPIFFS commands.
Also use IP-address/edit to see json

## setting up default led hardware (NOT segments)
set in the defines for FX_fcn.cpp

You'll get an error about pins not being able to be used if they're also defined in the usermod or the cfg.json file. 
cfg.json doesn't seem to make any difference, it seems to make a new one on the fly even if one exists.

## usermods
when putting lines into the connected() loop, i can't get them to run. 
usermods get called before the lights go into their default states. So ccalling getpixelcolor or similar from usermod::setup doesnt work

## storing data in eeprom via the readfromjsonstate
make sure you write using `addtoconfig` it will automatically read

## Erase eeprom 
in vscode, navigate to the platformio CLI. run `pio run --target erase`. If there is an access denied issue, close the serial monitor 

## to read LED colors
look to the main state json (which isnt in eerpom and just fires around during runtime)
state_seg_0_col_0_0 = root["seg"]["0"]["col"]["0"]; // 213

when previewing from the json look at XXXXX/json/state (rather than XXXXX/json. they're not quite the same)

readfromjson state seems to only fire the changing fields around. for instance, when i change strip color, i get 0 on brightness, then changing brightness will put it back to its usual value

# Making a new effect
(this allows for indivual control over leds and an experiance much more like fastLED or adafruits neopixel library. 
add a line to FX.cpp with the name of your effect

around line 491 of FX.h, add the mode name and linkage to the class
in the top of FX.h add a #define line giving the effect a number
in FX.h add it to the uint16-t list of builtin modes around line 718
in FX.h at the end, add to the progmem statement in the correct order as defined around line 718
add the name in your definitions at the top of FX.h
remember to update #define MODE_COUNT
recompile the html

the compiler is happier if you have a return statement in the function

It seems that whenever an effect is applied and you're sourcing `SEGCOLOR` functions, these get applied to the new segment and the old segment, so all segments of that effect become that color palette. 

## SEGENV
`SEGENV.XXX` is the ability to add persistent variables like timing or counting to each assigned effect. These are defined in fx.h around 356
 
## Blink vs Mode_Blink
they're not the same. blink is the helper object with arguments that gets called by mode_blink. blink itself is never revealed in the UI or as a mode itself. chase is similarly used.
Overall this architecture allows derivations on a theme and calling of basic helpers within other effects.

#Refresh times
It seems like if we call the GetWMATAData faster than 1hz it causes an unbootable error.

## using transition delay timing
you can call the global variable `transitionDelay` to get (in ms) your delay time. It seems like you click the effect again to get it to re-init when applying a new timing change from the gui.

# Global variables
it really seems like `wled.h` doesn't like fully defined variables. the compiler throws (possibly meaninguless) erros, but ultimately it wont compile. 
Put global variables in `wled.h` like `WLED_GLOBAL uint8_t devInt;`


# Changing WEB GUI
## 1,2,3 lables
see around line 107 in index.htm

# LittleFS files
all files in the `/data` folder are transferred when PlatformIO performs a file system write.
Adding a `/presets.json` file here will make it get written to the file system.

## Presets on boot
It seems like the software isn't reading the `presets.json` file, even when it exitsts. 
Using `applyPreset(1);` from any point in the user mode will apply preset `1` (this is present number 1 not preset 0 index).
per https://github.com/Aircoookie/WLED/issues/2434 wled does not support prettified json 

## Notes on Time
`localTime` expresses global epoch time in seconds, adjusted for time zone
call local times with `minute(localTime)`, year, month, day , hour, minute and second are acceptable inputs. expresses the current holding of those fields

# How the LEDS are plotted
after calculation, the LED values are written to `[color]DisplayFrame`, which is a global array variable declared as a `WLED_GLOBAL` in the wmata usermod.  
After that it goes to the FX.cpp and each displayframe is taken apart and plotted.  
At the end of each fx function, `return FRAMETIME` is called, which allows the actual plotting code to plot. 