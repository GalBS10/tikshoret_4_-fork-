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
    
    struct timeval start, end;
    long long seconds = 0;
    char ping[12] = " ";
    char pong[12] = " ";
    char *WatchDog_Message = "timeout";
    int flag = 0;

    while(1){
        
        recv(Second_Socket,ping,12,MSG_DONTWAIT); //sended ping

        if(!strcmp(ping,"sended ping")){
            bzero(ping,12);
            gettimeofday(&start, 0); // start clock
            flag = 0;
        }
        else{
            continue;
        }

        while(seconds < 10){

            recv(Second_Socket,pong,12,MSG_DONTWAIT); // sended pong 

            if(!strcmp(pong,"sended pong")){
                bzero(pong,12);
                flag = 1;
                break;
            }
            gettimeofday(&end, 0); // end clock
            seconds = ((end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f) * 0.001;


        }

        if(!flag){
            send(Second_Socket, WatchDog_Message,sizeof(WatchDog_Message),0);
            printf("-closing watchdog.\n");
            close(Second_Socket);
            close(Main_Socket);
            return 0;
        }
    }
        return 0;
}