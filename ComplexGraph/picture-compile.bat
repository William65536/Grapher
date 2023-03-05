@ECHO OFF

SET SRC=includes\picture.c
SET OBJ=includes\picture.o
SET COMPILER=gcc
:: SET FLAGS=-ffast-math -O3
SET FLAGS=-g -Wall -Wextra -Wpedantic -Werror

%COMPILER% -c %FLAGS% -o %OBJ% %SRC%