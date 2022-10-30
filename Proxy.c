//Codigo basado en el codigo demostrado en el siguiente video: https://www.youtube.com/watch?v=Pg_4Jz8ZIH4
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>

//Definicion de algunas constantes
#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100 //Numero maximo de peticiones que se pueden escuchar a la vez

//Definicion de variables globales
typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;
int i=0;
int j=1;
int TTL=0;

//Argumentos para el metodo prox, donde se hace la conexion
struct args{
    char* ip;
    int* client_sock;
};

//Prototipos de las funciones
void * prox(void* p_client_socket);
int check(int exp, const char *msg);


int main(int argc, char **argv){
    //Definicion de variables
    int server_socket, client_socket, addr_size, serverport, n;
    SA_IN server_addr, client_addr;
    //Lectura del TTL
    TTL=atoi(argv[0]);
    //Inicio de variables para leer archivos
    FILE *fptr;
    FILE *cLog;
    //Lectura de la configuracion del backend
    fptr = fopen("config.txt", "r");
    char ips[3][20];
    char server1[20];
    char server2[20];
    char server3[20];
    int scan=fscanf(fptr, "%s%s%s%d", server1,server2,server3, &serverport);
    strcpy(ips[0], server1);
    strcpy(ips[1], server2);
    strcpy(ips[2], server3);
    
    //Inicio del log
    cLog = fopen("log.txt", "w");
    fprintf(cLog, "Log de conexiones:\n");
    fclose(cLog);

    //Se crea un socket, si falla, imprime un error
    check((server_socket=socket(AF_INET, SOCK_STREAM, 0)), "Failed to create socket");

    //Creacion del tipo de socket
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY;
    server_addr.sin_port=htons(serverport);
    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Bind failed");
    check(listen(server_socket, SERVER_BACKLOG), "Listen failed");

    //Loop infinito para correr indefinidamente el proxy
    while (1){
        addr_size=sizeof(SA_IN);
        //Se acepta la conexion con el cliente, si no es posible, se genera un error
        check(client_socket=accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size), "Accept failed");
        //Definimos la creacion de los threads para el multi threading
        pthread_t t;
        //Creamos el conjunto de paramentros que se van a enviar al metodo prox
        int *pclient=malloc(sizeof(int));
        *pclient=client_socket;
        struct args *send = (struct args *)malloc(sizeof(struct args));
        char ipserv[20];
        strcpy(ipserv, ips[i]);
        send->ip = ipserv;
        send->client_sock = pclient;

        /*Creamos un thread nuevo y llamamos al metodo prox con el parametro send,
        que es una estructura similar a un struct que contiene la IP del servidor
        y el socket del cliente*/
        pthread_create(&t, NULL, prox, (void *)send);
    }

    return 0;

}

//Metodo que cumlpe con la funcion de proxy inverso y balanceador de carga
void * prox(void* p_client_socket){
    //Condicionales auxiliares para contadores
    if (i<2) i=i+1;
    else i=0;
    if (j<TTL) j=j+1;
    else j=1;

    //Definicion de variables
    int server_socket, client_socket, addr_size, connect_socket, n;
    struct  hostent *server;
    SA_IN server_addr;
    //Definicion de variables para leer archivos
    FILE *fprt;
    FILE *cLog;

    //Creacion de socket nuevo de cliente para liberar la memoria del anterior
    client_socket = *((int*)(((struct args*)p_client_socket)->client_sock));
    free((int*)(((struct args*)p_client_socket)->client_sock));
    char buffer[BUFSIZE];
    size_t bytes_read;
    int msgsize=0;
    char actualpath[PATH_MAX+1];

    //Lectura de la peticion
    while((bytes_read = read(client_socket, buffer+msgsize,  sizeof(buffer)-msgsize-1)) > 0){
        msgsize += bytes_read;
        if (msgsize>BUFSIZE-1 || buffer[msgsize-1]=='\n') break;
    }

    //Chequeo de la peticion y vaciado de buffer
    check (bytes_read, "recv error");
    buffer[msgsize-1]=0;

    //Creacion del socket al servidor al que se va a conectar
    connect_socket=socket(AF_INET, SOCK_STREAM, 0);
    server=gethostbyname((((struct args*)p_client_socket)->ip));
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&server_addr.sin_addr.s_addr,
          server->h_length);
    server_addr.sin_port = htons(8080);
    //Conexion del socket del servidor, si no funciona, suelta un error
    check(connect(connect_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)), "Connection error");

    //Mandamos la peticion al servidor
    n=write(connect_socket, buffer, BUFSIZE);

    //Leemos la respuesta del servidor y realizamos las acciones pertinentes
    n=read(connect_socket, buffer, BUFSIZE);
    if(n!=SOCKETERROR){ //Si la conexion fue exitosa
        //char auxiliar para la respuesta
        char response[BUFSIZE];
        //Copiamos los contenidos del buffer a response
        strcpy(response, buffer);

        //char auxiliar para el nombre del archivo
        char aux[10];
        //Creacion dinamica del cache
        snprintf(aux, sizeof(char)*32, "cache%i.txt", j);
        fprt = fopen(aux, "w");
        fprintf(fprt, "%s", response);
        fclose(fprt);

        //Añadimos los procesos del proxy al log
        cLog = fopen("log.txt", "a+");
        fprintf(cLog, "\nPeticion al servidor:%s\n", inet_ntoa(server_addr.sin_addr));
        fprintf(cLog, "Respuesta del servidor:%s\n", response);
        fclose(cLog);

        //Mandamos la respuesta del servidor al cliente
        n=write(client_socket, response, BUFSIZE);
    } else{ //Si la conexion no fue exitosa
        //char auxiliar para la respuesta
        char response[BUFSIZE];

        //char auxiliar para el nombre del archivo
        char aux[10];
        //Lectura dinamica del cache del cual vamos a usar la respuesta
        if(j==1) snprintf(aux, sizeof(char)*32, "cache%i.txt", TTL);
        else snprintf(aux, sizeof(char)*32, "cache%i.txt", j-1);
        fprt = fopen(aux, "r");
        fscanf(fprt, "%s", response);
        fclose(fprt);

        //Añadimos los procesos del proxy al log
        cLog = fopen("log.txt", "a+");
        fprintf(cLog, "\nPeticion al servidor fallida, mandando respuesta del cache:%s\n", inet_ntoa(server_addr.sin_addr));
        fprintf(cLog, "Respuesta del cache:%s\n", response);
        fclose(cLog);
    }

    //Cerramos el socket del cliente
    close(client_socket);

}


int check(int exp, const char *msg){
    if(exp == SOCKETERROR){
        perror(msg);
        exit(1);
    }
    return exp;
}