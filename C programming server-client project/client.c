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
			unsigned char len[4];
			unsigned char* job_txt;
			int i=0;int end;int size;
			while(1){
				if(read(pipefd1[0],&end, 4) ==-1){
					perror("pid[1] read");
					return EXIT_FAILURE;
				}
				if(end){
					break;
				}
				
				if(read(pipefd1[0],len, 4) ==-1){
					perror("pid[1] read");
					return EXIT_FAILURE;
				}
				size = bytes2int(len);
				if(i==0){
					job_txt = calloc(size+1,1);			
				}else{ 
					job_txt = realloc(job_txt,size+1);
				}
				if(read(pipefd1[0],job_txt,size)==-1){
					perror("pid[1] read");
					close(pipefd1[0]);
					return EXIT_FAILURE;
				}		
				job_txt[size] = '\0';
			//	fprintf(stdout,"%s",job_txt);
				memset(len,0,4);
				memset(job_txt,0,size);
				fflush(stdout);
				i++;
			}	//	while end
			free(job_txt);	
			close(pipefd1[0]);
			exit(0);
		}else if(pid1>0){ //parent
			close(pipefd1[0]);
			pid_t pid2 = fork();	
			if(pid2 == -1){ // error
				perror("pid2:\tFork error");
				return EXIT_FAILURE;
			}else if(pid2 == 0){ // child2
				close(pipefd2[1]);
				unsigned char len[4];
				unsigned char* job_txt;
				int i=0;int end=0;int size;
				while(1){
					if(read(pipefd2[0],&end, 4) ==-1){
						perror("pid[1] read");
							return EXIT_FAILURE;
					}
					if(end){
						break;
					}
					if(read(pipefd2[0],len, 4) ==-1){
						perror("pid[2] read");
						return EXIT_FAILURE;
					}
					size = bytes2int(len);
					if(i==0){
						job_txt = calloc(size+1,1);			
					}else{ 
						job_txt = realloc(job_txt,size*sizeof(char)+1);
						memset(job_txt,0,size);
					}
					if(read(pipefd2[0],job_txt,size)==-1){
						perror("pid[2] read");
						close(pipefd1[1]);
						return EXIT_FAILURE;
					}
					job_txt[size] = '\0';
					//fprintf(stderr,"%s",job_txt);
					memset(len,0,4);
					fflush(stderr);
					i++;
				}
				close(pipefd2[0]);
				free(job_txt);
				exit(0)	;
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
		int ordre;int antall;int i;int msg_len;int match;
		unsigned char* jobbtekst = NULL;
		unsigned char* new_jobbtekst = NULL;	
		unsigned char job;	
		unsigned char int_bytes[4];
		int end;
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
			}else if(ordre == 1){
			antall = 1;
			}else if(ordre == 3){
			antall = 1000;
			}
			for(i=0;i<antall;i++){
		//	while(1){	
				if(read(sock,&job, 1) ==-1){
			 	  perror("read1");
					break;
				}			
		    if(read(sock, int_bytes, 4) ==-1){
			 	  perror("read2");
					break;
				}			
				msg_len = bytes2int(int_bytes);			
				if(msg_len > 0){
					if(i==0){
						jobbtekst = calloc(msg_len,1);
					}else{
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
					memset(jobbtekst,0,msg_len);	
					if(read(sock,jobbtekst, msg_len) ==-1){
			 	 	 	perror("read2");
						free(jobbtekst);
						jobbtekst = NULL;
						close(sock);
						return EXIT_FAILURE;
					}				
					}		
				

				printf("JOBB BYTE %x\n",job);
				printf("jobb[%d]=>%s<END\n\n",i,jobbtekst);
				type_sum(job,&jobbtype,&msg_sum);
				match = sum_match(jobbtekst,msg_sum,msg_len);
				printf("Len %d\t",msg_len);
				printf("match : %d\n",match);
				printf("jobbtype : %x\n", jobbtype);
				if(match){
					switch(jobbtype){
						case 0x00:
							end = 0;
							if(write(pipefd1[1],&end,4)==-1){
								perror("pipe1 write1");
								close(pipefd1[1]);
								close(pipefd2[1]);
								close(sock);
								return EXIT_FAILURE;
							}		
							if(write(pipefd1[1],&msg_len,sizeof(msg_len))==-1){
								perror("pipe1 write2");
								close(pipefd1[1]);
								close(pipefd2[1]);
								close(sock);
								return EXIT_FAILURE;
							}			
							if(write(pipefd1[1],jobbtekst,msg_len)==-1){
								perror("pipe1 write3");
								close(pipefd1[1]);
								close(pipefd2[1]);
								close(sock);
								return EXIT_FAILURE;
							}
							break;
						case 0x01:
							end = 0;
							if(write(pipefd2[1],&end,4)==-1){
								perror("pipe2 write1");
								close(pipefd1[1]);
								close(pipefd2[1]);
								close(sock);
								return EXIT_FAILURE;
							}		
							if(write(pipefd2[1],&msg_len,sizeof(msg_len))==-1){
								perror("pipe2 write2");
								close(pipefd1[1]);
								close(sock);
								return EXIT_FAILURE;
							}
							if(write(pipefd2[1],jobbtekst,msg_len)==-1){
								perror("pipe2 write3");
								close(pipefd2[1]);
								close(sock);
								return EXIT_FAILURE;
							}
							break;
						case 0x07:			
							end = 1;
							if(write(pipefd1[1],&end,4)==-1){
								perror("pipe1 write3");
								close(pipefd2[1]);
								close(sock);
								return EXIT_FAILURE;
							}
							if(write(pipefd2[1],&end,4)==-1){
								perror("pipe1 write3");
								close(pipefd2[1]);
								close(sock);
								return EXIT_FAILURE;
							}
							close(sock);
							free(jobbtekst);
							return EXIT_SUCCESS;	
					
						} // switch end
					} // if end	  

				memset(int_bytes,0,4);
				/*i++;
				if(i == antall){
					break;
				}*/
			} // inner while end  
			sleep(1);  
			free(jobbtekst); 
		}	// outer while end 
		close(sock);
		return EXIT_SUCCESS;	
	 	} // parent 2 end
	}	// parent 1 end
}	// main end
