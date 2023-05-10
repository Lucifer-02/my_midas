import pandas as pd 
import matplotlib.pyplot as plt

# Read in the data
score_plus = pd.read_csv('output/Score_plus.csv', header=None)
score = pd.read_csv('output/Score.csv', header=None)

# Plot the scores in 2 subplots vertically with x-axis as the number of iterations
plt.rcParams.update({'font.size': 17})
fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, sharey=True)
ax1.plot(score_plus, color='darkorange')
ax1.set_title('MIDAS-R+')
ax1.set_ylabel('Điểm bất thường')
ax2.plot(score)
ax2.set_title('MIDAS-R')
ax2.set_ylabel('Điểm bất thường')
plt.xlabel('Số thứ tự gói tin')
# save the figure
plt.savefig('viz_score.png', dpi=300, bbox_inches='tight')
plt.show()
