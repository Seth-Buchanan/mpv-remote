# mpv-remote
mpv-remote is a c plugin for mpv that allows for using mpv with a tv remote through an arduino.


### Operating Systems
Currently, mpv c plugins only support linux.


### Hardware

* For this project, I had an [Arduino uno kit](https://www.amazon.com/ELEGOO-Project-Tutorial-Controller-Projects/dp/B01D8KOZF4/ref=sr_1_4?keywords=arduino+uno+kit&qid=1681251235&sr=8-4j) but each of the important parts can be purchased separately.

    * [Arduino uno](https://www.amazon.com/ELEGOO-Board-ATmega328P-ATMEGA16U2-Compliant/dp/B01EWOE0UU/ref=sr_1_5?crid=1LAIIDP26SAL3&keywords=elegoo+uno&qid=1681251383&sprefix=elegoo+uno%2Caps%2C105&sr=8-5)

    * [IR receiver](https://www.amazon.com/KOOBOOK-Infrared-Wireless-Control-Receiver/dp/B07S9BQKN2/ref=sr_1_4?keywords=elegoo+ir+receiver&qid=1681251538&sprefix=elegoo+IR+%2Caps%2C94&sr=8-4)

    * [Dupont wires](https://www.amazon.com/Elegoo-EL-CP-004-Multicolored-Breadboard-arduino/dp/B01EV70C78/ref=sr_1_5?crid=18ZDTBTRG1O3F&keywords=female%2Bto%2Bmale%2Bdupont%2Bwires&qid=1681251439&sprefix=female%2Bto%2Bmale%2Bdupont%2Bwires%2Caps%2C100&sr=8-5&th=1)

    * IR remote control: An old tv remote works best.


### Arduino Setup
* Make sure that your user is in the dialers group or the uucp group in arch linux. You may have to reboot your computer after adding yourself to the group.

* Install IRremote library. In Arduino IDE, this can be installed at Tools > Manage Libraries... type "IRremote" in the search bar and select the one authored by shirriff.

* Use Arduino-ide to program receiver.ino to your arduino uno.

* For physical set up, [here is a good guide](https://www.instructables.com/Beginners-Guide-to-Use-an-IR-Remote-Transmitter-an/) on connecting the IR receiver to the board.

### Building the Plugin

* To Build the plugin, [the json-c](https://github.com/json-c/json-c) library will need to be installed.

* Use gcc with the options specified in the MAKEFILE.

* Move the plugin to ~/.config/mpv/scripts.

### Configuring the Plugin

* Test that your computer is getting the output from the arduino by using arduino-ide's serial monitor tool.
* Move mpv-remote.json into the ~/.config/mpv/script-opts directory. 

```json
{
    "AB54C738": "cycle pause",
    "ED12C738": "quit",
    "E11EC738": "seek 5",
    "E01FC738": "seek -5",
    "B14EC738": "seek 60",
    "B04FC738": "seek -60",
    "FE01C738": "playlist-prev",
    "FD02C738": "playlist-next",
    "EE11C738": "cycle sub",
    "AD52C738": "show-progress",
    "FF00C738": "cycle fullscreen",
    "F10EC738": "add volume 5",
    "F00FC738": "add volume -5",
    "F50AC738": "add chapter 1",
    "F40BC738": "add chapter -1",
    "E718C738": "cycle mute"
}
```
With the output from the serial monitor, pair up the codes coming from your remote to the mpv IPC commands. For new commands look at the mpv documentation for a [list of mpv commands](https://mpv.io/manual/master/#list-of-input-commands) or [etc/input.conf](https://github.com/mpv-player/mpv/blob/master/etc/input.conf) in mpv's github for a more complete list of commands.

### Usage

* Connect arduino to your computer and start mpv. You should be able to control mpv a second after you start the video.
