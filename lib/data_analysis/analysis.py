import csv
import pandas as pd
import numpy as np
import numpy.typing as npt
import matplotlib.pyplot as plt
import os
from scipy import stats
#CONVERTS .TXT FILES TO .CSV FORMAT (for the app data w/out feedback)

# Read CSV
input_txt_file = '/content/gdrive/MyDrive/CPR group/Testing/team data/Huda’s cpr data.txt'
output_csv_file = '/content/gdrive/MyDrive/CPR group/Testing/team data/test.csv'

with open(input_txt_file, 'r') as txt_file:
    lines = txt_file.readlines()
delimiter = ','

with open(output_csv_file, 'w', newline='') as csv_file:
    writer = csv.writer(csv_file)

    for line in lines:
        row = line.strip().split(delimiter)
        writer.writerow(row)

# Dataframes.
df_before = pd.read_csv('/content/gdrive/MyDrive/CPR group/Testing/team data/Team data.csv')
df_after = pd.read_csv()

numericalbefore_df = df_before.select_dtypes(include=['number'])
numericalafter_df = df_after.select_dtypes(include=['number'])

resultsdf_before =resultsdf = pd.concat([df_before['Student Name'], numericalbefore_df], axis=1)

resultsdf_before =resultsdf = pd.concat([df_after['Number'], numericalafter_df], axis=1)

print(resultsdf)

# Create a figure and axes for subplots
fig, axs = plt.subplots(2, 2, figsize=(8, 4))

# Plot Volume accuracy (ml)
axs[0, 0].hist(resultsdf['Volume accuracy (ml)'], bins=20, edgecolor='black', alpha=0.7)
axs[0, 0].set_title('Volume accuracy (ml)')
axs[0, 0].set_xlabel('Volume accuracy (ml)')
axs[0, 0].set_ylabel('Frequency')
axs[0, 0].axvline(x=500, color='red', linestyle='--', linewidth=2, label='Ideal Range Lower Bound (500 ml)')
axs[0, 0].axvline(x=600, color='blue', linestyle='--', linewidth=2, label='Ideal Range Upper Bound (600 ml)')
axs[0, 0].legend()
axs[0, 0].spines[['top', 'right']].set_visible(False)

# Plot Depth accuracy (in)
axs[0, 1].hist(resultsdf['Depth accuracy (in)'], bins=20, edgecolor='black', alpha=0.7)
axs[0, 1].set_title('Depth accuracy (in)')
axs[0, 1].set_xlabel('Depth accuracy (in)')
axs[0, 1].set_ylabel('Frequency')
axs[0, 1].axvline(x=2.0, color='red', linestyle='--', linewidth=2, label='Ideal Range Lower Bound (2.0 in)')
axs[0, 1].axvline(x=2.4, color='blue', linestyle='--', linewidth=2, label='Ideal Range Upper Bound (2.4 in)')
axs[0, 1].legend()
axs[0, 1].spines[['top', 'right']].set_visible(False)

# Plot Hands-off time (seconds)
axs[1, 0].hist(resultsdf['Hands-off time (seconds)'], bins=20, edgecolor='black', alpha=0.7)
axs[1, 0].set_title('Hands-off time (seconds)')
axs[1, 0].set_xlabel('Hands-off time (seconds)')
axs[1, 0].set_ylabel('Frequency')
axs[1, 0].axvline(x=1.0, color='red', linestyle='--', linewidth=2, label='Ideal Range Lower Bound (1.0 seconds)')
axs[1, 0].axvline(x=5.0, color='blue', linestyle='--', linewidth=2, label='Ideal Range Upper Bound (5.0 seconds)')
axs[1, 0].legend()
axs[1, 0].spines[['top', 'right']].set_visible(False)

# Plot Rate accuracy (CPM)
axs[1, 1].hist(resultsdf['Rate accuracy (CPM)'], bins=20, edgecolor='black', alpha=0.7)
axs[1, 1].set_title('Rate accuracy (CPM)')
axs[1, 1].set_xlabel('Rate accuracy (CPM)')
axs[1, 1].set_ylabel('Frequency')
axs[1, 1].axvline(x=100, color='red', linestyle='--', linewidth=2, label='Ideal Range Lower Bound (100 CPM)')
axs[1, 1].axvline(x=120, color='blue', linestyle='--', linewidth=2, label='Ideal Range Upper Bound (120 CPM)')
axs[1, 1].legend()
axs[1, 1].spines[['top', 'right']].set_visible(False)

# Adjust layout
plt.tight_layout()

# Show plot
plt.show()

descriptive_statsdfbefore = df_before.describe()
print("Descriptive Statistics: ", descriptive_statsdfbefore)
descriptive_statsdfafter = df_after.describe()
print("Descriptive Statistics: ", descriptive_statsdfafter)

#Null hypothesis is μbefore = μafter, alternate hypothesis is that μbefore != μafter
#If we fail to reject the null hypothesis, that means there's no difference in the means of the pop samples and therefore we can conclude that the feedback device has no impact on bystander cpr performance
#Alternatively and what we want to see is a rejection of the null hypothesis where we accept our alternate which states that there is a difference in the pop samples and therefore the feedback devices does have an impact on bystander cpr performance

#paired t test for compression rate
t_stat, p_value = stats.ttest_rel(df_before['sample1 name'], df_after['sample2 name'])

#paired t test for compression depth
t_stat, p_value = stats.ttest_rel(df_before['sample1 name'], df_after['sample2 name'])

#paired t test for compression recoil
t_stat, p_value = stats.ttest_rel(df_before['sample1 name'], df_after['sample2 name'])

# statistically significant if p-val is >= 0.05, p-val<0.05 reject null hyp
print(f"Paired t-test result:\n t-statistic = {t_stat}\n p-value = {p_value}")