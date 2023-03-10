@ECHO OFF

SET SRC=src\main.c
SET OBJ=obj\main.o
SET BIN=bin\main
SET COMPILER=gcc
:: SET FLAGS=-ffast-math -O3
SET FLAGS=-g -Wall -Wextra -Wpedantic -Werror
SET INCLUDES=includes\picture.o includes\parser.o

%COMPILER% -c %FLAGS% -o %OBJ% %SRC%

%COMPILER% %INCLUDES% -o %BIN% %OBJ%

%BIN%

ECHO: & ECHO: & ECHO:

IF ERRORLEVEL 1 GOTO :PROGRAMERROR
IF ERRORLEVEL 0 GOTO :SUCCESS

ECHO SYSTEM ERROR %ERRORLEVEL%
GOTO :EOF

:PROGRAMERROR
ECHO PROGRAM ERROR %ERRORLEVEL%
GOTO :EOF

:SUCCESS
ECHO SUCCESS %ERRORLEVEL%