#!/bin/bash

set -e  # Ferma lo script se un comando fallisce

echo "Pulizia dei file oggetto..."
rm -f *.o

CXXFLAGS="-O3 -funroll-loops -mavx -march=native"

echo "Compilazione dei sorgenti C++..."
g++ $CXXFLAGS -c CppSQLite3.cpp
g++ $CXXFLAGS -c KVDatabase.cpp
g++ $CXXFLAGS -c main.cpp
g++ $CXXFLAGS -c Server.cpp

echo "Compilazione di sqlite3.c con gcc..."
gcc -O3 -funroll-loops -mavx -march=native -c sqlite3.c

echo "Linking finale..."
g++ CppSQLite3.o KVDatabase.o main.o Server.o sqlite3.o -o main -lpthread -ldl

echo "Build completato con successo!"

