import pandas as pd
from matplotlib import pyplot as plt


# original dataset
ctu = pd.read_csv('measurement/time_ctu.csv', header=None)
darpa = pd.read_csv('measurement/time_darpa.csv', header=None)
cic2018 = pd.read_csv('measurement/time_cic2018.csv', header=None)
iscx = pd.read_csv('measurement/time_iscx.csv', header=None)
unsw = pd.read_csv('measurement/time_unsw.csv', header=None)
cic2019 = pd.read_csv('measurement/time_cic2019.csv', header=None)

# new dataset
ns_ctu = pd.read_csv('measurement/time_ns_ctu.csv', header=None)
ns_darpa = pd.read_csv('measurement/time_ns_darpa.csv', header=None)
ns_cic2018 = pd.read_csv('measurement/time_ns_cic2018.csv', header=None)
ns_iscx = pd.read_csv('measurement/time_ns_iscx.csv', header=None)
ns_unsw = pd.read_csv('measurement/time_ns_unsw.csv', header=None)
ns_cic2019 = pd.read_csv('measurement/time_ns_cic2019.csv', header=None)

# print std of each dataset
print('CTU-13: ', ctu.std()[0])
print('DARPA: ', darpa.std()[0])
print('CIC-IDS2018: ', cic2018.std()[0])
print('ISCX-IDX2012: ', iscx.std()[0])
print('CIC-DDOS2019: ', cic2019.std()[0])
print('UNSW-NB15: ', unsw.std()[0])

# plot mean of each dataset using grouped barplot
df = pd.DataFrame({
    'MIDAS-R': [ctu.mean()[0], darpa.mean()[0], cic2018.mean()[0], iscx.mean()[0], cic2019.mean()[0], unsw.mean()[0]],
    'MIDAS-R+': [ns_ctu.mean()[0], ns_darpa.mean()[0], ns_cic2018.mean()[0], ns_iscx.mean()[0], ns_cic2019.mean()[0], ns_unsw.mean()[0]]
}, index=['CTU-13', 'DARPA', 'CIC-IDS2018', 'ISCX-IDX2012', 'CIC-DDOS2019','UNSW-NB15'])

plt.rcParams.update({'font.size': 15})
ax = df.plot.barh(rot=0, figsize=(10, 10), align='center')
ax.set_xlabel('Thời gian chạy(s)')
ax.set_ylabel('Tập dữ liệu')

# limit x-axis
ax.set_xlim(0, 70)

ax.bar_label(ax.containers[0], fmt='%.4f')
ax.bar_label(ax.containers[1], fmt='%.4f')

# save figure
plt.savefig('viz_time.png',dpi=300, bbox_inches='tight')
plt.show()
