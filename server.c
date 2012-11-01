/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *		  Project:  Operating Systems I - Project 1 - 5th Semester 2012
 *    Description:  Accepts orders
 *
 *        Version:  1.0
 *        Created:  10/30/2012 02:14:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Seimenis Spiros 5070 
 *   Organization:  Computer Engineering and Informatics Department
 *                  University of Patras
 *
 * =====================================================================================
  */
#include "common.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>
#include <sys/wait.h>
#include <sys/un.h>

#include <sys/ipc.h>
#include <sys/ipc.h>

/*Size of request queue*/
#define LISTENQ  20


/*  Helper function to avoid zombie processes */
void sig_chld(int signo);

int main(int argc, char **argv){
	
	/* definitions of standard times (used better with the enums defined in common.h)*/	
	int timeofPizza[]={10,12,15};
	int timeofClient[]={5,10};

	/* holds the client order information in the following way
	 *      |-----------------------------------------------|
	 * 		|pizza #1 | pizza #2 | pizza #NPIZZA | near/far |
	 *      |-----------------------------------------------|
	 *		to order 2 peperonis, 1 special for far is 1|1|2|1
	 *		according to the enum defined in common.h
	 */
	char buffer[NPIZZAS+1];

	int recv_length=1;

	/* Standard socket creation */
	int listenfd, sockfd;
	pid_t childpid;
	socklen_t client_size;
	struct sockaddr_un server_addr, client_addr;

	signal(SIGCHLD, sig_chld); 	/* sigchld handler to avoid zombie process generation*/
	unlink(UNIX_PATH);			/* deletes the specified file from disk to use as socket */

	if ((listenfd = socket(AF_LOCAL, SOCK_STREAM, 0))==-1)
		fatal("in socket");

	bzero(&server_addr,sizeof(server_addr));
	server_addr.sun_family = AF_LOCAL;
	strcpy(server_addr.sun_path, UNIX_PATH);
	
	if (bind(listenfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1 )
		fatal("binding to socket");

	/* begin listening to socket a.k.a begin taking orders from clients */
	if (listen(listenfd, LISTENQ)==-1)
		fatal("listening to socket");

	/* setting shared memory of the server */
	int shmid;
	key_t key;
	key = SHMGLOBAL;
	int shm;
	
	/* struct for storing the delivery boys and roasters status using bitfields */
	struct info_struct {
		unsigned int bak:NBAKERS;
		unsigned int deliv:NDELIVERY;

	} server_info;

	//test
	server_info.bak |=0x00000010;
	printf("%x\n",server_info.bak);

	//test

	if ((shmid = shmget(key,SHMSZ,IPC_CREAT | 0666))<0)
		fatal("in shmget");
	
	if ((shm = shmat(shmid,NULL,0))==-1)
		fatal("in shmat");



	while(1){
		client_size = sizeof(client_addr);

		sockfd = accept(listenfd, (struct sockaddr*)&client_addr,&client_size);
		if(sockfd < 0 ){
			if (errno == EINTR ) /* Interrupt received */
				continue;
			else
				fatal("in accepting connection");
		}

		/* fork to handle an order exclusively */
		childpid=fork();

		if(childpid==0){ /* child process */
			close(listenfd);/* no reason to continue listening for orders */
			send(sockfd, "Server: Pizza Ceid, tell me your order!\n",39,0);
			recv(sockfd,&buffer,(NPIZZAS+1)*sizeof(char),0);

			/* Setting shared memory for use between each order-dealing process and bakers/deliveries  */
			int shmpid;
			int shmp;
			key_t child_key = getpid();
			if ((shmpid = shmget(child_key,SHMSZ,IPC_CREAT | 0666))<0)
				fatal("in shmget");
		
			if ((shmp = shmat(shmpid,NULL,0))==-1)
				fatal("in shmat");http://netcins.ceid.upatras.gr/OpSys-I/index.php

			/* this variable holds a number that indicates how many pizzas remain until the order is complete  */
			/* a baker as a process will be given the shared memory of the pizza it habdles and will subrtact  */
			/* when he finished making the pizza of this process */
			int check = strlen(buffer)-1;

			/* order parser */
			int i;
			char type = buffer[strlen(buffer)-1];
			printf("%c\n", type);
			for (i=strlen(buffer)-2;i>=0; i--){
				/* code that constantly checking for free baker to apply a pizza to him */
				printf("Pizza of type %c\n",buffer[i]);
				
				/* code to check for a free baker */



			}
			exit(0);
		}//Here terminates the cild

		close(sockfd);

	}
	shmdt()
	shmctl();
}

void sig_chld( int signo) {
       pid_t pid;
       int stat;

       while ( ( pid = waitpid( -1, &stat, WNOHANG ) ) > 0 ) {
              printf( "Child %d terminated.\n", pid );
        }
 }


