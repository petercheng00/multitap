from evdev import UInput, ecodes as e
import serial
import os
import sys

if (os.getuid() != 0):
    print "please run again in sudo mode"
    sys.exit(0)


ser = serial.Serial("/dev/ttyUSB1", 9600)
ui = UInput()

def charToKey(c):
    if (c == ' '):
        return eval("e.KEY_SPACE")
    elif (c == '<'):
        return eval("e.KEY_BACKSPACE")
    else:
        return eval("e.KEY_" + c.upper())


def main():
    while(ser.isOpen()):
        currVal = ser.read()
        key = charToKey(currVal)
        if (currVal.isupper() and currVal != 8):
            ui.write(e.EV_KEY, e.KEY_LEFTSHIFT, 1)
        ui.write(e.EV_KEY, key, 1)
        ui.write(e.EV_KEY, key, 0)
        if (currVal.isupper() and currVal != 8):
            ui.write(e.EV_KEY, e.KEY_LEFTSHIFT, 0)
        ui.syn()
    ui.close()




if __name__ == "__main__":
    main()
