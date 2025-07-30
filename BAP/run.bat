@echo off

gcc receiver.c -o receiver -lws2_32

gcc sender.c -o sender -lws2_32

echo Compilacao completa

start cmd title receiver /k "receiver"

timeout /t 2 /nobreak > nul

start cmd title sender /k "sender"

