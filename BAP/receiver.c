#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include "bap.h"

int receiveEvent(SOCKET sock, char *buffer, int buffer_len, struct sockaddr_in *sender_addr, socklen_t sender_addr_len){
    while(1){
        // Recebe uma primitiva
        int r = recvfrom(sock, (char *)buffer, buffer_len, 0, (struct sockaddr *)sender_addr, &sender_addr_len);
        // Se o retorno da função recvfrom for um erro
        if(r == SOCKET_ERROR){
            perror("Falha no recebimento da primitiva\n");
            return EVENT_ERROR;
        }
        // Se não, se a primitiva recebida for 0, retorna o evento de recebimento D0 
        else if(buffer[0] == '0'){
            printf("Primitiva D0 recebida!\n");
            return EVENT_RECEIVE_D0;
        }
        // Se não, se a primitiva recebida for 1, retorna o evento de recebimento D1
        else if(buffer[0] == '1'){
            printf("Primitiva D1 recebida!\n");
            return EVENT_RECEIVE_D1;
        }
        Sleep(1000);
    } 
    return EVENT_ERROR;
}

int sendAck(SOCKET sock, char *buffer, int buffer_len, struct sockaddr_in *sender_addr, socklen_t sender_addr_len){
    // Envia o ACK respectivo a primitiva recebida
    int s = sendto(sock, (char *)buffer, buffer_len, 0, (struct sockaddr *)sender_addr, sender_addr_len);
    // Se ouver uma falha no envio retorna o evento de erro
    if(s == SOCKET_ERROR){
        perror("Falha no envio do ack");
        return EVENT_ERROR;
    }
    else{ // Se o envio for concluido
        if(buffer[0] == '0'){  
            printf("Ack0 enviado!\n"); // Se foi realizado o envio do ACK 0 aguarda a primitica D1
            return EVENT_WAIT_D1; 
        }
        else if(buffer[0] == '1'){  // Se foi realizado o envio do ACK 1 aguarda a primitica D0
            printf("Ack1 enviado!\n");
            return EVENT_WAIT_D0;
        }
    } // Se nenhuma da condições foram atendidas retorna um ERRO
    return EVENT_ERROR;
}
int main() {

    // Inicialização da biblioteca winsock2
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { 
        printf("Erro na inicializacao do Winsock.\n");
        return 1;
    }

    // Setup para o envio de primitivas através do protocolo UDP
    char buffer[1];
    struct sockaddr_in recv_addr = {0}, sender_addr = {0};
    socklen_t sender_addr_len = sizeof(sender_addr);
    socklen_t recv_addr_len = sizeof(recv_addr);

    // Criação do socket UDP
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == INVALID_SOCKET){
        perror("Falha na criação do socket");
        exit(1);
    }

    // Setup para o bind do socket
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(8080);
    recv_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind no socket
    int rc = bind(sock, (const struct sockaddr *)&recv_addr, recv_addr_len);
    if(rc == SOCKET_ERROR){
        perror("Falha no bind");
        closesocket(sock);
        exit(1);
    }

    // Declaração de um variavel do tipo ABP para controlar o fluxo na FSM
    ABP abp;
    abp.currentState = STATE_WAIT_D0;

    while(1){
        // Loop para o recebimento de primitivas, e troca de estados
        int event = receiveEvent(sock, buffer, 1, &sender_addr, sender_addr_len);
       
        if(abp.currentState == STATE_WAIT_D0){  // Estado de espera por D0 
            // Se D0 foi recebido troca o estado para enviar ACK0
            if(event == EVENT_RECEIVE_D0){
                abp.currentState = STATE_SEND_ACK0;
            }
            // Se não, se no aguado de D0 é recebido D1, significa que o ACK1 não foi recebido, Assim troca o estado para o reenvio do ACK1
            else if(event == EVENT_RECEIVE_D1){
                abp.currentState = STATE_SEND_ACK1;
            }
            // Se foi recebido um erro, o ACK1 é reenviado
            else if(event == EVENT_ERROR){
                abp.currentState = STATE_SEND_ACK1;
            }
        }

        else if(abp.currentState == STATE_WAIT_D1){ // Estado de espera por D1
            // Se D1 foi recebido troca o estado para enviar ACK1
            if(event == EVENT_RECEIVE_D1){
                abp.currentState = STATE_SEND_ACK1;
            }
            // Se não, se no aguado de D1 é recebido D0, significa que o ACK0 não foi recebido, Assim troca o estado para o reenvio do ACK0
            else if(event == EVENT_RECEIVE_D0){
                abp.currentState = STATE_SEND_ACK0;
            }
            // Se foi recebido um erro, o ACK0 é reenviado
            else if(event == EVENT_ERROR){
                abp.currentState = STATE_SEND_ACK0;
            }
        } 
        // Estado de envio do ACK0
        if(abp.currentState == STATE_SEND_ACK0){ 
            buffer[0] = '0';
            int sa = sendAck(sock, buffer, 1, &sender_addr, sender_addr_len);
            // Envia ACK0 e aguarda por D1
            abp.currentEvent = sa;
            abp.currentState = STATE_WAIT_D1;
        }
       // Estado de envio do ACK1
        else if(abp.currentState == STATE_SEND_ACK1){
            buffer[0] = '1';
            int sa = sendAck(sock, buffer, 1, &sender_addr, sender_addr_len);
             // Envia ACK1 e aguarda por D0
            abp.currentEvent = sa;
            abp.currentState = STATE_WAIT_D0;
        }
        // Caso ocorra um erro
        if(abp.currentEvent == EVENT_ERROR){
            if (abp.currentState == STATE_SEND_ACK0){ // Se o erro foi retornado durante o envio do ACK0
                buffer[0] = '0';
                int sa = sendAck(sock, buffer, 1, &sender_addr, sender_addr_len);
                abp.currentEvent = sa;
            }else if(abp.currentState == STATE_SEND_ACK1){ // Se o erro foi retornado durante o envio do ACK1
                buffer[0] = '1';
                int sa = sendAck(sock, buffer, 1, &sender_addr, sender_addr_len);
                abp.currentEvent = sa;
                }
            }
        // Aguardando por nova primitiva
        else if(abp.currentEvent == EVENT_WAIT_D0 || abp.currentEvent == EVENT_WAIT_D1){
            //printf("Esperando nova primitiva...\n");
            Sleep(1500); 
        }  
    }
    closesocket(sock);
}
