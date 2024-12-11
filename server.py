from flask import Flask, render_template, jsonify
import serial
import json

app = Flask(__name__)

values = {}

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.reset_input_buffer()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            data = json.loads(line)
            if(not data["error"]):
                values[data["type"]] = data["value"]

@app.route("/")
def hello_world():
    return render_template('index.html')

@app.route("/api/data/:id")
def get_data(capteur):
    return jsonify(values[capteur])

app.run(port=8080)