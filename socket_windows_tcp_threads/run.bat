@echo off

set PATH=C:\MinGW\bin;%PATH%  

gcc Servidor.c -o servidor -lws2_32

gcc Cliente.c -o cliente1 -lws2_32

gcc Cliente.c -o cliente2 -lws2_32


echo Compilacao completa

start cmd title servidor /k "servidor"

timeout /t 10 /nobreak > nul

start cmd title cliente1 /k "cliente1 127.0.0.1"

start cmd title cliente2 /k "cliente2 127.0.0.1"



