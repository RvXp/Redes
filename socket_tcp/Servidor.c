#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "assinaturas.h"
#include "config.h"
/*Adaptação da implementação de socket
    creado por Diego Molinos para uso academico
*/
int main(){
    // Essa inicialização da biblioteca winsock2 é necessaria por permite que um aplicativo, ou DLL, especifique a versão do Windows Sockets necessária e recupere detalhes da implementação específica do Windows Sockets. 
    WSADATA wsaData; // Crie um objeto WSADATA chamado wsaData.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // Chame WSAStartup e retorne seu valor como um inteiro e marcar para erros.
        /*  A WSAStartup possui o seguinte cabeçalho
        int WSAStartup(
            [in]  WORD      wVersionRequired,   A versão mais alta da especificação do Windows Sockets que o chamador pode usar.
            [out] LPWSADATA lpWSAData           Um ponteiro para a estrutura de dados WSADATA que deve receber detalhes da implementação do Windows Sockets.
        );
        */
        printf("Erro na inicialização do Winsock.\n");
        return 1;
    }

    // Em C a função accept() é usada para  aceitar uma conexão de um cliente. Ela sera detalhada futuramente.
    int socket_servidor, socket_cliente; // Um socket é definico como um canal de comunicação que permite processos não relacionados a troca de dados, seja localmente como ocorre entre a camada de aplicação e a camada de transporte, ou pela rede como ocorre na comunicação cliente servidor.
    struct sockaddr_in endereco_servidor, enderco_cliente;
    // Um dos parametros passados para accept() é um ponteiro para uma estrutura que armazena o endereço do cliente
    /* A struct sockaddr_in é contruida da seguinte forma:
        struct sockaddr_in {
            short            sin_family;   A família de endereços para o endereço de transporte.
            unsigned short   sin_port;     Um número da porta do protocolo de transporte.
            struct in_addr   sin_addr;     Uma estrutura IN_ADDR que contém um endereço de transporte IPv4.
            char             sin_zero[8];   Reservado para uso do sistema. 
        };
    */
    char buffer[TAM_BUFFER];
    socklen_t tamanho_do_endereco;

    int n;

    // Criação do socket
    socket_servidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // AF_INET se refere ao tipo de endereço, no caso IPv4
    // SOCK_STREAM define o tipo de socket a ser criado, no caso TCP
    // IPPROTO_TCP define o tipo de protocolo utilizado no socket, no caso TCP

    if(socket_servidor < 0){
        perror("Erro na criacao do socket");
        exit(1);
    }
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = INADDR_ANY;// INADDR_ANY significa que o servidor ira escutar por qualquer interface fisica
    endereco_servidor.sin_port = htons(PORTA);

    // Bind do socket
    /* A função bind possui a seguinte assinatura em C++
    int bind(
            [in] SOCKET         s,      Para identificar um socket não associado 
            const sockaddr *addr,       Um ponteiro para a struct sockaddr com o objetivo de atribuir um um socket associado
            [in] int     namelen        O comprimento, em bytes, do valor apontado po addr
    );
    
    */
    if(bind(socket_servidor, (struct sockaddr *)&endereco_servidor, sizeof(endereco_servidor)) < 0){
        perror("Erro no bind");
        close(socket_servidor);
        exit(1);
    }
    // O bind permite a associação de um IP e uma porta a um socket
    
    // Estabelecer o servidor no mode de escuta
    /* A função listem possui a seguinte assinatura em C++
    int WSAAPI listen(
                [in] SOCKET s,      Identifica o socket criado e não associado
                [in] int backlog    Define o comprimento maximo da fila de conexões pendentes
    );
    */
    if(listen(socket_servidor, 1) < 0){
        perror("Erro na escuta");
        close(socket_servidor);
        exit(1);
    }
    // A função listen() permite que o servidor recebeba mais de uma solicitação de conexão por vez

    printf("Servidor aguardando por conexoes...\n");

    tamanho_do_endereco = sizeof(enderco_cliente);
    /* A função accept() possui a seguinte assinatura em C++
    SOCKET WSAAPI accept(
            [in]      SOCKET   s,       Recebe o socket que foi criado e colocado no modo de escuta
            [out]     sockaddr *addr,   Um ponteiro para o buffer que armazena o enderaço a entidade, criada pela struct sockaddr, que deseja se conectar
            [in, out] int      *addrlen Um ponteiro para para um inteiro que contem o comprimento da struct aprontada em *addr
    );
    */
    socket_cliente = accept(socket_servidor, (struct sockaddr *)&enderco_cliente, &tamanho_do_endereco); // Cria um socket para estabelecer a conexão com o cliente 

    if(socket_cliente < 0){
        perror("Erro ao aceitar a conexao");
        close(socket_servidor);
        exit(1);
    }

    printf("Cliente conectado.\n");

    while(1){
        /*A função recv() possui a seguinte assinatura em C++
        int recv(
            [in]  SOCKET s,     Identifica um socket conectado
            [out] char   *buf,  ponteiro para o buffer que recebe os dados de entrada
            [in]  int    len,   comprimento, em bytes, do buffer apontado por *buf
            [in]  int    flags  um conjunto de sinalizadores que influencia o comportamenta dessa função

        );
        */
        n = recv(socket_cliente, buffer, TAM_BUFFER, 0); // A função recv é usada para ler dados de entrada em soquetes orientados à conexão
        if(n <= 0){
            printf("Conexao encerrada");
            break;
        }
        buffer[n] = '\0';
        printf("Cliente: %s", buffer);   

        printf("Servidor: ");
        fgets(buffer, TAM_BUFFER, stdin);
        /*A função send() é utilizada para gravar dados de saida em um socket conectado
        E possui a seguinte assinatura em c++
        int WSAAPI send(
                [in] SOCKET     s,      Identificação do socket conectado
                [in] const char *buf,   buffer que possui dados a serem transmitidos
                [in] int        len,    comprimento, em bytes, do buffer apontado por *buf
                [in] int        flags   sinalizadores para especificar como a chamada é feita
        );

        */
        send(socket_cliente, buffer, strlen(buffer), 0);
    }
    closesocket(socket_cliente);
    closesocket(socket_servidor);

    return 0;
}