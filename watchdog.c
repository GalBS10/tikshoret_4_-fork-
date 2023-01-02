#include "stdio.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "sys/stat.h"
#include "string.h"
#include "arpa/inet.h"
#include "stdlib.h"
#include "unistd.h"
#include "netinet/in.h"
#include "netinet/tcp.h"
#include <time.h>
#include <fcntl.h>
#include "errno.h"
#include "error.h"




int main()
{
    //creating a socket
    int Main_Socket;
    Main_Socket = socket(AF_INET,SOCK_STREAM,0);
    if(Main_Socket==-1){
        printf("-there is a problem with initializing receiver\n");
    }
    else{
        printf("-initialize successfully.\n");
    }
//--------------------------------------------------------------------------------
//initialize where to send
    struct sockaddr_in Sender_address,new_addr;
    Sender_address.sin_family=AF_INET;
    Sender_address.sin_port=htons(3000);
    Sender_address.sin_addr.s_addr=INADDR_ANY;
//---------------------------------------------------------------------------------
//connecting the Receiver and Sender
    int bindd =bind(Main_Socket,(struct sockaddr *) &Sender_address,sizeof(Sender_address));
    if(bindd==-1){
        printf("-there is a problem with bindding.\n");
    }
    else{
        printf("-bindding successfully.\n");
    }
//---------------------------------------------------------------------------------

    int sock_queue =listen(Main_Socket,2);//now it can listen to two senders in pareral.
    if(sock_queue==-1){//if there are already 2 senders.
        printf("-queue is full, can't listen.\n");
    }
    else{
        printf("-listening...\n");
    }
//initialize the socket for comunicating with the Sender.
    int Second_Socket;//the socket
    socklen_t addr_size=sizeof(new_addr);
    Second_Socket= accept(Main_Socket,(struct sockaddr*)&new_addr, &addr_size);//the func return socket discriptor of a new 
    //socket and information of the Sender like IP and Port into new_addr.
//---------------------------------------------------------------------------------

    //char *strerror = errno;
    int errno;
    size_t size1 = 0;
    size_t size2 = 0;
    clock_t begin;
    char data[12];
    char *WatchDog_Message = "timeout";
    double time_spent;
    /* Mark beginning time */
    while(1)
        {
        size1 = recv(Second_Socket,data,12,MSG_DONTWAIT); //sended ping

        if(size1 == -1){
            if(errno != EWOULDBLOCK){
                perror("---recv1---");
                exit(errno);
            }
        }
        
        begin = clock(); // start the clock

        while((size2 <= 0) && (time_spent < 10.0)){

            time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;

            size2 = recv(Second_Socket,data,12,MSG_DONTWAIT); // sende pong 

            if ((time_spent>=10.0) && (size2 == 0)){ //kill the program
                send(Second_Socket, WatchDog_Message,sizeof(WatchDog_Message),0);
                printf("-------closing-------");
                close(Second_Socket);
                break;
            }
            else if((time_spent < 10.0) && (size2 > 0)){ //restart timer
                begin = clock();
                break;
            }
            else{
                //dont know yet
            }
        }


    }
        return 0;
}