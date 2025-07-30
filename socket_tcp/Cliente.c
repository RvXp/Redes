#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "assinaturas.h"
#include "config.h"
/*Adaptação da implementação de socket
    creado por Diego Molinos para uso academico
*/

int main(int argc, char *argv[]){
    // Os argumento passados na main obrigam o cliente a passar o parametro necessario para execução, nesse caso o endereço do servido, sendo executado no locahost o endereço a ser passado na execuação é 127.0.0.1
    if(argc != 2){
        fprintf(stderr, "Usage: %s <ip_servidor>\n", argv[0]);
        exit(1);
    }

    WSADATA wsaData; // Crie um objeto WSADATA chamado wsaData.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // Chame WSAStartup e retorne seu valor como um inteiro e marcar para erros.
        /*  A WSAStartup possui o seguinte cabeçalho
        int WSAStartup(
            [in]  WORD      wVersionRequired,   A versão mais alta da especificação do Windows Sockets que o chamador pode usar.
            [out] LPWSADATA lpWSAData           Um ponteiro para a estrutura de dados WSADATA que deve receber detalhes da implementação do Windows Sockets.
        );
        */
        printf("Erro na inicializacao do Winsock.\n");
        return 1;
    }

    int socket_cliente;
    struct sockaddr_in endereco_servidor;
    /* A struct sockaddr_in é contruida da seguinte forma:
        struct sockaddr_in {
            short            sin_family;   A família de endereços para o endereço de transporte.
            unsigned short   sin_port;     Um número da porta do protocolo de transporte.
            struct in_addr   sin_addr;     Uma estrutura IN_ADDR que contém um endereço de transporte IPv4.
            char             sin_zero[8];   Reservado para uso do sistema. 
        };
    */
    char buffer[TAM_BUFFER];
    int n;
    const char *ip_servidor = argv[1];

    // Criação do socket 
    socket_cliente = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // AF_INET se refere ao tipo de endereço, no caso IPv4
    // SOCK_STREAM define o tipo de socket a ser criado, no caso TCP
    // IPPROTO_TCP define o tipo de protocolo utilizado no socket, no caso TCP

    if(socket_cliente < 0){
        perror("Erro na ciacao do socket");
        exit(1);
    }

    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = inet_addr(ip_servidor);
    // Esta linha configura o servidor para escutar apenas conexões no host local (interface de loopback).
    endereco_servidor.sin_port = htons(PORTA);

    // Conexão com o servidor
    /* A função connect é usada para criar uma conexão com o destino especificado.
    A função connect() possui a seguinte assinatura em C++ 

    int WSAAPI connect(
            [in] SOCKET         s,          Um descritor que identifica um socket não conectado.
            [in] const sockaddr *name,      Um ponteiro para a estrutura sockaddr à qual a conexão deve ser estabelecida.
            [in] int            namelen     O comprimento, em bytes, da estrutura sockaddr apontada pelo parâmetro name .
    );
    */
    if(connect(socket_cliente, (struct sockaddr *)&endereco_servidor, sizeof(endereco_servidor)) < 0){
        perror("Erro na conexao com o servidor");
        close(socket_cliente);
        exit(1);
    }
    

    printf("Conexao com o servidor estabelecida.\n");

    // Recebe e envia mensagens para o socket
    while(1){
        printf("Cliente: ");
        fgets(buffer, TAM_BUFFER, stdin);
        send(socket_cliente, buffer, strlen(buffer), 0);
        /*A função send() é utilizada para gravar dados de saida em um socket conectado
        E possui a seguinte assinatura em c++
        int WSAAPI send(
                [in] SOCKET     s,      Identificação do socket conectado
                [in] const char *buf,   buffer que possui dados a serem transmitidos
                [in] int        len,    comprimento, em bytes, do buffer apontado por *buf
                [in] int        flags   sinalizadores para especificar como a chamada é feita
        );

        */
       n = recv(socket_cliente, buffer, TAM_BUFFER, 0);
       if(n <= 0)  {
            printf("Conexao com o servidor fechada");
            break;
       }
       buffer[n] = '\0';
       printf("Servidor: %s", buffer);
    }
    close(socket_cliente);
    return 0;
}
