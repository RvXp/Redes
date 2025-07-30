@echo off
set PATH=C:\MinGW\bin;%PATH% 

gcc Server.c -o servidor -lws2_32

gcc Client.c -o cliente -lws2_32

echo Compilacao completa

start cmd title servidor /k "servidor"

timeout /t 10 /nobreak > nul

start cmd title cliente /k "cliente"

