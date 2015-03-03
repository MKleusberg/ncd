all:
	clang++ -g -std=c++11 -llzma -lboost_system -lboost_filesystem -lpthread -O3 -Wall -o cluster cluster.cpp

run:
	./cluster data/dna data/dna.dat
	Rscript plot.R data/dna.dat
