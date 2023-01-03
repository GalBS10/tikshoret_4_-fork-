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
//connecting the watchdof and new_ping
    int bindd =bind(Main_Socket,(struct sockaddr *) &Sender_address,sizeof(Sender_address));
    if(bindd==-1){
        printf("-there is a problem with bindding.\n");
    }
    else{
        printf("-bindding successfully.\n");
    }
//---------------------------------------------------------------------------------
    int sock_queue =listen(Main_Socket,2);//now it can listen to two senders in parallel.
    if(sock_queue==-1){//if there are already 2 senders.
        printf("-queue is full, can't listen.\n");
    }
    else{
        printf("-listening...\n");
    }
//initialize the socket for comunicating with the new_ping.
    int Second_Socket;//the socket
    socklen_t addr_size=sizeof(new_addr);
    Second_Socket= accept(Main_Socket,(struct sockaddr*)&new_addr, &addr_size);//the func return socket discriptor of a new 
    //socket and information of the new_png like IP and Port into new_addr.
//---------------------------------------------------------------------------------
    struct timeval start, end; //parameters for culculating time
    long long seconds = 0; // holds the actual time
    char ping[12] = " "; // this array receives the message "sended ping"
    char pong[12] = " "; // this array receives the message "sended pong"
    char *WatchDog_Message = "timeout"; //the message that we will send to new_ping if we dont get pong in time
    int flag = 0; // this flag will tell us if we got the pong before timeout
//---------------------------------------------------------------------------------
//the timer
    while(1){
        recv(Second_Socket,ping,12,MSG_DONTWAIT); //receives the message "sended ping"
        if(!strcmp(ping,"sended ping")){ // check if we got the correct message
            bzero(ping,12); // reset the array 
            gettimeofday(&start, 0); // start clock
            flag = 0; // reset the flag
        }
        else{ // if we didnt get ping we will start again from the beginning
            continue;
        }
        while(seconds < 10){ //loop that checks if we got pong under 10 seconds
            recv(Second_Socket,pong,12,MSG_DONTWAIT); // receives the message "sended pong"
            if(!strcmp(pong,"sended pong")){ // check if we got the correct message
                bzero(pong,12); // reset the array 
                flag = 1; // we got the pong message before timeout
                break;
            }
            gettimeofday(&end, 0); // end clock
            seconds = ((end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f) * 0.001;//calculate time
        }
        if(!flag){ //if we didnt get pong before timeout then we will enter to the "if"
            send(Second_Socket, WatchDog_Message,sizeof(WatchDog_Message),0); // sending "timeout" to new_ping
            printf("-closing watchdog.\n");
            close(Second_Socket); //closing the socket that communicates with new_ping
            return 0;
        }
    }
        return 0;
}

