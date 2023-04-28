CC=gcc
SRCDIR=src
COMPFLAGS = -lm -lgsl -mavx 
# COMPFLAGS += -Wall -Wextra -fsanitize=undefined,address
INPUT_DIR=data
SCORE=output/Score.txt
LIB=src/count_min_sketch.c  src/nitro_sketch.c src/auroc.c src/midas.c src/prepare.c src/midasR.c

DEFINES = -DFULL=1
DEFINES += -DAUC=1
DEFINES += -DMIDAS_PLUS=0
DEFINES += -DMIDAS=0
DEFINES += -DMIDAS_R=1
DEFINES += -DMIDAS_R_PLUS=0 


darpa: INPUT_DIR=data/DARPA/processed
darpa: run

darpa2: INPUT_DIR=data/DARPA2/processed
darpa2: run 


cic2018: INPUT_DIR=data/CIC-IDS2018/processed
cic2018: run check

cic2019: INPUT_DIR=data/CIC-DDoS2019/processed
cic2019: run check

ctu: INPUT_DIR=data/CTU-13/processed
ctu: run check

iscx: INPUT_DIR=data/ISCX-IDS2012/processed
iscx: run check

unsw: INPUT_DIR=data/UNSW-NB15/processed
unsw: run check


clean:
	rm -r main
	rm ./output/Score.txt
	# rm -rf midas.svg out.folded out.perf perf.data 

com: 
	clear
	$(CC) main.c $(LIB) -o main $(COMPFLAGS) $(DEFINES)

run: 
	./main $(INPUT_DIR)/Data.csv $(INPUT_DIR)/Meta.txt $(INPUT_DIR)/Label.csv $(INPUT_DIR)/Density.csv

check:
	 python3 util/EvaluateScore.py $(INPUT_DIR)/Label.csv $(SCORE) 

pre:
	python3 util/PreprocessData.py

exp:
	$(CC) -o test test.c src/count_min_sketch.c -lm; ./test

flame:
	rm -rf midas.svg out.folded out.perf perf.data 
	sudo perf record -F max -a -g ./main data/CIC-DDoS2019/processed/Data.csv data/CIC-DDoS2019/processed/Meta.txt data/CIC-DDoS2019/processed/Label.csv data/CIC-DDoS2019/processed/Density.csv 
	sudo perf script > out.perf
	../FlameGraph/stackcollapse-perf.pl out.perf > out.folded
	../FlameGraph/flamegraph.pl out.folded > midas.svg
	open midas.svg
stat:
	sudo perf stat -a ./main data/CIC-DDoS2019/processed/Data.csv data/CIC-DDoS2019/processed/Meta.txt data/CIC-DDoS2019/processed/Label.csv data/CIC-DDoS2019/processed/Density.csv
	
roc:
	python3 util/ReproduceROC.py $(INPUT_DIR)/Label.csv $(SCORE)
	python3 util/viz_roc.py output/ROC.csv

darpa_roc:
	python3 util/viz_roc.py output/ROC_plus.csv output/ROC.csv

auc:
	python3 util/viz_auc.py

time:
	python3 util/viz_time.py

score:
	python3 util/viz_scores.py
