#!/usr/bin/python3
import argparse
import time

from hpil_3468A import HP3468A

parser = argparse.ArgumentParser(description='HP-IL Client')
parser.add_argument('--usb_id', required=True, metavar='usb#', type=int, help='USB_ID - `ls /dev/ttyUSB*`')
args = parser.parse_args()

print("start")
multimeter = HP3468A(args.usb_id)
multimeter.setDisplay(multimeter.displayModes.normal)
#time.sleep(.5)
multimeter.setDigits(5)
print(multimeter.readSingleValue())

time.sleep(2)
print("c")
multimeter.setDisplay(multimeter.displayModes.text,  "B")


#time.sleep(1)

print("end")



