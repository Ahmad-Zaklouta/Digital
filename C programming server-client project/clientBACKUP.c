	#include <netdb.h> 
	#include <stdio.h>
	#include <string.h> 
	#include <unistd.h>
	#include <stdlib.h>
	#include <signal.h>
	#include <netinet/in.h> 
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include "header.h"


	int main(int argc, char* argv[]) {

		
		if(usage(argc,3)) {
			printf("Feil antall argumenter\n");
		 	return EXIT_SUCCESS;
		}
		
		struct sockaddr_in serveraddr; 
		int sock;
		int tmp;int ordre_tmp;
		unsigned short port_nr;
		unsigned char msg_sum; unsigned char jobbtype;
		unsigned char msg[4] = {0,0,0,0};
		struct addrinfo hints;
		struct addrinfo *result;

		// Handling signal interrupts
		struct sigaction sigac;
		memset(&sigac, 0, sizeof(struct sigaction));
		sigac.sa_handler = &interrupt_handler;
		sigaction(SIGINT, &sigac, NULL);
	
		// Creating Child processes

		int pipefd1[2];
		if(pipe(pipefd1) == -1){
			perror("Pipe error");
			return EXIT_FAILURE;
		}

		int pipefd2[2];
		if(pipe(pipefd2) == -1){
			perror("Pipe error");
			return EXIT_FAILURE;
		}
	
		pid_t pid1 = fork();	
		if(pid1 == -1){ 
			perror("pid1:\tFork error");
			return EXIT_FAILURE;
		}else if(pid1 == 0){ // child1
			close(pipefd1[1]);
			unsigned char len[4];unsigned char* job_txt;int i=0;
			while(1){
			if(read(pipefd1[0],len, 4) ==-1){
				perror("pid[1] read");
				return EXIT_FAILURE;
			}
			int size = bytes2int(len);
			if(size == 0){
				break;
			}
			job_txt = calloc(size+1,1);			
/*
			if(i==0){
				job_txt = calloc(size+1,1);			
			}else{ 
				job_txt = realloc(job_txt,size+1);
			}*/
			if(read(pipefd1[0],job_txt,size)==-1){
				perror("read");
				close(pipefd1[0]);
				return EXIT_FAILURE;
			}		
			job_txt[size] = '\0';
			fprintf(stdout,"%s",job_txt);
			memset(len,0,4);
			//memset(job_txt,0,size+1);
			fflush(stdout);
			i++;
			free(job_txt);
			job_txt = NULL;
			}
			//free(job_txt);	
			exit(0);
		}else if(pid1>0){ //parent
			close(pipefd1[0]);
			pid_t pid2 = fork();	
			if(pid2 == -1){ // error
				perror("pid2:\tFork error");
				return EXIT_FAILURE;
			}else if(pid2 == 0){ // child2
				close(pipefd2[1]);
			unsigned char len[4];unsigned char* job_txt;int i=0;
			while(1){
			if(read(pipefd2[0],len, 4) ==-1){
				perror("pid[2] read");
				return EXIT_FAILURE;
			}
			int size = bytes2int(len);
			if(size == 0){
				exit(0);
			}
			if(i==0){
				job_txt = calloc(size*sizeof(char)+1,1);			
			}else{ 
				job_txt = realloc(job_txt,size*sizeof(char)+1);
				memset(job_txt,0,size);
			}
			if(read(pipefd2[0],job_txt,size)==-1){
				perror("pid[2] read");
				close(pipefd1[1]);
				return EXIT_FAILURE;
			}		
			job_txt[size+1] = '\0';
			fprintf(stderr,"%s",job_txt);
			memset(len,0,4);
			i++;
			}
			close(pipefd2[0]);
		
			}else if(pid2>0){ // parent
				close(pipefd2[0]);

		// Establishing connection to Server
	
		// helps getaddrinfo find the adress
		memset(&hints, 0, sizeof(struct addrinfo));
		         hints.ai_family = AF_INET;
		         hints.ai_socktype = SOCK_STREAM;
		         hints.ai_flags = 0; 
		         hints.ai_protocol = IPPROTO_TCP;

		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sock == -1){
			perror("Socket init");
			return EXIT_FAILURE;
		}
	
		tmp = port2int(&port_nr,argv[2]);	
		if(tmp == 1) {
		  printf("Failed to convert port, port has to be an integer, ");
			return EXIT_SUCCESS;
		 }

		// makes it possible to use server name as well
		tmp = getaddrinfo(argv[1], argv[2],&hints,&result);
		if(tmp){
				const char *s = gai_strerror(tmp);
				printf("Adress / server name first, then port\nuser typed: \naddress: %s \tport: %s\n", argv[1], argv[2]);
				printf("%s\n",s);
				freeaddrinfo(result);
				return EXIT_FAILURE;
		}

	
		memset(&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(port_nr);

		tmp = inet_pton(AF_INET, argv[1], 				                  &serveraddr.sin_addr);
		if(tmp < 0){
			perror("inet_pton");
			close(sock);
			return EXIT_FAILURE;
		}
	
		tmp = connect(sock, (struct sockaddr *)&serveraddr, 							sizeof(serveraddr));

		freeaddrinfo(result);
		if(tmp == -1){
			perror("Connect");
			close(sock);
			return EXIT_FAILURE;
		}
		int ordre;int antall;int i;
		unsigned char* jobbtekst = NULL;
		unsigned char* new_jobbtekst = NULL;
		while(1){
			//fflush(stdin);
			ordre_tmp = ordrelokke(msg,&ordre,&antall);	 
			if(write(sock,msg,sizeof(msg)) == -1){
				perror("write");
				close(sock);
				return EXIT_FAILURE;	
			}
			if(!ordre_tmp){
				break;  
			}
			if(ordre == 1){
			antall = 1;
			}	
			if(ordre == 3){
			antall = 10000;
			}
			int tmp_len = 0;		
			for(i=0;i<antall;i++){
			
			unsigned char job[1];
			tmp = read(sock,job, 1);
			if(tmp ==-1){
		 	  perror("read1");
				close(sock);
				return EXIT_FAILURE;
			}	
			unsigned char int_bytes[4];
      tmp = read(sock, int_bytes, 4);
      if(tmp ==-1){
		 	  perror("read2");
				close(sock);
				return EXIT_FAILURE;
			}
			
			unsigned int msg_len = bytes2int(int_bytes);
			//printf("LEN %d", msg_len);
			jobbtekst = (char*)calloc(msg_len,1);
			/*
			if(msg_len > 0 && msg_len > tmp_len){
			if(i==0){
				jobbtekst = (char*)calloc(msg_len,1);
			}else{
				memset(jobbtekst,0,tmp_len);
			if(new_jobbtekst = realloc(jobbtekst,msg_len)){
				jobbtekst = new_jobbtekst;
			}else{
				perror("Jobbtekst realloc");
				close(sock);
				free(jobbtekst);
				jobbtekst = NULL;
				return EXIT_FAILURE;
			}

			}
		//	memset(jobbtekst,0,msg_len*sizeof(char)+1);			
			}*/
			//memset(jobbtekst,0,msg_len*sizeof(char)+1)
      tmp = read(sock, jobbtekst, msg_len);
      if(tmp ==-1){
		 	  perror("read2");
				close(sock);
				return EXIT_FAILURE;
			}
			//jobbtekst[msg_len-1]='\0';
			//printf("jobb[%d]=>%s<END\n\n",i,jobbtekst);
			type_sum(*job,&jobbtype,&msg_sum);
			int match = sum_match(jobbtekst,msg_sum,msg_len);
			//printf("Len %d\n",msg_len);
			//printf("match : %d\n",match);
			//printf("jobbtype : %x\n", jobbtype);
			if(match){
				switch(jobbtype){
					case 0x00:		
			//			printf("Jobb 0x00\n");	
					/*	if(write(pipefd1[1],&msg_len,sizeof(msg_len))==-1){
							perror("pipe1 write1");
							close(pipefd1[1]);
							close(pipefd2[1]);
							close(sock);
							return EXIT_FAILURE;
						}			
						if(write(pipefd1[1],jobbtekst,msg_len)==-1){
							perror("pipe1 write2");
							close(pipefd1[1]);
							close(pipefd2[1]);
							close(sock);
							return EXIT_FAILURE;
						}*/
						break;
					case 0x01:
				//		printf("Jobb 0x01\n");
						if(write(pipefd2[1],&msg_len,4)==-1){
							perror("pipe2 write1");
							close(pipefd1[1]);
							close(sock);
							return EXIT_FAILURE;
						}
						if(write(pipefd2[1],jobbtekst,msg_len)==-1){
							perror("pipe2 write2");
							close(pipefd2[1]);
							close(sock);
							return EXIT_FAILURE;
						}
						break;
					case 0x07:
						//kill(pid1,SIGKILL);
						//kill(pid2,SIGKILL);
				/*		printf("Jobb 0x07\n");
						if(write(pipefd1[1],0,4)==-1){
							perror("pipe1 write3");
							close(pipefd2[1]);
							close(sock);
							return EXIT_FAILURE;
						}
						if(write(pipefd2[1],0,4)==-1){
							perror("pipe2 write3");
							close(pipefd2[1]);
							close(sock);
							return EXIT_FAILURE;
						}  */
						close(sock);
						free(jobbtekst);
						return EXIT_SUCCESS;	
					
				} // end switch
			} // if end	  */  
			memset(int_bytes,0,4);
			tmp_len = msg_len;
			free(jobbtekst);
			} // for end  
	//	return;
		//free(jobbtekst);
		sleep(1);   
		}	// end while 
		close(sock);
		//free(jobbtekst);
		return EXIT_SUCCESS;	
	 	} // parent 2
		}	// parent 1
	}	// main
