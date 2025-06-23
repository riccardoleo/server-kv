#!/bin/bash

set -e  # Ferma lo script se un comando fallisce

echo "Pulizia dei file oggetto..."
rm -f *.o main

echo "Compilazione dei sorgenti C++ con flag -g..."
g++ -g -c CppSQLite3.cpp
g++ -g -c KVDatabase.cpp
g++ -g -c main.cpp
g++ -g -c Server.cpp

echo "Compilazione di sqlite3.c con gcc e flag -g..."
gcc -g -c sqlite3.c

echo "Linking finale..."
g++ -g CppSQLite3.o KVDatabase.o main.o Server.o sqlite3.o -o main -lpthread -ldl

echo "Build completato con successo!"
