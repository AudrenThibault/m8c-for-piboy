# m8c

m8c is a client for Dirtywave M8 tracker's headless mode. The application should be cross-platform ready and can be built in Linux, Windows (with MSYS2/MINGW64) and Mac OS.

Please note that routing the headless M8 USB audio isn't in the scope of this program -- if this is needed, it can be achieved with tools like jackd, alsa\_in and alsa\_out for example. Check out the guide in file AUDIOGUIDE.md for some instructions on routing the audio.

Many thanks to:

Trash80 for the great M8 hardware and the original font (stealth57.ttf) that was converted to a bitmap for use in the progam.

driedfruit for a wonderful little routine to blit inline bitmap fonts, https://github.com/driedfruit/SDL_inprint/

marcinbor85 for the slip handling routine, https://github.com/marcinbor85/slip

turbolent for the great Golang-based g0m8 application, which I used as reference on how the M8 serial protocol works.

Disclaimer: I'm not a coder and hardly understand C, use at your own risk :)

-------

## Installation

These instructions are tested with Raspberry Pi 3 B+ and Raspberry Pi OS with desktop (March 4 2021 release), but should apply for other Debian/Ubuntu flavors as well. The begining on the build process on OSX is slightly different at the start, and then the same once packages are installed.

The instructions assume that you already have a working Linux desktop installation with an internet connection.

Open Terminal and run the following commands:

### Install required packages (Raspberry Pi, Linux)

```
sudo apt update && sudo apt install -y git gcc make libsdl2-dev libserialport-dev
```
### Install required packages (OSX)

This assumes you have [installed brew](https://docs.brew.sh/Installation)

```
brew update && brew install -y git gcc make sdl2 libserialport
```
### Download source code (All)

```
mkdir code && cd code
git clone https://github.com/laamaa/m8c.git
 ```

### Build the program

```
cd m8c
make && sudo make install
 ```

### Start the program

Connect the Teensy to your computer and start the program. It should automatically detect your device.

```
m8c
```

If the stars are aligned correctly, you should see the M8 screen.

-----------

## Keyboard mappings

Keys for controlling the progam:

* Up arrow = up
* Down arrow = down
* Left arrow = left
* Right arrow = right
* a / left shift = select
* s / space = start
* z / left alt = opt
* x / left ctrl = edit

Additional controls:
* Alt + enter = toggle full screen / windowed
* Alt + F4 = quit program
* Delete = opt+edit (deletes a row)
* Esc = toggle keyjazz on/off 

Keyjazz allows to enter notes with keyboard, oldschool tracker-style. The layout is two octaves, starting from keys Z and Q.
When keyjazz is active, regular a/s/z/x keys are disabled.

## Gamepads

The program uses SDL's game controller system, which should make it work automagically with most gamepads.

Enjoy making some nice music!

-----------

### Bonus: improve performance on the Raspberry Pi
Enabling the experimental GL Driver with Full KMS can boost the program's performance a bit.

The driver can be enabled with ```sudo raspi-config``` and selecting "Advanced options" -> "GL Driver" -> "GL (Full KMS)" and rebooting.

Please note that with some configurations (for example, composite video) this can lead to not getting video output at all. If that happens, you can delete the row ```dtoverlay=vc4-kms-v3d``` in bottom of /boot/config.txt.

Further performance improvement can be achieved by not using X11 and running the program directly in framebuffer console, but this might require doing a custom build of SDL.


-----------

everything above this line is original readme

### notes on this fork

- made some tiny changes to make m8c work with my odroid go advance. I just hacked in a joystick device for the OGA buttons (didn't use the actual joystick, just DPAD). super simple and not very elegant, I'm not that familiar with SDL
- B is edit, A is option, the last two buttons on the bottom of the screen are shift/select and start
- you still need to use alsa/jack for audio, same as in Pi, etc. it "works" with headphones and speaker. "works" is in quotes because so far sound is glitchy.
- teensy has to go in the USB-A host port, not the USB-C port that does power
- my observations so far:
    - it's actually fairly usable
    - the OGA's audio is fairly bad. beside the glitchiness (probably due to overtaxed CPU?) it's also just tinny and lifeless (this has nothing to with M8 -- the OGA is a device made for gaming, not serious audio. it sounds way better on my main desktop)
    - the display looks a bit wonky (aliasing artifacts) because the aspect ratios don't match. Same as with NES and other consoles except for GBA. Sad!
    - buttons on OGA are OK to mediocre, but we all knew that already
    - even with all these issues it's still pretty fun and has made me eager to get real m8
- this is in no way a replacement for a real m8, or even comparable! with real m8 you get
    - 1000% better audio. The m8 makes great sounds, the OGA DAC doesn't do them justice!
    - nice display that actually matches aspect ratio of software and even a touch surface
    - no fiddling around with alsa/jack/m8c/whatever to get it running
    - high quality key switches, not plasticky buttons
    - probably more battery life
