#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <winsock2.h>
#include <sys/types.h>
// Baseado em https://www.youtube.com/watch?v=lUyaV4haBUE
int main(){
    // Inicialização da winsock2
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { 
        printf("Erro na inicializacao do Winsock.\n");
        return 1;
    }
    // Como o protocolo UDP executa uma comunicação não orientada a conexão, a mensagem a ser transmitida deve ser preparada previamente pelo emissor
    char * hello = "Ola do cliente";
    char buffer[50] = {0};
    struct sockaddr_in serveraddr = {0};
    int addr_len = sizeof(serveraddr);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == INVALID_SOCKET){
        perror("Falha na criação do socket");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8080);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    // Envio da mensagem, utilizando a funçaõ sendto, que não é orientada a conexão
    int len = sendto(sockfd, (const char *)hello, strlen(hello), 0, (const struct  sockaddr *)&serveraddr, sizeof(serveraddr));
    /* A função sendto() possui a seguinte assinatura
        int sendto(
            [in] SOCKET         s,      Identifica um soquete (possivelmente conectado).
            [in] const char     *buf,   Um ponteiro para um buffer que contém os dados a serem transmitidos.
            [in] int            len,    O comprimento, em bytes, dos dados apontados pelo parâmetro buf .
            [in] int            flags,  Um conjunto de sinalizadores que especificam a maneira como a chamada é feita.
            [in] const sockaddr *to,    Um ponteiro opcional para uma estrutura sockaddr que contém o endereço do soquete de destino.
            [in] int            tolen   O tamanho, em bytes, do endereço apontado pelo parâmetro to .
        );
    */
       // A função sendto, naturalmente baseado no protocolo UDP, envia uma mensagem para um endereço especifico que esteja, esse recebera a mensagem caso tenha um socket aberto para escutar
    if(len == SOCKET_ERROR){
        perror("Falha no envio da mensagem");
    }
    else{
        printf("Mensagem enviada com sucesso...\n");
    }

    int n = recvfrom(sockfd, (char *)buffer, 50, 0, (struct sockaddr *)&serveraddr, &addr_len);
    if(n == SOCKET_ERROR){
        perror("Falha no recebimento da mensagem...");
        closesocket(sockfd);
        exit(1);
    }
    buffer[n] = '\0';
    printf("Mensagem recebida: %s\n", buffer);
    closesocket(sockfd);
    return 0;
}