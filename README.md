# ErikaOS-RealTime-watch
Runs a real-time digital watch using Erika-OS on a stm32f4-discovery board with a discover-more extension board.
## Hardware 
It's a _STM32f407-discovery_ using _std_ library mounted over a _dicover-more_ extension board. This board also includes a LCD. 

## How to run/flash it
if the hardware is ready, The file _c_mX.bin_ can be flashed as follows:
``` bash
$ st-flash write c_mX.bin 0x8000000
```

## Recompile
In order to recompile you need to download ERIKA2.x-OS from https://www.erika-enterprise.com/index.php/download/erika-v2.x.html
Once Erika is intall, you only need the Erika-CLI not the whole Eclipse to work. So, everything usefull executable you need is under the _evidence_ folder.

### Step 1
Generate files from configuration file _conf.oil_ 

``` bash
$ erika-gen /path/where/Erika-CLI/is/installed/Erika-CLI/ conf.oil .
```

### step 2
Edit _usr.mk_ to change your gcc compiler location:

``` bash
$ nano usr.mk
```
in the first line you should see something like

``` make
GNU_ARM_ROOT = $(HOME)/Apps/gcc-arm-none-eabi-9-2019-q4-major
```
change it with your gcc compiler location, save it and add _usr.mk_ inside _makefile_ as follows: 

``` bash
$ nano makefile
```
in the first line write:

``` bash
$ include usr.mk
```


## Requirements
- Erika2.x OS (the operating system of the board)
- gcc-arm-none-eabi (to compile the project)
- stlink (to flash the board)
