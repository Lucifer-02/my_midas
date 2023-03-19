CC=gcc
SRCDIR=src
COMPFLAGS= -g -lm -Wall -Wextra -Wpedantic -Winline -Wno-long-long 
INPUT_DIR=data
SCORE=temp/Score.txt
LIB=src/count_min_sketch.c  src/auroc.c src/midas.c src/midasR.c src/prepare.c

darpa: INPUT_DIR=data/DARPA/processed
darpa: run 

cic2018: INPUT_DIR=data/CIC-IDS2018/processed
cic2018: run 

cic2019: INPUT_DIR=data/CIC-DDoS2019/processed
cic2019: run 

ctu: INPUT_DIR=data/CTU-13/processed
ctu: run 

iscx: INPUT_DIR=data/ISCX-IDS2012/processed
iscx: run 

unsw: INPUT_DIR=data/UNSW-NB15/processed
unsw: run 


clean:
	rm -r main

com:
	$(CC) main.c $(LIB) -o main $(COMPFLAGS)

run: 
	./main $(INPUT_DIR)/Data.csv $(INPUT_DIR)/Meta.txt $(INPUT_DIR)/Label.csv

check:
	 python3 util/EvaluateScore.py $(INPUT_DIR)/Label.csv $(SCORE) 

pre:
	python3 util/PreprocessData.py

exp:
	gcc -o test test.c src/count_min_sketch.c -lm; ./test

