#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include "bap.h"

int sendData(SOCKET sock, char *buffer, int buffer_len, struct sockaddr_in *recv_addr, socklen_t recv_addr_len){
    // Envia a primitiva
    int s = sendto(sock, (char *)buffer, buffer_len, 0, (struct sockaddr *)recv_addr, recv_addr_len);
    // Caso ocorra um erro no envio da primitiva retorna um erro primitiva
    if(s == SOCKET_ERROR){
        perror("Falha no envio da primitiva");
        return EVENT_ERROR;
    }
    else{
        // Se a primitiva enviada foi D0 aguarda pelo ACK0
        if(buffer[0] == '0'){
            printf("D0 enviado\n");
            return STATE_WAIT_ACK0;
        }
        // Se não, se a a primitiva enviada foi D1, aguarda por ACK1
        else if(buffer[0] == '1'){
            printf("D1 enviado\n");
            return STATE_WAIT_ACK1;
        }
    }
    return EVENT_ERROR;
}

int receiveEvent(SOCKET sock, char *buffer, int buffer_len, struct sockaddr_in *recv_addr, socklen_t recv_addr_len){
        socklen_t addr_len = recv_addr_len; // Tava dando pau em passar recv_addr_len direto pra recvfrom
        
        fd_set read;
        /*  https://learn.microsoft.com/pt-br/windows/win32/api/winsock2/ns-winsock2-fd_set
        typedef struct fd_set {
            u_int  fd_count;                O número de soquetes no conjunto.
            SOCKET fd_array[FD_SETSIZE];    Um vetor de soquetes que estão no conjunto. A variável FD_SETSIZE usa como padrão 64.
        } fd_set, FD_SET, *PFD_SET, *LPFD_SET;
        */

        struct timeval timeout;
        /* https://learn.microsoft.com/pt-br/windows/win32/api/winsock/ns-winsock-timeval
        typedef struct timeval {
            long tv_sec;    Intervalo de tempo, em segundos.
            long tv_usec;   Intervalo de tempo, em microssegundos. Esse valor é usado em combinação com o membro tv_sec para representar valores de intervalo de tempo que não são vários segundos.
        } TIMEVAL, *PTIMEVAL, *LPTIMEVAL;
        */

        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;

        FD_ZERO(&read); // Limpar (inicializar) um conjunto de descritores de arquivos. Isso é necessário antes de adicionar sockets a esse conjunto com a função FD_SET().
        FD_SET(sock, &read);
        /* https://learn.microsoft.com/pt-br/windows/win32/api/winsock2/nf-winsock2-fd_set
        void FD_SET(
            fd, Um descritor que identifica um soquete que será adicionado ao conjunto.
            set Um ponteiro para um fd_set.
        );
        */

        int activity = select(sock + 1, &read, NULL, NULL, &timeout);
        /* https://learn.microsoft.com/pt-br/windows/win32/api/winsock2/nf-winsock2-select
        A função select retorna o número total de identificadores de soquete que estão prontos e contidos nas estruturas fd_set , zero se o limite de tempo expirou ou SOCKET_ERROR se ocorreu um erro. 
        int WSAAPI select(
            [in] int                nfds,   
            [in, out] fd_set        *readfds,   Um ponteiro opcional para um conjunto de soquetes a ser verificado quanto à legibilidade.
            [in, out] fd_set        *writefds,  Um ponteiro opcional para um conjunto de soquetes a serem verificados quanto à capacidade de gravação.
            [in, out] fd_set        *exceptfds, Um ponteiro opcional para um conjunto de soquetes a serem verificados quanto a erros.
            [in] const timeval      *timeout    O tempo máximo para selecionar aguardar, fornecido na forma de uma estrutura TIMEVAL .
        );
        */
        if (activity == SOCKET_ERROR) {
            perror("Erro");
            return EVENT_ERROR;
        } 
        else if (activity == 0) {
            // Timeout atingido
            printf("Timeout atingido\n");
            return EVENT_TIMEOUT;
        }
        // Caso ocorra atividade no socket
        else {
            // Esse role todo só pq a recvfrom não tem timeout, ela faz o programa aguarda por uma mensagem por tempo indeterminado
            int r = recvfrom(sock, (char *)buffer, buffer_len, 0, (struct sockaddr *)recv_addr, &addr_len);
            // Caso ocorra um erro no recebimento da confirmação, retorna um evento de erro
            if(r == SOCKET_ERROR){
                perror("Falha no recebimento do ACK");
                return EVENT_ERROR;
            }
            // Caso receba um ACK0 
            else if(buffer[0] == '0'){
                printf("ACK0 recebido\n");
                return EVENT_ACK0_RECEIVED;
            }
            // Caso receba um ACK1
            else if(buffer[0] == '1'){
                printf("ACK1 recebido\n");
                return EVENT_ACK1_RECEIVED;
            }
        }
    return EVENT_ERROR;
}

