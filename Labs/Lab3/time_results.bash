#!/bin/bash

ROUNDS=5000
ALGOL=5
ALGOL_EXT=sha256
IP=words10000
EXT=.txt
SEED=333

> time_results_r${ROUNDS}.txt
for T in 1 2 3 5 8 10 13 16 20 25
do
    echo ${T} >> time_results_r${ROUNDS}.txt
    time ( ./thread_crypt -i ${IP}${EXT} -o ${IP}-${T}.${ALGOL_EXT} -a ${ALGOL} -r ${ROUNDS} -R ${SEED} -t ${T} -v)  >> time_results_r${ROUNDS}.txt 2>&1
done
