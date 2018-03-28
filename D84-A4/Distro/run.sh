#!/bin/bash
rm logfile
printf "Start training run (0 0 0 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 0 0 0 50 0 2>&1 | tail -14 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5
printf "\n\nStart classification run (1 0 0 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 1 0 0 50 0 2>&1 | tail -12 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5
printf "\n\nStart training run (0 0 1 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 0 0 1 50 0 2>&1 | tail -14 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5
printf "\n\nStart classification run (1 0 1 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 1 0 1 50 0 2>&1 | tail -12 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5
printf "\n\nStart training run (2 150 1 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 2 150 1 50 0 2>&1 | tail -14 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5
printf "\n\nStart classification run (3 150 1 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 3 150 1 50 0 2>&1 | tail -12 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5
printf "\n\nStart training run (2 10 1 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 2 10 1 50 0 2>&1 | tail -14 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5
printf "\n\nStart classification run (3 10 1 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 3 10 1 50 0 2>&1 | tail -12 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5
printf "\n\nStart training run (2 3 1 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 2 3 1 50 0 2>&1 | tail -14 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5
printf "\n\nStart classification run (3 3 1 50 0) ===============================================" 2>&1 >> logfile
./NeuralNets 3 3 1 50 0 2>&1 | tail -12 >> logfile
printf "end run ==============================================================" 2>&1 >> logfile
sleep 5

# TODO custom run
# printf "\n\nStart training run (2 3 1 50 0) ===============================================" 2>&1 >> logfile
# ./NeuralNets 2 3 1 50 0 2>&1 | tail -14 >> logfile
# printf "end run ==============================================================" 2>&1 >> logfile
# sleep 5
# printf "Start classification run (3 3 1 50 0) ===============================================" 2>&1 >> logfile
# ./NeuralNets 3 3 1 50 0 2>&1 | tail -12 >> logfile
# printf "end run ==============================================================" 2>&1 >> logfile
# sleep 5