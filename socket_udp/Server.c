#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <winsock2.h>
#include <sys/types.h>
#include <ws2tcpip.h>

// Baseado em https://www.youtube.com/watch?v=lUyaV4haBUE
int main(){

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { 
        printf("Erro na inicializacao do Winsock.\n");
        return 1;
    }
    // Atráves do protocolo UDP, por não ser orientadoa conexão, a mensagem a ser enviada deve ser previamente estabelecida
    char * hello = "Ola do servidor";
    char buffer[50] = {0};
    struct sockaddr_in serveraddr = {0}, clientaddr = {0};
    socklen_t clientaddr_len = sizeof(clientaddr);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == INVALID_SOCKET){
        perror("Falha na criação do socket");
        exit(1);
    }
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8080);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    printf("Servidor aguardando mensagem...\n");

    int rc = bind(sockfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if(rc == SOCKET_ERROR){
        perror("Falha no bind");
        closesocket(sockfd);
        exit(1);
    }
    

    socklen_t len = 0;
    // A função recvfrom lê dados de entrada em soquetes conectados e não conectados e captura o endereço do qual os dados foram enviados. Essa função normalmente é usada com soquetes sem conexão. 
    int n = recvfrom(sockfd, (char *)buffer, 50, 0, (struct sockaddr *)&clientaddr, &clientaddr_len);
    /* A função recvfrom possui a seguinte assinatura em C++
    int WSAAPI recvfrom(
        [in]                SOCKET   s,         Um descritor que identifica um soquete associado.
        [out]               char     *buf,      Um buffer para os dados de entrada.  
        [in]                int      len,       O comprimento, em bytes, do buffer apontado pelo parâmetro buf .
        [in]                int      flags,     Um conjunto de opções que modificam o comportamento da chamada de função além das opções especificadas para o soquete associado.
        [out]               sockaddr *from,     Um ponteiro opcional para um buffer em uma estrutura sockaddr que manterá o endereço de origem no retorno.   
        [in, out, optional] int      *fromlen   Um ponteiro opcional para o tamanho, em bytes, do buffer apontado pelo parâmetro from .  
);
    
    */ 
   // Assim, nota-se a ausencia da função accept, que estabelece uma conexão com um cliente
    if(n == SOCKET_ERROR){
        perror("Falha no recebimento da mensagem...");
        closesocket(sockfd);
        exit(1);
    }

    buffer[n] = '\0';
    printf("Mensagem recebida: %s\n", buffer);

    int server_len = sendto(sockfd, (const char *)hello, strlen(hello), 0, (const struct  sockaddr *)&clientaddr, sizeof(clientaddr));

    if(server_len == SOCKET_ERROR){
        perror("Falha no envio da mensagem");
    }
    else{
        printf("Mensagem enviada com sucesso...\n");
    }
    closesocket(sockfd);
    return 0;
}