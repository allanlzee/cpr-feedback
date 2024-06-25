"""Run this file to record data from the Arduino serial monitor.
This file needs to be run prior to operation of the device.

Author: Allan Zhou
"""

import serial
import csv
import os
from datetime import datetime

def main():
    try:
        ser = serial.Serial('COM7', 9600)
    except serial.SerialException as e:
        print(f"Error opening serial port: \n{e}")
        return


    output_directory = "C:\\Users\\allan\\Documents\\cpr-feedback\\lib\\testing_data\\m{}-d{}-y{}".format(
        datetime.now().month, datetime.now().day, datetime.now().year
    )

    os.makedirs(output_directory, exist_ok=True)

    name = input("Name of Participant: ").lower()
    graph_number = 1

    for filename in os.listdir(output_directory):
        if filename.endswith('.csv'):
            if 'm{}-d{}-v'.format(datetime.now().month, datetime.now().day) in filename:
                graph_number += 1

    csv_file = os.path.join(output_directory, 'm{}-d{}-v{}-{}.csv'.format(
        datetime.now().month, datetime.now().day, graph_number, name))

    try:
        with open(csv_file, 'w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(['TIME','AX', 'AY', 'AZ', 'POS', 'RATE'])

            while True:
                try:
                    line = ser.readline().decode().strip()
                    print(line)
                    if line:
                        values = line.split(',')
                        writer.writerow(values)
                except KeyboardInterrupt:
                    print("Interrupted by user. Exiting...")
                    break
                except Exception as e:
                    print(f"Error reading from serial port: \n{e}")

    except IOError as e:
        print(f"Error writing to CSV file: \n{e}")

    finally:
        ser.close()
        print("Serial port closed.")

if __name__ == "__main__":
    main()
