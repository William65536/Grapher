@ECHO OFF

SET SRC=includes\parser.c
SET OBJ=includes\parser.o
SET COMPILER=gcc
SET FLAGS=-O3 -ffast-math

:: SET BIN=includes\parser

%COMPILER% -c %FLAGS% -o %OBJ% %SRC%
:: %COMPILER% %FLAGS% -o %BIN% %SRC%