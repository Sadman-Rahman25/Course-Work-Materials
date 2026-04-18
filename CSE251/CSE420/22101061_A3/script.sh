#!/bin/bash

yacc -d -y --debug --verbose 22101061.y
echo 'Generated the parser C file as well the header file'
g++ -std=c++11 -w -c -o y.o y.tab.c
echo 'Generated the parser object file'
flex 22101061.l
echo 'Generated the scanner C file'
g++ -std=c++11 -fpermissive -w -c -o l.o lex.yy.c
echo 'Generated the scanner object file'
g++ -std=c++11 y.o l.o -o a.exe
echo 'All ready, running'
./a.exe input.c
echo 'logfile'
cat 22101061_log.txt  
