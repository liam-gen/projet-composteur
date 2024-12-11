"""
https://roboticsbackend.com/raspberry-pi-arduino-serial-communication/

When connecting the Arduino with a USB cable, you should see it appear as /dev/ttyACM0, or /dev/ttyUSB0 (sometimes the number can be different, for example /dev/ttyACM1).

Simply run ls /dev/tty*
and you should see it. At this point if you’re not sure which device is the Arduino board, simply disconnect the board (remove the USB cable), and run ls /dev/tty*
again. This way you will easily spot the serial device name of your Arduino.
"""

#python3 -m pip install pyserial
import serial
if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.reset_input_buffer()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)


"""import sqlite3
con = sqlite3.connect("database.sql")

cur = con.cursor()
cur.execute("CREATE TABLE CAPT_TEM_HUM(id INT PRIMARY KEY NOT NULL, value VARCHAR(255), time DATETIME)")

def stock_data(capteur, value):
    cur = con.cursor()
    cur.execute("CREATE TABLE movie(title, year, score)")"""
