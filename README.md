# mpv-remote
mpv-remote is a work-in-progress script that allows users to bind keys of an IR remote control to mpv actions. Currently, this also allows for exploring files with netrw.

## Installation
Right now in the experimental stages of the project there will be quite a few hoops to jump through if you want to follow along.

### Operating System
Currently, mpv-remote only supports linux.

### Hardware

* For this project I bought an [Arduino uno kit](https://www.amazon.com/ELEGOO-Project-Tutorial-Controller-Projects/dp/B01D8KOZF4/ref=sr_1_4?keywords=arduino+uno+kit&qid=1681251235&sr=8-4j) but each of the important parts can be purchased separately.

    * [Arduino uno](https://www.amazon.com/ELEGOO-Board-ATmega328P-ATMEGA16U2-Compliant/dp/B01EWOE0UU/ref=sr_1_5?crid=1LAIIDP26SAL3&keywords=elegoo+uno&qid=1681251383&sprefix=elegoo+uno%2Caps%2C105&sr=8-5)

    * [IR receiver](https://www.amazon.com/KOOBOOK-Infrared-Wireless-Control-Receiver/dp/B07S9BQKN2/ref=sr_1_4?keywords=elegoo+ir+receiver&qid=1681251538&sprefix=elegoo+IR+%2Caps%2C94&sr=8-4)

    * [Dupont wires](https://www.amazon.com/Elegoo-EL-CP-004-Multicolored-Breadboard-arduino/dp/B01EV70C78/ref=sr_1_5?crid=18ZDTBTRG1O3F&keywords=female%2Bto%2Bmale%2Bdupont%2Bwires&qid=1681251439&sprefix=female%2Bto%2Bmale%2Bdupont%2Bwires%2Caps%2C100&sr=8-5&th=1)
    * TV remote control: It is better to get a good one of these maybe lying around your house somewhere.

### Software

* Arduino-ide

    * IRremote library: In Arduino IDE, this can be installed at Tools > Manage Libraries... and typing "IRremote" in the search bar and installing the one authored by shirriff.

* Python 3 
    
    * I currently have python 3.10.10 installed.
### Configuration
To enable the IPC server add

```conf
input-ipc-server=/tmp/mpv-socket  
```

to your ~/.config/mpv/mpv.conf

### Arduino setup
Install receiver.ino on the arduino and connect the IR receiver to Ground, 5v and pin 2. Plug the arduino into the computer that you want to control.

### Final steps
Make sure the mpv-remote script has the proper privileges with 

```console
$ chmod 755 mpv-remote
```

Start mpv and in a separate terminal run the mpv-remote script with root privileges.

```console
$ sudo ./mpv-remote

```
