# plot roc curve from roc csv 
# usage: python viz_roc.py roc_data.csv

import sys
import matplotlib.pyplot as plt
import pandas as pd

def main():
    data = np.genfromtxt(sys.argv[1], delimiter=',', skip_header=1)
    fpr = data[:, 0]
    tpr = data[:, 1]
    plt.plot(fpr, tpr)
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    plt.title('ROC Curve')
    plt.show()

def plot_roc():
    # read csv files then plot roc curve
    new = pd.read_csv('output/ROC_plus.csv', header=None)
    origin = pd.read_csv('output/ROC.csv', header=None)

    # get fpr and tpr 
    fpr1 = new.iloc[:, 0]
    tpr1 = new.iloc[:, 1]
    fpr2 = origin.iloc[:, 0]
    tpr2 = origin.iloc[:, 1]

    # plot roc curve with dashed line
    plt.plot(fpr1, tpr1, '--',label='ROC Curve of MIDAS-R+')
    plt.plot(fpr2, tpr2, label='ROC Curve of MIDAS-R')
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    # put legend at bottom right
    plt.legend(loc='lower right')
    # save the figure
    plt.savefig('viz_roc.png', dpi=300, bbox_inches='tight')
    plt.show()

if __name__ == '__main__':
    # main()
    plt.rcParams.update({'font.size': 20})
    plot_roc()
