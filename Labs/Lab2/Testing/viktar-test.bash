#!/bin/bash

PROG=viktar
CLASS=cs333
POINTS=0
TOTAL_POINTS=215
CLEANUP=1
WARNINGS=0
#DIFF_OPTIONS="-B -w -i"
DIFF_OPTIONS="-w"
FAIL_COUNT=0
LAB=Lab2
VERBOSE=0
CORE_COUNT=0

TO=10s
TOS="-s QUIT --kill-after=60 --preserve-status"

SDIR=.
JDIR=~rchaney/Classes/${CLASS}/Labs/${LAB}
#JDIR=~rchaney/Classes/${CLASS}/Labs/src/viktar

SPROG=${SDIR}/${PROG}
JPROG=${JDIR}/${PROG}

signalCaught() {
    echo "++++ caught signal while running script ++++"
}

signalCtrlC() {
    echo "Caught Control-C"
    echo "You will neeed to clean up some files"
    exit
}

signalSegFault() {
    echo "+++++++ Caught Segmentation Fault from your program! OUCH!  ++++++++"
}

coreDumpMessage()
{
    if [ $1 -eq 139 ]
    then
        echo "      >>> core dump during $2 testing"
        ((CORE_COUNT++))
    elif [ $1 -eq 137 ]
    then
        echo "      >>> core dump during $2 testing"
        ((CORE_COUNT++))
    elif [ $1 -eq 134 ]
    then
        echo "      >>> abort during $2 testing"
        ((CORE_COUNT++))
    elif [ $1 -eq 124 ]
    then
        echo "      >>> timeout during $2 testing"
    #else
        #echo "$1 is a okay"
    fi
    sync
}

