import pandas as pd 
import sys

# get the file path is passed as an argument
path = sys.argv[1]
file = path + '/Data.csv'

# read csv file 
df = pd.read_csv(file,header=None)

# print the first 5 rows of column 3 of the dataframe
input = df.iloc[:, 2]
# print(input)

value_counts = input.value_counts()
output_series = input.map(value_counts)

# print(output_series)

# write to csv file
output_series.to_csv(path + '/Density.csv', index=False, header=False)


