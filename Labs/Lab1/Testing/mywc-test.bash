#!/bin/bash

PROG=mywc
CLASS=cs333
POINTS=2
CLEANUP=1
WARNINGS=0
#DIFF_OPTIONS="-B -w -i"
DIFF_OPTIONS="-w"
LAB=Lab1

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

echo "Testing all outputs"
for I in ingredients1 ingredients2
do    
    ${SDIR}/${PROG} -f ${JDIR}/${I}.txt > S-mywc${I}_allf.out
    ${JDIR}/${PROG} -f ${JDIR}/${I}.txt > J-mywc${I}_allf.out

    diff ${DIFF_OPTIONS} S-mywc${I}_allf.out J-mywc${I}_allf.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -f ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -f ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_allf.out J-mywc${I}_allf.out"
    fi

    ${SDIR}/${PROG} < ${JDIR}/${I}.txt > S-mywc${I}_alls.out
    ${JDIR}/${PROG} < ${JDIR}/${I}.txt > J-mywc${I}_alls.out

    diff ${DIFF_OPTIONS} S-mywc${I}_alls.out J-mywc${I}_alls.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} < ${I}.txt (stdin) is good +++"
    else
        echo "  --- ${PROG} < ${I}.txt (stdin) is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_alls.out J-mywc${I}_alls.out"
    fi


    ${SDIR}/${PROG} -l -f ${JDIR}/${I}.txt -c -w > S-mywc${I}_allf2.out
    ${JDIR}/${PROG} -l -f ${JDIR}/${I}.txt -c -w > J-mywc${I}_allf2.out

    diff ${DIFF_OPTIONS} S-mywc${I}_allf2.out J-mywc${I}_allf2.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -l -f ${I}.txt -c -w is good +++"
    else
        echo "  --- ${PROG} -l -f ${I}.txt -c -w is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_allf2.out J-mywc${I}_allf2.out"
    fi

    ${SDIR}/${PROG} -w -l -c < ${JDIR}/${I}.txt > S-mywc${I}_alls2.out
    ${JDIR}/${PROG} -w -l -c < ${JDIR}/${I}.txt > J-mywc${I}_alls2.out

    diff ${DIFF_OPTIONS} S-mywc${I}_alls2.out J-mywc${I}_alls2.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -w -l -c < ${I}.txt (stdin) is good +++"
    else
        echo "  --- ${PROG} -w -l -c < ${I}.txt (stdin) is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_alls2.out J-mywc${I}_alls2.out"
    fi
    echo
done

echo "Testing just -c"
for I in ingredients2 ingredients1
do    
    ${SDIR}/${PROG} -f ${JDIR}/${I}.txt -c > S-mywc${I}_fc.out
    ${JDIR}/${PROG} -f ${JDIR}/${I}.txt -c > J-mywc${I}_fc.out

    diff ${DIFF_OPTIONS} S-mywc${I}_fc.out J-mywc${I}_fc.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -f ${I}.txt -c is good +++"
    else
        echo "  --- ${PROG} -f ${I}.txt -c is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_fc.out J-mywc${I}_fc.out"
    fi
    
    ${SDIR}/${PROG} -c < ${JDIR}/${I}.txt > S-mywc${I}_sc.out
    ${JDIR}/${PROG} -c < ${JDIR}/${I}.txt > J-mywc${I}_sc.out

    diff ${DIFF_OPTIONS} S-mywc${I}_sc.out J-mywc${I}_sc.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -c < ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -c < ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_sc.out J-mywc${I}_sc.out"
    fi
    echo
done

echo "Testing just -w"
for I in ingredients1 ingredients2
do
    ${SDIR}/${PROG} -f ${JDIR}/${I}.txt -w > S-mywc${I}_fw.out
    ${JDIR}/${PROG} -f ${JDIR}/${I}.txt -w > J-mywc${I}_fw.out

    diff ${DIFF_OPTIONS} S-mywc${I}_fw.out J-mywc${I}_fw.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -w -f ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -w -f ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_fw.out J-mywc${I}_fw.out"
    fi
    
    ${SDIR}/${PROG} -w < ${JDIR}/${I}.txt > S-mywc${I}_sw.out
    ${JDIR}/${PROG} -w < ${JDIR}/${I}.txt > J-mywc${I}_sw.out

    diff ${DIFF_OPTIONS} S-mywc${I}_sw.out J-mywc${I}_sw.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -w < ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -w < ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_sw.out J-mywc${I}_sw.out"
    fi
    echo
