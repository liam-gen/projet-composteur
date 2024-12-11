from flask import Flask, render_template, jsonify
import serial
import json
import multiprocessing

app = Flask(__name__)

@app.route("/")
def hello_world():
    return render_template('index.html')

@app.get("/api/data/<id>")
def get_data(id):
    f = open("data.json", "r")
    current_data = json.loads(f.read())
    f.close()
    return jsonify(current_data[id])


def runApp():
    app.run(port=8080)

def runGetData():
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
    ser.reset_input_buffer()
    while True:
        if ser.in_waiting > 0:
            #print(ser.readline())
            try:
                line = ser.readline().decode('utf-8').rstrip()
            except UnicodeDecodeError:
                line = ser.readline().decode('utf-8').rstrip()
            try:
                data = json.loads(line)
                if(not data["error"]):
                    f = open("data.json", "r")
                    
                    # Créer si il est vide
                    if(f.read()) == "":
                        f.close()
                        f2 = open("data.json", "w")
                        json.dump("{}", f2)
                        f2.close()
                        current_data = {}
                    else:
                        current_data = json.loads(f.read())
                        f.close()
                    
                    current_data[data["type"]] = data["value"]
                    f2 = open("data.json", "w")
                    json.dump(current_data, f2)
                    f2.close()
                    
            except ValueError as e:
                print("err: "+str(e))

if __name__ == "__main__":

    p1 = multiprocessing.Process(target=runApp)
    p2 = multiprocessing.Process(target=runGetData)

    p1.start()
    p2.start()

    p1.join()
    p2.join()
