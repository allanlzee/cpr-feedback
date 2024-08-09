"""This file generates a graph based on CSV data on CPR quality between 
feedback and non-feedback study trials. 

Author: Allan Zhou 
"""

import matplotlib.pyplot as plt 
import pandas as pd 

def main():
    csv_path = "C:\\Users\\allan\\Documents\\cpr-feedback\\lib\\participant_data\\feedback_analysis\\feedback.csv"

    df = pd.read_csv(csv_path, skiprows=1)

    # Each graph will be for one statistic, with all participants being plotted.
    x_axis = ["Chest Compression Fraction", "Compression Rate Accuracy", "Compression Depth Accuracy", "Compression Recoil Accuracy", "Hands-off Time"]

    participants = [] 
    for i in range(int(df.shape[0])): 
        participants.append(i + 1)

    print(df)

    # Graph generation. 
    


    

if __name__ == "__main__": 
    main()