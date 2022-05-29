# WLED Integration Notes
Work to integtrate the dc metro map logic and controls into the excellent WLED (wireless LED) project. https://github.com/Aircoookie/WLED/blob/main/usermods/EXAMPLE_v2/usermod_v2_example.h

## Build Environment
https://kno.wled.ge/advanced/compiling-wled/
installed vscode and platformio extension per directions. Used sourcetree to clone a personal fork of wled.

had to comment out `default_envs = travis_esp8266, travis_esp32` despite instructions 

only building for `esp32dev`

ton of libraries cloning and downloading

initial build failed for this reason https://github.com/platformio/platformio-vscode-ide/issues/2363 then resolved itself on second build