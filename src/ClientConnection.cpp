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
#include <unistd.h>
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
    struct sockaddr_in sin;
    struct hostent *hent;
  
    int s;
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    hent = gethostbyaddr(&address, sizeof(address), AF_INET);
    
    if (hent) {
        memcpy(&sin.sin_addr, hent->h_addr,hent->h_length);
    }else if ((sin.sin_addr.s_addr = inet_addr((char*)address)) == INADDR_NONE)
    {
        printf("No se puede resolver el nombre de dominio \"%s \" \n ",address);
    }
    
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0)
    {
        printf("No Puede Crear El Socket: %s \n", strerror(errno)); //?
    }
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin))<0) {
        printf("No se puede conectar con: %s, %s \n",address,strerror(errno));
    }
    return s;
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
	fprintf(fd, "PWD\n");
      }
      else if (COMMAND("PASS")) {
	char pass[30];
        fscanf(fd,"%s",pass);

        fprintf(fd,"230 User logged in\n");
	   
      }
      
      else if (COMMAND("PORT")) {
	unsigned ip[4];
        unsigned port[2];

        fscanf(fd,"%u,%u,%u,%u,%u,%u",&ip[0],&ip[1],&ip[2],&ip[3],&port[0],&port[1]);


        uint32_t aux1;
        uint16_t aux2;

        aux1 = ip[3] << 24 | ip[2] << 16 | ip[1] << 8 | ip[0];

        aux2 = port[1]*256 + port[0];

        data_socket = connect_TCP(aux1,aux2);

        fprintf(fd,"200 OK\n");

      }
      else if (COMMAND("PASV")) {
	fprintf(fd, "PASV\n");
      }
      else if (COMMAND("CWD")) {
	fprintf(fd, "CWD\n");
      }
      else if (COMMAND("STOR") ) {
	    fprintf(fd, "STOR\n");
      }
      else if (COMMAND("SYST")) {
        fprintf(fd,"SYSTEM DETAILS\n");
	   
      }
      else if (COMMAND("FEAT") ) {
	    fprintf(fd, "FEAT\n");
      }
      else if (COMMAND("TYPE")) {
	
	fprintf(fd,"type 1");
	  
      }
      else if (COMMAND("RETR")) {
	fscanf(fd,"%s",arg);
        std::cout << "Argument: " << arg << std::endl;
        FILE* fp = fopen(arg,"r+");

        int sent_datas;

        int size_buffer = 512;

        char buffer[size_buffer];

         std::cout << "Buffer size = " << size_buffer << std::endl;

        do{

          sent_datas = fread(buffer,size_buffer,1,fp);
          printf("Code %d |  %s\n",errno,strerror(errno));
          send(data_socket,buffer,sent_datas,0);
          printf("Code %d |  %s\n",errno,strerror(errno));

        }while(sent_datas == size_buffer);

        close(data_socket);
        fclose(fp);

        fprintf(fd,"Transferencia completada");
	   
      }
      else if (COMMAND("QUIT")) {
	fprintf(fd, "QUIT\n");
	//stop();
	 
      }
      else if (COMMAND("LIST")) {
	fprintf(fd, "LIST\n");
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