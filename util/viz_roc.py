# plot roc curve from roc csv 
# usage: python viz_roc.py roc_data.csv

import sys
import numpy as np
import matplotlib.pyplot as plt

def main():
    data = np.genfromtxt(sys.argv[1], delimiter=',', skip_header=1)
    fpr = data[:, 0]
    tpr = data[:, 1]
    plt.plot(fpr, tpr)
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    plt.title('ROC Curve')
    plt.show()

# get 2 csv files from args and plot them
def plot_roc():
    new = np.genfromtxt(sys.argv[1], delimiter=',', skip_header=1)
    origin = np.genfromtxt(sys.argv[2], delimiter=',', skip_header=1)
    fpr1 = new[:, 0]
    tpr1 = new[:, 1]
    fpr2 = origin[:, 0]
    tpr2 = origin[:, 1]
    # plot roc curve with dashed line
    plt.plot(fpr1, tpr1, '--',label='ROC Curve of MIDAS-R+')
    plt.plot(fpr2, tpr2, label='ROC Curve of MIDAS-R')
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    # put legend at bottom right
    plt.legend(loc='lower right')
    plt.show()

if __name__ == '__main__':
    # main()
    plt.rcParams.update({'font.size': 20})
    plot_roc()