int main() {
    // Inicialização da biblioteca winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { 
        printf("Erro na inicializacao do Winsock.\n");
        return 1;
    }
    
    // SetUp para o envio das primitivas
    char buffer[1];
    struct sockaddr_in recv_addr = {0};
    socklen_t recv_addr_len = sizeof(recv_addr);

    // Criação do socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == INVALID_SOCKET){
        perror("Falha na criação do socket");
        exit(1);
    }

    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(8080);
    recv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Definição do estado inicial
    ABP abp;
    abp.currentState = STATE_SEND_D0;
    abp.currentEvent = EVENT_SEND_D0;

    while(1){
        if(abp.currentState == STATE_SEND_D0){
            if(abp.currentEvent == EVENT_SEND_D0){
                // Envia a primitiva D0
                buffer[0] = '0';
                int sd = sendData(sock, (char *)buffer, 1, &recv_addr, recv_addr_len);
                // Espera pelo recebimento do ACK0
                if(sd == STATE_WAIT_ACK0){
                    abp.currentState = sd;
                }
                // Caso ocorra um erro no envio, a primitiva é reenviada
                else if(sd == EVENT_ERROR || sd == STATE_WAIT_ACK1){
                    while(sd != STATE_WAIT_ACK0){
                        int sd = sendData(sock, (char *)buffer, 1, &recv_addr, recv_addr_len);
                        Sleep(1000);
                    }   
                }
            }
        }
        // Aguarda pelo ACK0
        else if(abp.currentState == STATE_WAIT_ACK0){
            int event = receiveEvent(sock, buffer, 1, &recv_addr, recv_addr_len);
            // Caso ocorra erro, timeout ou o recebimento do ACK1 reotrna para o estado de envio da primitiva
            if(event == EVENT_ERROR || event == EVENT_TIMEOUT || event == EVENT_ACK1_RECEIVED){
                abp.currentState = STATE_SEND_D0;
            }
            // Caso ACK0 seja recebido, parte para o estado de envio de D1
            else if(event == EVENT_ACK0_RECEIVED){
                abp.currentState = STATE_SEND_D1;
                abp.currentEvent = EVENT_SEND_D1;
            }
        }

        else if(abp.currentState == STATE_SEND_D1){
            if(abp.currentEvent == EVENT_SEND_D1){
                // Envia a primitiva D1
                buffer[0] = '1';
                int sd = sendData(sock, (char *)buffer, 1,&recv_addr, recv_addr_len);
                // Caso seja enviada com sucesso parte para o estado de recebimento do ACk1
                if(sd == STATE_WAIT_ACK1){
                    abp.currentState = sd;
                }
                // Caso ocorra um erro no envio, a primitiva é reenviada
                else if(sd == EVENT_ERROR || sd == STATE_WAIT_ACK0){
                    while(sd != STATE_WAIT_ACK1){
                        int sd = sendData(sock, (char *)buffer, 1, &recv_addr, recv_addr_len);
                        Sleep(1000);
                    }
                }
            }
        }
        else if(abp.currentState == STATE_WAIT_ACK1){
            int event = receiveEvent(sock, buffer, 1, &recv_addr, recv_addr_len);
            // Caso ocorra erro, timeout ou o recebimento do ACK0 reotrna para o estado de envio da primitiva
            if(event == EVENT_ERROR || event == EVENT_TIMEOUT || event == EVENT_ACK0_RECEIVED){
                abp.currentState = STATE_SEND_D1;
            }
            // Caso ACK1 seja recebido, parte para o estado de envio de D0
            else if(event == EVENT_ACK1_RECEIVED){
                abp.currentState = STATE_SEND_D0;
                abp.currentEvent = EVENT_SEND_D0;
            }
        }
        Sleep(1500);
    }
    
}
