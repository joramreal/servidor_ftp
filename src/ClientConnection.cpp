//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//                      
//                     2ª de grado de Ingeniería Informática
//                       
//  Clase que atiende una petición FTP.
// 
//****************************************************************************



#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cerrno>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
 #include <fcntl.h>

#include <sys/stat.h> 
#include <iostream>
#include <dirent.h>

#include "common.h"

#include "ClientConnection.h"




ClientConnection::ClientConnection(int s) {
    int sock = (int)(s);
  
    char buffer[MAX_BUFF];

    control_socket = s;
    // Consultar la documentación para conocer el funcionamiento de fdopen.
    fd = fdopen(s, "a+");
    if (fd == NULL){
	std::cout << "Connection closed" << std::endl;

	fclose(fd);
	close(control_socket);
	ok = false;
	return ;
    }
    
    ok = true;
    data_socket = -1;
   
  
  
};


ClientConnection::~ClientConnection() {
 	fclose(fd);
	close(control_socket); 
  
}


int connect_TCP( uint32_t address,  uint16_t  port) {
     // Esta función deberá implementarla el alumno.

    return -1; // Deberá devolver un descriptor de socket.

}






void ClientConnection::stop() {
    close(data_socket);
    close(control_socket);
    parar = true;
  
}





    
#define COMMAND(cmd) strcmp(command, cmd)==0


// Esta función es la que atiende las peticiones
// Aquí es donde se implementan las acciones asociadas a los comandos.
// Véase el ejemplo del comando USER.
// Si considera que debe añadir algún otro comando siéntase libre
// de hacerlo. Asimismo, puede añadir tantos métodos auxiliares como
// sea necesario.
void ClientConnection::WaitForRequests() {
    if (!ok) {
	 return;
    }
    
    fprintf(fd, "220 Service ready\n");
  
    while(!parar) {


 
      fscanf(fd, "%s", command);
      if (COMMAND("USER")) {
	    fscanf(fd, "%s", arg);
	    fprintf(fd, "331 User name ok, need password\n");
      }
      else if (COMMAND("PWD")) {
	   
      }
      else if (COMMAND("PASS")) {
	   
      }
      else if (COMMAND("PORT")) {
	  
      }
      else if (COMMAND("PASV")) {
	  
      }
      else if (COMMAND("CWD")) {
	   
      }
      else if (COMMAND("STOR") ) {
	    
      }
      else if (COMMAND("SYST")) {
	   
      }
      else if (COMMAND("TYPE")) {
	  
      }
      else if (COMMAND("RETR")) {
	   
      }
      else if (COMMAND("QUIT")) {
	 
      }
      else if (COMMAND("LIST")) {
	
      }
      else  {
	    fprintf(fd, "502 Command not implemented.\n"); fflush(fd);
	    printf("Comando : %s %s\n", command, arg);
	    printf("Error interno del servidor\n");
	
      }
      
    }
    
    fclose(fd);

    
    return;
  
};