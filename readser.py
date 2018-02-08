import serial
import time
import csv
#ser = serial.Serial("COM13", 9600)
#ser.close()
#time.sleep(15)
#try:
#    ser.open()
#    
#    ser.write(chr(10))
#except:
#    print "nope"

with open('Data4display.csv') as myFile:
    reader = csv.DictReader(myFile)
    for row in reader:
        print(row)