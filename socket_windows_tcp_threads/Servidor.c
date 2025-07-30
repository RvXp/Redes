#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include  <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "assinaturas.h"
#include "config.h"
DWORD WINAPI threads(LPVOID socket_client); // O tipo de dado DWOR ser para retornar uma faixa especifica de valores, não sendo necessariamente um int ou um log int
// https://www.guj.com.br/t/para-que-servem-os-tipos-word-e-dword-no-c-c/332156/2

// E WINAPI serve para ter acesso direto ao Windows e ao hardware  
// https://learn.microsoft.com/pt-br/windows/win32/desktop-programming
int main(){
    WSADATA wsaData; 
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { 
        printf("Erro na inicialização do Winsock.\n");
        return 1;
    }

    int socket_servidor, socket_cliente;
    struct sockaddr_in endereco_servidor, endereco_cliente;
    int cliente_len = sizeof(endereco_cliente);
    char buffer[TAM_BUFFER];
    socklen_t tamanho_do_endereco;

    HANDLE thread_id; // HANDLE é um tipo de dado usado no Windows para representar um identificador genérico para um objeto ou recurso do sistema operacional
    // https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#handle
    int n;
    socket_servidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(socket_servidor < 0){
        perror("Erro na criacao do socket");
        exit(1);
    }
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = INADDR_ANY;
    endereco_servidor.sin_port = htons(PORTA);

    if(bind(socket_servidor, (struct sockaddr *)&endereco_servidor, sizeof(endereco_servidor)) < 0){
        perror("Erro no bind");
        closesocket(socket_servidor);
        exit(1);
    }
    if(listen(socket_servidor, SOMAXCONN) < 0){
        perror("Erro na escuta");
        closesocket(socket_servidor);
        exit(1);
    }
    printf("Servidor aguardando por conexoes...\n");

    while(1){
        socket_cliente = accept(socket_servidor, (struct sockaddr*)&endereco_cliente, &cliente_len);
        if(socket_cliente == INVALID_SOCKET){
            perror("Falha ao aceitar conexão");
            continue;
        }
        printf("Cliente conectado.\n");
        SOCKET* client_sock = (SOCKET*)malloc(sizeof(SOCKET)); // Aloca memoria dinamicamente do tamanho necessario para um dado do tipo SOCKET
        if(client_sock == NULL){
            perror("Erro ao alocar memória para o socket");
            closesocket(socket_cliente);
            continue;
        }
        thread_id = CreateThread(NULL, 0, threads, &socket_cliente, 0, NULL);
        /*
            HANDLE CreateThread(
                    [in, optional]  LPSECURITY_ATTRIBUTES   lpThreadAttributes,     Determina se o identificador retornado pode ser herdado por processos filho.
                    [in]            SIZE_T                  dwStackSize,            O tamanho inicial da pilha, em bytes. 
                    [in]            LPTHREAD_START_ROUTINE  lpStartAddress,         Um ponteiro para a função definida pelo aplicativo a ser executada pelo thread.
                    [in, optional]  __drv_aliasesMem LPVOID lpParameter,            Um ponteiro para uma variável a ser passada para o thread.
                    [in]            DWORD                   dwCreationFlags,        Os sinalizadores que controlam a criação do thread.
                    [out, optional] LPDWORD                 lpThreadId              
            );
        */
        if(thread_id == NULL){
            perror("Erro ao criar thread.\n");
            closesocket(socket_cliente);
            free(client_sock);
        }else{
            CloseHandle(thread_id); // CloseHandle invalida o identificador de objeto especificado, diminui a contagem de identificadores do objeto e executa verificações de retenção de objeto. Depois que o último identificador para um objeto é fechado, o objeto é removido do sistema.
        }
    }
    closesocket(socket_servidor);
    return 0;
}

    DWORD WINAPI threads(LPVOID socket_cliente){ // LPVOID é um ponteiro generico
    SOCKET socket = *(SOCKET*)socket_cliente; // Aqui é convertido o tipo do ponteiro de generico para socket
    char buffer[TAM_BUFFER];
    free(socket_cliente);  // Libera a memoria alocada para o SOCKET (todo)
    int n;
     
     while(1){
        n = recv(socket, buffer, TAM_BUFFER, 0);
        if(n <= 0){
            printf("Conexao encerrada");
            break;
        }
        buffer[n] = '\0';
        printf("Cliente: %s", buffer);   

        printf("Servidor: ");
        fgets(buffer, TAM_BUFFER, stdin);
        send(socket, buffer, strlen(buffer), 0);
    }
    closesocket(socket);

    return 0;

}