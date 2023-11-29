#!/bin/bash

VERBOSE=0
bGIVEN=0
eGIVEN=0
bVAL=1
eVAL=10

showHelp() {
    echo "$0 [-h] [-b <begin>] [-e <end>] [-v]"
    echo "  Today's date is `date`"
}

while getopts "he:b:v" opt
do
    case $opt in
        h)
            showHelp
            exit 0;
            ;;
        v)
            VERBOSE=1
            ;;
        b)
            bVAL=${OPTARG}
            bGIVEN=1
            ;;
        e)
            eVAL=${OPTARG}
            eGIVEN=1
            ;;
        :)
            echo "Option -$OPTARG requires an argument."
            exit 1
            ;;
    esac
done

if [ "$bGIVEN" -eq 0 ] && [ "$eGIVEN" -eq 0 ]
then
    showHelp
    exit 1;
fi

if [ "$bVAL" -gt "$eVAL" ]
then
    showHelp
    exit 2;
fi

if [ "$VERBOSE" -eq 1 ]
then
    echo "Begin value = $bVAL    End value = $eVAL"
fi

for ((VAL=$bVAL; VAL<=$eVAL; VAL++))
do
    ./foo $VAL
    RESULT=$?
    if [ "$RESULT" -eq 0 ]
    then
        echo "    $VAL was successful"
    elif [ "$VERBOSE" -eq 1 ]
    then
        echo "    $VAL returned $RESULT"
    fi
done

exit 0;