import pandas as pd 
from scipy import stats

csv_path = "C:\\Users\\allan\\Documents\\cpr-feedback\\lib\\data_analysis\\participant-data - trials.csv"

df = pd.read_csv(csv_path, skiprows=1) 

print("\n-----------------PAIRED T TESTS-----------------")
# Compression Fraction 
c_frac = stats.ttest_rel(df['CCF No Feedback'], df['CCF Feedback'])
print("Compression Fraction: {}".format(c_frac))

# Compression Rate 
c_rate = stats.ttest_rel(df['CRA No Feedback'], df['CRA Feedback'])
print("Compression Rate: {}".format(c_rate))

# Compression Depth 
c_depth = stats.ttest_rel(df['CDA No Feedback'], df['CDA Feedback'])
print("Compression Depth: {}".format(c_depth))

# Compression Recoil
c_recoil = stats.ttest_rel(df['CReA No Feedback'], df['CReA Feedback'])
print("Compression Recoil: {}".format(c_recoil)) 

# Hands Off Time 
c_hands = stats.ttest_rel(df['Hands No Feedback'], df['Hands Feedback'])
print("Hands Off Time: {}".format(c_hands))

print()

"""Need either: 
1) A look up table for T values based on number of participants.
2) Software.
"""