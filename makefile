CC=gcc
SRCDIR=src
COMPFLAGS = -g -lm -lgsl
# COMPFLAGS += -Wall -Wextra -fsanitize=undefined,address
INPUT_DIR=data
SCORE=temp/Score.txt
LIB=src/count_min_sketch.c  src/auroc.c src/midas.c src/prepare.c src/midasR.c


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
	clear
	$(CC) main.c $(LIB) -o main $(COMPFLAGS)

run: 
	./main $(INPUT_DIR)/Data.csv $(INPUT_DIR)/Meta.txt $(INPUT_DIR)/Label.csv

check:
	 python3 util/EvaluateScore.py $(INPUT_DIR)/Label.csv $(SCORE) 

pre:
	python3 util/PreprocessData.py

exp:
	$(CC) -o test test.c src/count_min_sketch.c -lm; ./test

perf:
	rm -rf midas.svg out.folded out.perf perf.data 
	sudo perf record -F max -a -g ./main data/CIC-DDoS2019/processed/Data.csv data/CIC-DDoS2019/processed/Meta.txt data/CIC-DDoS2019/processed/Label.csv
	sudo perf script > out.perf
	../FlameGraph/stackcollapse-perf.pl out.perf > out.folded
	../FlameGraph/flamegraph.pl out.folded > midas.svg
	open midas.svg

