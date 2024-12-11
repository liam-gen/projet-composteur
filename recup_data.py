import sqlite3
import serial
import datetime

con = sqlite3.connect("database.sql")

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.reset_input_buffer()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)

# Stocker les données dans la base de données
def stock_data(capteur, value):
    cur = con.cursor()
    cur.executemany(f"INSERT INTO {capteur} (value, time) VALUES (?, ?)", [value, datetime.datetime.now()])

"""
import sqlite3
con = sqlite3.connect("database.sql")

cur = con.cursor()
cur.execute("CREATE TABLE CAPT_TEM_HUM(id INT PRIMARY KEY NOT NULL, value VARCHAR(255), time DATETIME)")
"""