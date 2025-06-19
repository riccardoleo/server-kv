#!/bin/bash

set -e  # Ferma lo script se un comando fallisce

echo "Pulizia dei file oggetto..."
rm -f *.o

echo "Compilazione dei sorgenti C++..."
g++ -c CppSQLite3.cpp
g++ -c KVDatabase.cpp
g++ -c main.cpp
g++ -c Server.cpp

echo "Compilazione di sqlite3.c con gcc..."
gcc -c sqlite3.c

echo "Linking finale..."
g++ CppSQLite3.o KVDatabase.o main.o Server.o sqlite3.o -o main -lpthread -ldl

echo "Build completato con successo!"