done

echo "Testing just -l"
for I in ingredients2 ingredients1
do
    ${SDIR}/${PROG} -f ${JDIR}/${I}.txt -l > S-mywc${I}_fl.out
    ${JDIR}/${PROG} -f ${JDIR}/${I}.txt -l > J-mywc${I}_fl.out

    diff ${DIFF_OPTIONS} S-mywc${I}_fl.out J-mywc${I}_fl.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -l -f ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -l -f ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_fl.out J-mywc${I}_fl.out"
    fi
    
    ${SDIR}/${PROG} -l < ${JDIR}/${I}.txt > S-mywc${I}_sl.out
    ${JDIR}/${PROG} -l < ${JDIR}/${I}.txt > J-mywc${I}_sl.out

    diff ${DIFF_OPTIONS} S-mywc${I}_sl.out J-mywc${I}_sl.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -l < ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -l < ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_sl.out J-mywc${I}_sl.out"
    fi
    echo
done

echo "Testing just -c -l"
for I in ingredients1 ingredients2
do
    ${SDIR}/${PROG} -lc -f ${JDIR}/${I}.txt > S-mywc${I}_flc.out
    ${JDIR}/${PROG} -lc -f ${JDIR}/${I}.txt > J-mywc${I}_flc.out

    diff ${DIFF_OPTIONS} S-mywc${I}_flc.out J-mywc${I}_flc.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -lc -f ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -lc -f ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_flc.out J-mywc${I}_flc.out"
    fi
    
    ${SDIR}/${PROG} -cl < ${JDIR}/${I}.txt > S-mywc${I}_slc.out
    ${JDIR}/${PROG} -cl < ${JDIR}/${I}.txt > J-mywc${I}_slc.out

    diff ${DIFF_OPTIONS} S-mywc${I}_slc.out J-mywc${I}_slc.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -cl < ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -cl < ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_slc.out J-mywc${I}_slc.out"
    fi
    echo
done

echo "Testing just -c -w"
for I in ingredients2 ingredients1
do
    ${SDIR}/${PROG} -wcf ${JDIR}/${I}.txt > S-mywc${I}_fwc.out
    ${JDIR}/${PROG} -wcf ${JDIR}/${I}.txt > J-mywc${I}_fwc.out

    diff ${DIFF_OPTIONS} S-mywc${I}_fwc.out J-mywc${I}_fwc.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -wcf ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -wcf ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_fwc.out J-mywc${I}_fwc.out"
    fi
    
    ${SDIR}/${PROG} -w -c < ${JDIR}/${I}.txt > S-mywc${I}_swc.out
    ${JDIR}/${PROG} -w -c < ${JDIR}/${I}.txt > J-mywc${I}_swc.out

    diff ${DIFF_OPTIONS} S-mywc${I}_swc.out J-mywc${I}_swc.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo "  +++ ${PROG} -w -c < ${I}.txt is good +++"
    else
        echo "  --- ${PROG} -w -c < ${I}.txt is sad  ---"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y S-mywc${I}_swc.out J-mywc${I}_swc.out"
    fi
    echo
done

echo "Testing bad command line option"
${SDIR}/${PROG} -Q < ${JDIR}/ingredients1.txt > S-mywcingredients1_q.out 2> S-mywcingredients1_q.err
ERR=$?
if [ ${ERR} -eq 1 ]
then
    echo "  +++ nice job with the exit value +++"
    ((POINTS+=2))
else
    echo "  --- the exit value with a bad command line option should be 1 ---"
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
    rm [JS]-mywc* out.log warn.log
    rm ${PROG}
fi
