"""Run this file to analyze data recorded by the Arduino serial monitor. 
This file needs to be run after a valid CSV file has been generated using 
data_acquisition.py. 

Author: Allan Zhou
"""

import matplotlib.pyplot as plt 
import pandas as pd 
import os
from datetime import datetime
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as tk

# Window functionality.
def set_window_position(root, x, y):
    root.update_idletasks()
    root.geometry(f"+{x}+{y}")

def close_window():
    root.quit()
    root.destroy()

# Take data from Serial Monitor (need a better way to export data from MCU to CSV file).
name = input("Name of Participant: ").lower()
date = input("Today? (Y/N): ")
csv_path = ""

if date.lower() == 'y':
    version = input("Version: ")
    csv_path = "lib\\testing_data\\m{}-d{}-y{}\\m{}-d{}-v{}-{}.csv".format(
        datetime.now().month, datetime.now().day, datetime.now().year,  datetime.now().month, datetime.now().day, version, name)
else: 
    month = input("Month: ")
    day = input("Day: ")
    version = input("Version: ")
    csv_path = "lib\\testing_data\\m{}-d{}-y{}\\m{}-d{}-v{}-{}.csv".format(
        month, day, datetime.now().year, month, day, version, name)

df = pd.read_csv(csv_path)

fig, axs = plt.subplots(2, 2, figsize=(12, 8))

# Acceleration
axs[0, 0].plot(df.index, df['AX'], label='Acceleration X')
axs[0, 0].plot(df.index, df['AY'], label='Acceleration Y')
axs[0, 0].plot(df.index, df['AZ'], label='Acceleration Z')
axs[0, 0].grid(True)
axs[0, 0].set_xlabel('Time (ms)')
axs[0, 0].set_ylabel('Acceleration (m/s)')
axs[0, 0].legend()
axs[0, 0].set_title('Acceleration')

# Roll, Pitch, Yaw
axs[0, 1].plot(df.index, df['GX'], label='Roll (X)')
axs[0, 1].plot(df.index, df['GY'], label='Pitch (Y)')
axs[0, 1].plot(df.index, df['GZ'], label='Yaw (Z)')
axs[0, 1].grid(True)
axs[0, 1].set_xlabel('Time (ms)')
axs[0, 1].set_ylabel('Angle (Degrees)')
axs[0, 1].legend()
axs[0, 1].set_title('Gyroscope (Rotation)')

# Position
axs[1, 1].plot(df.index, df['POS'], label='Position')
axs[1, 1].grid(True)
axs[1, 1].set_xlabel('Time (ms)')
axs[1, 1].set_ylabel('Position (mm)')
axs[1, 1].legend()
axs[1, 1].set_title('Compression Depth')

# Rate
# 1 = 100 cycles/min, 1.2 = 120 cycles/min
axs[1, 0].plot(df.index, df['RATE'], label='Rate') 
axs[1, 0].axhline(y=1.0, color='r', linestyle='--', label='Upper Bounds')
axs[1, 0].axhline(y=1.2, color='g', linestyle='--', label='Lower Bounds')
axs[1, 0].grid(True)
axs[1, 0].set_xlabel('Time (ms)')
axs[1, 0].set_ylabel('Rate Factor')
axs[1, 0].legend()
axs[1, 0].set_title('Compression Rate')

fig.suptitle(csv_path, fontsize=16)
plt.grid(True)

root = tk.Tk()
root.title("Matplotlib Plot")

# Set the position of the window (x, y).
set_window_position(root, 0, 0)

# Embed the Matplotlib figure in the tkinter window.
canvas = FigureCanvasTkAgg(fig, master=root)
canvas.draw()
canvas.get_tk_widget().pack()

output_directory = "C:\\Users\\allan\\Documents\\cpr-feedback\\lib\\testing_graphs\\m{}-d{}-y{}".format(
        datetime.now().month, datetime.now().day, datetime.now().year)
os.makedirs(output_directory, exist_ok=True)

graph_number = 1
for filename in os.listdir(output_directory): 
    if filename.endswith('.png'): 
        if 'm{}-d{}-v'.format(datetime.now().month, datetime.now().day) in filename:
            graph_number += 1 

output_path = os.path.join(output_directory, 'm{}-d{}-v{}-{}.png'.format(datetime.now().month, datetime.now().day, graph_number, name))
plt.savefig(output_path)

# Start the tkinter main loop. 
button = tk.Button(root, text="Close", command=close_window)
button.pack()

root.protocol("WM_DELETE_WINDOW", close_window)

root.mainloop()