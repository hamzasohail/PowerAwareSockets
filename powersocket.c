#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <assert.h>
#include <sys/wait.h>

// void set_deadline();

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    int n;
    struct sockaddr_in serv_addr; 

    char readBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(readBuff, '0', sizeof(readBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 
   
  // This program is supposed to listen on a tcp port and 
  // and listen to frequency change requests. It requires 
  // the tcp socket_RTT time it took from request-to-response
  // THe interface offers setting up a deadline for socket
  // RTTs. Anything less than that and this app reduces the
  // clock frequency just enough so that the deadline is met
  // and in the other case , it assumes a certain IPC and tries
  // to make up for the slow response time by increasing compute
  // for a certain amount of time.

  while (1) {

       // connection made by an pp, request came on socket
       // what is the RTT ?
       // Is it lesser than the deadline or greater ?
       // a. lesser -> how much should we slow down and for how long ?
       // b. greater -> how much should we increase and for how long ?

        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        ticks = time(NULL);
        snprintf(readBuff, sizeof(readBuff), "%.24s\r\n", ctime(&ticks));
        n = read(connfd, readBuff, strlen(readBuff)); 
	assert (n > 0);
        readBuff[n]='\0';
	// call atoi to convert 	
	int rtt = atoi(readBuff);
	// the interface assumes the rtt is in milliseconds
	// launch the cpufreqdriver app here
        pid_t pid=fork();
	if (pid == 0){
	  /* child process */
	  char * arguments[3] = {"/bin/ls", "-l", "./"};
	  execv("/bin/ls",NULL);
	  exit(127);
	} else {
	  waitpid(pid,0,0);
	}
        close(connfd);

       // call sleep after setting the frequencies based on the amount of 
       // time the program is supposed to run for that frequency

        sleep(1);      
 
  }


  return 0;
}