copyTestFiles() {
    if [ ${VERBOSE} -eq 1 ]
    then
	    echo ""
	    echo "  Copying test files into current directory"
    fi

    rm -f ${PROG}.h
    ln -s ${JDIR}/${PROG}.h .
    
    rm -f Constitution.txt Iliad.txt jargon.txt text-*k.txt words.txt
    rm -f random-*.bin
    rm -f zeroes-1M.bin zero-sized-file.bin
    rm -f [0-9]-s.txt 

    cp ${JDIR}/*.txt .
    cp ${JDIR}/random-*.bin .
    cp ${JDIR}/zer* .
    cp ${JDIR}/*.viktar .
    cp ${JDIR}/[0-9]-s.txt .
    
	chmod a+r Constitution.txt Iliad.txt jargon.txt words.txt ?-s.txt
	chmod g+wr,o+r,u-w text-*.txt
	chmod a+rx *.bin
	chmod g+w random-333.bin
	chmod a-x zeroes-1M.bin
	chmod o+s zeroes-1M.bin
	chmod g+s zeroes-1M.bin
	chmod u+s random-2M.bin
	chmod a+s random-333.bin
	chmod a+rx zero-sized-file.bin
	touch -t 200110111213.14 text-5k.txt
	touch -t 197009011023.44 text-75k.txt
	touch -t 199912122358.59 words.txt
	touch -t 197805082150.59 Constitution.txt Iliad.txt
	touch -t 202112110217.44 jargon.txt
	touch -t 201202030405.06 zer*.bin
	touch -t 198012050303.06 ran*.bin
	touch -t 199507080910.36 [0-9]-s.txt

    if [ ${VERBOSE} -eq 1 ]
    then
	    echo "    Test files copied. Permissions and dates set."
    fi
    sync ; sync ; sync
}

build() {
    BuildFailed=0

    echo -e "\nBuilding ..."

    rm -f ${PROG}.h
    ln -s ${JDIR}/${PROG}.h .

    make all > /dev/null

    if [ ! -x ${PROG} ]
    then
        BuildFailed=1
    fi
}

testCreateTextFiles() {
    local TEST_FAIL=0

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -c -f test01_S.viktar Constitution.txt 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} Constitution.txt Iliad.txt -f test02_S.viktar jargon.txt words.txt -c 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} [0-9]-s.txt -cf test03_S.viktar text-*k.txt 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi

    ${JPROG} -c -f test01_J.viktar Constitution.txt
    ${JPROG} Constitution.txt Iliad.txt -f test02_J.viktar jargon.txt words.txt -c
    ${JPROG} [0-9]-s.txt -cf test03_J.viktar text-*k.txt 

    echo "Testing with plain text files ..."
    SSUM=$(sum test01_S.viktar | awk '{print $1;}')
    JSUM=$(sum test01_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED single archive member test, with Constitution.txt"
        echo ">>> Fix this before trying more create tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed single archive member test, with Constitution.txt\n\tPOINTS=${POINTS}"

    SSUM=$(sum test02_S.viktar | awk '{print $1;}')
    JSUM=$(sum test02_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED 4 archive members test, with Constitution.txt Iliad.txt jargon.txt words.txt"
        echo ">>> Fix this before trying more create tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed 4 archive members test, with Constitution.txt Iliad.txt jargon.txt words.txt\n\tPOINTS=${POINTS}"

    SSUM=$(sum test03_S.viktar | awk '{print $1;}')
    JSUM=$(sum test03_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED many archive members test, with [0-9]-s.txt text-*k.txt"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed many archive members test, with [0-9]-s.txt text-*k.txt\n\tPOINTS=${POINTS}"

    echo "** Text files with named archive passed."
    echo "   Moving to sending archive to stdout."
    
    # testing stdout

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -c Constitution.txt > test04_S.viktar 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} Constitution.txt Iliad.txt jargon.txt words.txt -c > test05_S.viktar 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} [0-9]-s.txt -c text-*k.txt > test06_S.viktar 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi

    ${JPROG} -c Constitution.txt > test04_J.viktar
    ${JPROG} Constitution.txt Iliad.txt jargon.txt words.txt -c > test05_J.viktar
    ${JPROG} [0-9]-s.txt -c text-*k.txt > test06_J.viktar

    SSUM=$(sum test04_S.viktar | awk '{print $1;}')
    JSUM=$(sum test04_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED single archive member to stdout test, with Constitution.txt"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed single archive member test to stdout, with Constitution.txt\n\tPOINTS=${POINTS}"

    SSUM=$(sum test05_S.viktar | awk '{print $1;}')
    JSUM=$(sum test05_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED 4 archive members to stdout test, with Constitution.txt Iliad.txt jargon.txt words.txt"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed 4 archive members test to stdout, with Constitution.txt Iliad.txt jargon.txt words.txt\n\tPOINTS=${POINTS}"

    SSUM=$(sum test06_S.viktar | awk '{print $1;}')
    JSUM=$(sum test06_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED many archive members to stdout test, with [0-9]-s.txt text-*k.txt"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed many archive members test to stdout, with [0-9]-s.txt text-*k.txt\n\tPOINTS=${POINTS}"

    echo "** Text files with archive to stdout passed."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testCreateBinFiles() {
    local TEST_FAIL=0

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -cf test07_S.viktar random-333.bin 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} random-333.bin -c random-24M.bin -f test08_S.viktar random-2M.bin 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} zeroes-1M.bin random-333.bin random-24M.bin random-2M.bin zero-sized-file.bin -f test09_S.viktar -c 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi

    ${JPROG} -cf test07_J.viktar random-333.bin
    ${JPROG} random-333.bin -c random-24M.bin -f test08_J.viktar random-2M.bin
    ${JPROG} zeroes-1M.bin random-333.bin random-24M.bin random-2M.bin zero-sized-file.bin -f test09_J.viktar -c

    echo "Testing with binary files ..."
    SSUM=$(sum test07_S.viktar | awk '{print $1;}')
    JSUM=$(sum test07_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED single archive member random test, with random-333.bin"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed single archive member random test, with random-333.bin\n\tPOINTS=${POINTS}"

    JSUM=$(sum test08_J.viktar | awk '{print $1;}')
    SSUM=$(sum test08_S.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED 3 archive member random test, with random-333.bin random-24M.bin random-2M.bin"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed 3 archive member random test, with random-333.bin random-24M.bin random-2M.bin\n\tPOINTS=${POINTS}"

    SSUM=$(sum test09_S.viktar | awk '{print $1;}')
    JSUM=$(sum test09_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED 3 archive member random test, with "
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed 5 archive member random test, with\n\t\tzeroes-1M.bin random-333.bin random-24M.bin random-2M.bin zero-sized-file.bin\n\tPOINTS=${POINTS}"

    echo "   Binary files with named archive passed."
    echo "   Moving to sending archive to stdout."

    # testing stdout
    
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -c random-333.bin > test10_S.viktar 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} random-333.bin random-24M.bin -c random-2M.bin > test11_S.viktar 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} random-333.bin zeroes-1M.bin random-24M.bin random-2M.bin zero-sized-file.bin -c > test12_S.viktar 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi

    ${JPROG} -c random-333.bin > test10_J.viktar
    ${JPROG} random-333.bin random-24M.bin -c random-2M.bin > test11_J.viktar
    ${JPROG} random-333.bin zeroes-1M.bin random-24M.bin random-2M.bin zero-sized-file.bin -c > test12_J.viktar

    SSUM=$(sum test10_S.viktar | awk '{print $1;}')
    JSUM=$(sum test10_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED single archive member random to stdout, with random-333.bin"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed single archive member random test to stdout, with random-333.bin\n\tPOINTS=${POINTS}"

    SSUM=$(sum test11_S.viktar | awk '{print $1;}')
    JSUM=$(sum test11_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED 3 archive member random to stdout, with random-333.bin random-24M.bin random-2M.bin"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed 3 archive member random test to stdout, with random-333.bin random-24M.bin random-2M.bin\n\tPOINTS=${POINTS}"

    SSUM=$(sum test12_S.viktar | awk '{print $1;}')
    JSUM=$(sum test12_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED 3 archive member random to stdout, with random-*.bin"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=5))
    echo -e "\tPassed 5 archive member random test to stdout, with\n\t\tzeroes-1M.bin random-333.bin random-24M.bin random-2M.bin zero-sized-file.bin\n\tPOINTS=${POINTS}"
    
    echo "** Binary files to stdout passed."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testLongName() {
    local TEST_FAIL=0

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -cf test13_S.viktar random-333-with-a-long-name.bin 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -c random-24M.bin random-333-with-a-long-name.bin random-2M.bin -f test14_S.viktar 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -c random-24M.bin -f test15_S.viktar random-333-with-a-long-name.bin 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi

    ${JPROG} -c -f test13_J.viktar random-333-with-a-long-name.bin
    ${JPROG} -c -f test14_J.viktar random-24M.bin random-333-with-a-long-name.bin random-2M.bin
    ${JPROG} -c -f test15_J.viktar random-24M.bin random-333-with-a-long-name.bin

    echo "Testing with over-long file name ..."
    SSUM=$(sum test13_S.viktar | awk '{print $1;}')
    JSUM=$(sum test13_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED long name file, 1 member"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed single over-long file, with random-333-with-a-long-name.bin\n\tPOINTS=${POINTS}"

    JSUM=$(sum test14_J.viktar | awk '{print $1;}')
    SSUM=$(sum test14_S.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED long name member with 2 other members"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed 3 over-long file, with random-24M.bin random-333-with-a-long-name.bin random-2M.bin\n\tPOINTS=${POINTS}"

    SSUM=$(sum test15_S.viktar | awk '{print $1;}')
    JSUM=$(sum test15_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED long name member with 1 other member"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=10))
    echo -e "\tPassed 2 over-long file, with random-333-with-a-long-name.bin random-24M.bin\n\tPOINTS=${POINTS}"

    echo "** Long name file passed."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testBigArchive()
{
    local TEST_FAIL=0

    # must use bash due to file spec
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -c -f test16_S.viktar *.{txt,bin} > /dev/null 2>&1" ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    ${JPROG} -c -f test16_J.viktar *.{txt,bin}

    echo "Testing with BIG archive file ..."
    SSUM=$(sum test16_S.viktar | awk '{print $1;}')
    JSUM=$(sum test16_J.viktar | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED big archive with *.{txt,bin}"
        echo ">>> Fix this before trying more tests"
        return
    fi
    ((POINTS+=5))
    echo -e "\tPassed big archive, with *.{txt,bin}\n\tPOINTS=${POINTS}"

    echo "** Big archive file passed."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testExtractTextFiles() {
    echo "Testing extract from archive file ..."
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -c *.txt -f testX01_S.viktar > /dev/null 2>&1" ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    ${JPROG} -c *.txt -f testX01_J.viktar > /dev/null 2>&1

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -c *.bin -f testX02_S.viktar > /dev/null 2>&1" ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    ${JPROG} -c *.bin -f testX02_J.viktar > /dev/null 2>&1

    rm -f *.txt
    ${SPROG} -xf testX01_S.viktar
    ls -lAuq *.txt > testX01_S.out
    stat --printf="\tfile name: %n\n\t\tmode:  %A\n\t\tuser:  %U\n\t\tgroup: %G\n\t\tsize:  %s\n\t\tmtime: %y\n\t\tatime: %x\n" *.txt > testX01_S.sout

    rm -f *.txt
    ${JPROG} -xf testX01_J.viktar
    ls -lAuq *.txt > testX01_J.out
    stat --printf="\tfile name: %n\n\t\tmode:  %A\n\t\tuser:  %U\n\t\tgroup: %G\n\t\tsize:  %s\n\t\tmtime: %y\n\t\tatime: %x\n" *.txt > testX01_J.sout

    JSUM=$(sum testX01_J.sout | awk '{print $1;}')
    SSUM=$(sum testX01_S.sout | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED extract with text files"
        echo ">>> Fix this before trying more tests"
        echo ">>> "
        return
    fi
    ((POINTS+=20))
    echo -e "\tPassed extract with text files\n\tPOINTS=${POINTS}"

    rm -f *.bin
    ${SPROG} -xf testX02_S.viktar
    ls -lAuq *.bin > testX02_S.out
    stat --printf="\tfile name: %n\n\t\tmode:  %A\n\t\tuser:  %U\n\t\tgroup: %G\n\t\tsize:  %s\n\t\tmtime: %y\n\t\tatime: %x\n" *.bin > testX02_S.sout

    rm -f *.bin
    ${JPROG} -xf testX02_J.viktar
    ls -lAuq *.bin > testX02_J.out
    stat --printf="\tfile name: %n\n\t\tmode:  %A\n\t\tuser:  %U\n\t\tgroup: %G\n\t\tsize:  %s\n\t\tmtime: %y\n\t\tatime: %x\n" *.bin > testX02_J.sout

    JSUM=$(sum testX02_J.sout | awk '{print $1;}')
    SSUM=$(sum testX02_S.sout | awk '{print $1;}')
    if [ ${JSUM} -ne ${SSUM} ]
    then
        echo ">>> FAILED extract with binary files"
        echo ">>> Fix this before trying more tests"
        echo ">>> "
        return
    fi
    ((POINTS+=20))
    echo -e "\tPassed extract with binary files\n\tPOINTS=${POINTS}"

    echo "** Extract from archive file passed."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"

}

testSmallTOC()
{
    echo "Testing short toc ..."

    #{ timeout ${TOS} ${TO} sh -c "exec ${SPROG} -vtf simple_text.viktar 2> test01_S.err | sed -e s+\"++g > test01_S.out" ; }
    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -vtf simple_text.viktar 2> test01_S.err > test01_S.out" ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    sed -e s+\"++g test01_S.out > JUNK ; mv JUNK test01_S.out
    ${JPROG} -vtf simple_text.viktar 2> test01_J.err | sed -e s+\"++g > test01_J.out

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -v -v -v -f long_name.viktar -v -v -t -v -v > test02_S.out 2> test02_S.err" ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    sed -e s+\"++g test02_S.out > JUNK ; mv JUNK test02_S.out
    ${JPROG} -tf long_name.viktar | sed -e s+\"++g > test02_J.out 2> test02_J.err

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -v -v -vv -t -vv -f bin_files.viktar -vv -v > test03_S.out 2> test03_S.err" ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    sed -e s+\"++g test03_S.out > JUNK ; mv JUNK test03_S.out
    ${JPROG} -tf bin_files.viktar | sed -e s+\"++g > test03_J.out 2> test03_J.err

    diff ${DIFF_OPTIONS} test01_S.out test01_J.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=5))
        echo -e "\tshort toc on simple_text.viktar is good"
    else
        echo ">>> short toc on simple_text.viktar is sad"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y test01_S.out test01_J.out"
    fi

    diff ${DIFF_OPTIONS} test02_S.out test02_J.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo -e "\tshort toc on long_name.viktar is good"
    else
        echo ">>> short toc on long_name.viktar is sad"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y test02_S.out test02_J.out"
    fi

    diff ${DIFF_OPTIONS} test01_S.out test01_J.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=2))
        echo -e "\tshort toc on bin_files.viktar is good"
    else
        echo ">>> short toc on bin_files.viktar is sad"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y test03_S.out test03_J.out"
    fi

    if [ -s test01_S.err ]
    then
        ((POINTS+=2))
        echo -e "\tverbose output good"
    else
        echo "  verbose output is sad"
    fi

    echo "** Short toc done..."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testBigTOC()
{
    echo "Testing long TOC ..."

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -v -v -v -v -f simple_text.viktar -T > test04_S.out 2> test04_S.err" ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    sed -e s+\"++g test04_S.out > JUNK ; mv JUNK test04_S.out
    ${JPROG} -Tf simple_text.viktar | sed -e s+\"++g > test04_J.out 2> test04_J.err

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -T -v -v -v -v -f long_name.viktar > test05_S.out 2> test05_S.err" ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    sed -e s+\"++g test05_S.out > JUNK ; mv JUNK test05_S.out
    ${JPROG} -Tf long_name.viktar | sed -e s+\"++g > test05_J.out 2> test05_J.err

    { timeout ${TOS} ${TO} sh -c "exec ${SPROG} -Tf bin_files.viktar -v -v -v -v > test06_S.out 2> test06_S.err" ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    sed -e s+\"++g test06_S.out > JUNK ; mv JUNK test06_S.out
    ${JPROG} -Tf bin_files.viktar | sed -e s+\"++g > test06_J.out 2> test06_J.err
    
    diff ${DIFF_OPTIONS} test04_S.out test04_J.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=3))
        echo -e "\tlong toc on simple_text.viktar is good"
    else
        echo ">>> long toc on simple_text.viktar is sad"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y test04_S.out test04_J.out"
    fi

    diff ${DIFF_OPTIONS} test05_S.out test05_J.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=3))
        echo -e "\tlong toc on long_name.viktar is good"
    else
        echo ">>> long toc on long_name.viktar is sad"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y test05_S.out test05_J.out"
    fi

    diff ${DIFF_OPTIONS} test06_S.out test06_J.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=3))
        echo -e "\tlong toc on bin_files.viktar is good"
    else
        echo ">>> long toc on bin_files.viktar is sad"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y test06_S.out test06_J.out"
    fi

    echo "** Long toc done..."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testHelp()
{
    echo "Testing help text ..."

    ${SPROG} -h 2>&1 | grep -v viktar > test07_S.out
    ${JPROG} -h 2>&1 | grep -v viktar > test07_J.out

    diff ${DIFF_OPTIONS} test07_S.out test07_J.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=5))
        echo -e "\thelp text is good"
    else
        echo ">>> help text needs help"
        CLEANUP=0
        echo "    try this: diff ${DIFF_OPTIONS} -y test07_S.out test07_J.out"
    fi

    echo "** Help text done..."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

cleanTestFiles()
{
    if [ ${CLEANUP} -eq 1 ]
    then
        rm -f Constitution.txt Iliad.txt jargon.txt text-*k.txt words.txt
        rm -f [0-9]-s.txt
        rm -f random-*.bin
        rm -f zeroes-1M.bin zero-sized-file.bin

        rm -f test[01][0-9]_[JS].viktar

        rm -f test0[0-9]_[JS].{toc,err}

        rm -f testX0[0-9]_[JS].viktar

        rm -f *.out *.sout *.viktar

        make clean
    fi
}

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

build
if [ ${BuildFailed} -ne 0 ]
then
    echo "Since the program build failed (using make), ending test"
    echo "Points = 0"
    exit 1
fi

trap 'signalCaught;' SIGTERM SIGQUIT SIGKILL SIGSEGV
trap 'signalCtrlC;' SIGINT
#trap 'signalSegFault;' SIGCHLD

rm -f test_[0-9][1-9]_[JS].viktar

copyTestFiles

testHelp

testSmallTOC
testBigTOC

testCreateTextFiles
testCreateBinFiles
testLongName
testBigArchive


testExtractTextFiles
##testExtractBinFiles

##testBadFile

# stat --printf="\tfile name: %n\n\t\tmode:  %A\n\t\tuser:  %U\n\t\tgroup: %G\n\t\tsize:  %s\n\t\tmtime: %y\n\t\tatime: %x\n\t\tctime: %z\n" [1-3]-s.txt

cleanTestFiles
