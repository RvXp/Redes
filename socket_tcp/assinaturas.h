#ifndef ASSINATURAS_H
#define ASSINATURAS_H
// Funções do servidor
int cria_socket_do_servidor(int);
int aceita_cliente(int, struct sockaddr_in);

// Funções do cliente
int cria_socket_do_cliente(const char, int);

// Funções enviar e receber
void envia_mensagem(int, const char);
int recebe_mensagem(int, char, int);

#endif