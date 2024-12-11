from flask import Flask, render_template, jsonify
import serial
import json
import multiprocessing

app = Flask(__name__)
values = {}

@app.route("/")
def hello_world():
    return render_template('index.html')

@app.route("/api/data/:id")
def get_data(capteur):
    return jsonify(values[capteur])


def runApp():
    app.run(port=8080)

def runGetData():
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.reset_input_buffer()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            data = json.loads(line)
            if(not data["error"]):
                values[data["type"]] = data["value"]

if __name__ == "__main__":

    p1 = multiprocessing.Process(target=runApp)
    p2 = multiprocessing.Process(target=runGetData)

    p1.start()
    p2.start()

    p1.join()
    p2.join()

    print("Done!")