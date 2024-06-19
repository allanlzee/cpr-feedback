import matplotlib.pyplot as plt 
import pandas as pd 
import os
from datetime import datetime

# Take data from Serial Monitor (need a better way to export data from MCU to CSV file)
name = input("Name of Participant: ").lower()

df = pd.read_csv("C:\\Users\\allan\\Documents\\cpr-feedback\\lib\\testing-{}.csv".format(name))

plt.figure(figsize=(10, 6))

plt.plot(df.index, df['AX'], marker='o', label='Acceleration X')
plt.plot(df.index, df['AY'], marker='o', label='Acceleration Y')
plt.plot(df.index, df['AZ'], marker='o', label='Acceleration Z')

plt.title("100 Hz Acceleration of MPU9520 (Chest Compressions)")
plt.xlabel("Time (ms)")
plt.ylabel("Acceleration (m/s)")

plt.legend() 
plt.grid(True)

output_directory = "C:\\Users\\allan\\Documents\\cpr-feedback\\lib\\testing_graphs"
os.makedirs(output_directory, exist_ok=True)

graph_number = 1
for filename in os.listdir(output_directory): 
    if filename.endswith('.png'): 
        if 'm{}-d{}-v'.format(datetime.now().month, datetime.now().day) in filename:
            graph_number += 1 

output_path = os.path.join(output_directory, 'm{}-d{}-v{}-{}.png'.format(datetime.now().month, datetime.now().day, graph_number, name))
plt.savefig(output_path)

plt.show()