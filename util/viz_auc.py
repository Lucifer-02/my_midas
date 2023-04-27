import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Load data
auc = pd.read_csv('measurement/auc.csv')

auc_ctu = pd.read_csv('measurement/auc_ctu.csv', header=None)
auc_darpa = pd.read_csv('measurement/auc_darpa.csv', header=None)
auc_cic2018 = pd.read_csv('measurement/auc_cic2018.csv', header=None)
auc_unsw = pd.read_csv('measurement/auc_unsw.csv', header=None)
auc_iscx = pd.read_csv('measurement/auc_iscx.csv', header=None)
auc_cic2019 = pd.read_csv('measurement/auc_cic2019.csv', header=None)

# print std of each dataset
print('std CTU-13: ', auc_ctu.std()[0])
print('std DARPA: ', auc_darpa.std()[0])
print('std CIC-IDS2018: ', auc_cic2018.std()[0])
print('std ISCX-IDX2012: ', auc_iscx.std()[0])
print('std CIC-DDOS2019: ', auc_cic2019.std()[0])
print('std UNSW-NB15: ', auc_unsw.std()[0])

# plot mean of each dataset using grouped barplot
df = pd.DataFrame({
    'MIDAS-R': [auc['ctu'][0], auc['darpa'][0], auc['cic2018'][0], auc['iscx'][0], auc['cic2019'][0], auc['unsw'][0]],
    'MIDAS-R+': [auc_ctu.mean()[0], auc_darpa.mean()[0], auc_cic2018.mean()[0], auc_iscx.mean()[0], auc_cic2019.mean()[0], auc_unsw.mean()[0]],
}, index=['CTU-13', 'DARPA', 'CIC-IDS2018', 'ISCX-IDX2012', 'CIC-DDOS2019','UNSW-NB15'])

plt.rcParams.update({'font.size': 15})
ax = df.plot.barh(rot=0, figsize=(10, 10))
ax.legend(loc='upper right')
ax.set_xlabel('AUC')
ax.set_ylabel('Tập dữ liệu')


ax.set_xlim(0, 1.5)
ax.bar_label(ax.containers[0], fmt='%.4f')
ax.bar_label(ax.containers[1], fmt='%.4f')

# save plot 
# plt.savefig('viz_auc.png', dpi=300)
plt.show()

