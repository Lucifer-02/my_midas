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

if __name__ == '__main__':
    main()
