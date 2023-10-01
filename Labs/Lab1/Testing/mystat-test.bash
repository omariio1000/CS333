#!/bin/bash

PROG=mystat
CLASS=cs333
POINTS=0
CLEANUP=1
WARNINGS=0
#DIFF_OPTIONS="-B -w -i"
DIFF_OPTIONS="-w -i"
LAB=Lab1
#FUNNIES="FUNNYhlink"
FUNNIES="FUNNYbroken FUNNYdir FUNNYhlink FUNNYnoaccess FUNNYpipe FUNNYregfile FUNNYslink FUNNYsocket"

SDIR=.
JDIR=~rchaney/Classes/${CLASS}/Labs/${LAB}

while getopts "xC" opt
do
    case $opt in
        x)
            # If you really, really, REALLY want to watch what is going on.
            echo "Hang on for a wild ride."
            set -x
            ;;
        C)
            # Skip removal of data files
            CLEANUP=0
            ;;
        \?)
            echo "Invalid option" >&2
            echo ""
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit 1
            ;;
    esac
done

if [ ! -e ${PROG}.c ]
then
    echo "No ${PROG}.c exists. Cannot continue."
    exit 1
fi

gcc -Wall -g -o ${PROG} ${PROG}.c 1> out.log 2> warn.log

if [ $? -ne 0 ]
then
    echo "compilation failed"
    echo "zero points"
    cat out.log warn.log
    exit 1
else
    NUM_BYTES=`wc -c < warn.log`
    if [ ${NUM_BYTES} -ne 0 ]
    then
        WARNINGS=1
        echo "  --- You have compiler warning messages. That is -20 percent!"
    else
        echo "  +++ You have no compiler warning messages. Good job."
        #((POINTS+=2))
    fi
    echo
fi

for I in ${FUNNIES}
do
    ${SDIR}/${PROG} ${JDIR}/${I} > S-mystat${I}.out
    ${JDIR}/${PROG} ${JDIR}/${I} > J-mystat${I}.out

    diff ${DIFF_OPTIONS} S-mystat${I}.out J-mystat${I}.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=5))
        echo "  +++ ${PROG} ${I} is good +++"
    else
        echo "  --- ${PROG} ${I} is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mystat${I}.out J-mystat${I}.out"
    fi
done
echo

${SDIR}/${PROG} ${JDIR}/FUNNY* > S-mystatALL.out
${JDIR}/${PROG} ${JDIR}/FUNNY* > J-mystatALL.out

diff ${DIFF_OPTIONS} S-mystatALL.out J-mystatALL.out > /dev/null
if [ $? -eq 0 ]
then
    ((POINTS+=10))
    echo "  +++ ${PROG} 'FUNNY*' is good +++"
else
    echo "  --- ${PROG} 'FUNNY*' is sad  ---"
    CLEANUP=0
    echo "    try this: diff ${DIFF_OPTIONS} -y S-mystatALL.out J-mystatALL.out"
fi

${SDIR}/${PROG} /dev/mem /dev/vda > S-mystatDEV.out
${JDIR}/${PROG} /dev/mem /dev/vda > J-mystatDEV.out

diff ${DIFF_OPTIONS} S-mystatDEV.out J-mystatDEV.out > /dev/null
if [ $? -eq 0 ]
then
    ((POINTS+=10))
    echo "  +++ ${PROG} /dev/mem /dev/vda is good +++"
else
    echo "  --- ${PROG} /dev/mem /dev/vda is sad  ---"
    CLEANUP=0
    echo "    try this: diff ${DIFF_OPTIONS} -y S-mystatDEV.out J-mystatDEV.out"
fi
echo

echo "You have ${POINTS} points out of 60"
if [ ${WARNINGS} -ne 0 ]
then
    POINTS=$(echo ${POINTS}*.8 | bc)
    echo "But, you have compiler warnings"
    echo "  So, you get this many points ${POINTS}"
fi

if [ ${CLEANUP} -eq 1 ]
then
    rm [JS]-mystat* out.log warn.log
    rm ${PROG}
fi
