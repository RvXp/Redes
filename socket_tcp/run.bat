@echo off

set PATH=C:\MinGW\bin;%PATH% 

gcc Servidor.c -o servidor -lws2_32

gcc Cliente.c -o cliente -lws2_32

echo Compilacao completa

start cmd title servidor /k "servidor"

timeout /t 10 /nobreak > nul

start cmd title cliente /k "cliente 127.0.0.1"

