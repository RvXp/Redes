#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "assinaturas.h"
#include "config.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <ip_servidor>\n", argv[0]);
        exit(1);
    }

    WSADATA wsaData; 
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { 
        printf("Erro na inicializacao do Winsock.\n");
        return 1;
    }

    int socket_cliente;
    struct sockaddr_in endereco_servidor;
    char buffer[TAM_BUFFER];
    int n;
    const char *ip_servidor = argv[1];
    socket_cliente = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(socket_cliente < 0){
        perror("Erro na ciacao do socket");
        exit(1);
    }

    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = inet_addr(ip_servidor);
    endereco_servidor.sin_port = htons(PORTA);

    if(connect(socket_cliente, (struct sockaddr *)&endereco_servidor, sizeof(endereco_servidor)) < 0){
        perror("Erro na conexao com o servidor");
        close(socket_cliente);
        exit(1);
    }
    

    printf("Conexao com o servidor estabelecida.\n");

    while(1){
        printf("Cliente: ");
        fgets(buffer, TAM_BUFFER, stdin);
        send(socket_cliente, buffer, strlen(buffer), 0);
        n = recv(socket_cliente, buffer, TAM_BUFFER, 0);
        if(n <= 0)  {
            printf("Conexao com o servidor fechada");
            break;
        }
        buffer[n] = '\0';
        printf("Servidor: %s", buffer);
    }
    closesocket(socket_cliente);
    return 0;
}
