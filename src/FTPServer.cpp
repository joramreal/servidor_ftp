//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//                      
//                     2ª de grado de Ingeniería Informática
//                       
//  Clase principal para la implementación de un servidor FTP sencillo
// 
//****************************************************************************

#include <cerrno>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

 #include <unistd.h>


#include <pthread.h>

#include <list>

#include "common.h"
#include "FTPServer.h"
#include "ClientConnection.h"


int define_socket_TCP(int port) {
  // Aquí deberá incluir el código para definir el socket.
  struct sockaddr_in sin;
    int s;
 
    s = socket (AF_INET, SOCK_STREAM, 0); //Socket TCP
 
    if (s < 0) {
        errexit ("No puedo crear el socket: %s\n", strerror(errno));
    }
 
    memset (&sin, 0, sizeof (sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons (port);
   
    if ( bind(s, (struct sockaddr*)&sin, sizeof(sin)) < 0 ){
        errexit ("No puedo hacer el bind con el puerto: %s\n", strerror(errno));
    }
    if ( listen (s, 1) < 0)
        errexit ("Error en el listen: %s\n", strerror(errno));
   
    return s;
    //RESUMEN: socket(), inicializar sockaddr_in, bind(), listen()
//   return -1;
}

// Esta es la función que se ejecuta al
// crear el hilo.
void* run_client_connection(void *c) {
    ClientConnection *connection = (ClientConnection *)c;
    connection->WaitForRequests();
  
    return NULL;
}



FTPServer::FTPServer(int port) {
    this->port = port;
  
}


// Parada del servidor.
void FTPServer::stop() {
    close(msock);
    shutdown(msock, SHUT_RDWR);

}


// Arranque del servidor
void FTPServer::run() {

    struct sockaddr_in fsin;
    int ssock;
    socklen_t alen;
    msock = define_socket_TCP(port);  // Esta función deberá implementarla el alumno.
    alen=sizeof(fsin);
    while (1) {
	pthread_t thread;
        ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
        if(ssock < 0)
            errexit("Fallo en el accept: %s\n", strerror(errno));
	
	ClientConnection *connection = new ClientConnection(ssock);
	
	// Aquí se crea un hilo para que se puedan ejecutar varias
	// peticiones simultáneamente.
	pthread_create(&thread, NULL, run_client_connection, (void*)connection);
       
    }

}