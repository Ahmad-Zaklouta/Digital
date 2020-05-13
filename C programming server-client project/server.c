	#include <netdb.h> 
	#include <stdio.h>
	#include <string.h> 
	#include <unistd.h>
	#include <stdlib.h>
	#include <signal.h>
	#include <netinet/in.h> 
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include "header.h"
	/*
	Har tatt utgangspunkt i eksempel serverkode som gitt i forelsening uke 43. 

	*/

	int main(int argc, char* argv[]){

		if(usage(argc,3)) {
					printf("Feil antall argumenter\n");
		      return EXIT_SUCCESS;
		  }	
	
		// initialiserer variabler
		char msg[5];char job_type; 
		unsigned int len;int tot_len = 0;
		char* msg2client;char* jobs;
		char* job_txt = NULL;int i;
	
		struct sockaddr_in serveraddr, clientaddr; 
		socklen_t clientaddrlen = sizeof(clientaddr);
		int request_sock, sock;int tmp;
		unsigned short port_nr;
	
		// Handling signal interrupts
		struct sigaction siga;
		memset(&siga, 0, sizeof(struct sigaction));
		siga.sa_handler = &interrupt_handler;
		sigaction(SIGINT, &siga, NULL);

		// Opening file
		FILE *file = fopen(argv[1],"r");
		if(file == NULL){
			perror("File opening:");
			return EXIT_SUCCESS;
		}

		// oppretter request tilgangspunkt
		request_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(request_sock  == -1){
				perror("request socket\n");
				return EXIT_FAILURE;
			}
		// tilatter rebinding av port. 
		int optval = 1;
		tmp = setsockopt(request_sock, SOL_SOCKET, SO_REUSEADDR, &optval,sizeof(int));
		if(tmp == -1){
			perror("setsockopt");
			return EXIT_FAILURE;
		}

		tmp = port2int(&port_nr,argv[2]);	
		if(tmp == 1) {
		  printf("Failed to convert port, port has to be an integer, ");
			return EXIT_SUCCESS;
		  }

			// fyller inn tilkoblings-info til serveraddr
			memset(&serveraddr, 0, sizeof(serveraddr));
		  serveraddr.sin_family = AF_INET;						
		  serveraddr.sin_addr.s_addr = INADDR_ANY;
		  serveraddr.sin_port = htons(port_nr); 	

			//assosierer request socket med server info
			tmp = bind(request_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
			if(tmp == -1){
				perror("Bind");
				close(request_sock);
				return EXIT_FAILURE;
			}
			// setter request_sock til å være passiv lytter
			tmp = listen(request_sock, 1);
			if(tmp==-1){
				perror("listen");
				close(request_sock);
				return EXIT_FAILURE;
			}
			// godtar tilkoblingsforsøk, fyller inn info
			sock = accept(request_sock, (struct sockaddr *)&clientaddr, &clientaddrlen);
			if(sock==-1){
				perror("accept");
				close(request_sock);
				return EXIT_FAILURE;
			}
			int end;
			while(1){	
				
				if(read(sock, msg, 4) == -1){
					perror("read");
					close(sock);
					close(request_sock);
					fclose(file);
					return EXIT_FAILURE;
				}   		
				int ordre = msg[3];
				unsigned char ant1 = msg[1];
				unsigned char ant0 = msg[0];			
				int ant = (ant1 << 8)|ant0;
				printf("ordre: %d ant: %d\n",ordre,ant);
		

			// executing commands
				switch(ordre){ 
					case 0x80:	// klient har feil
						fclose(file);
						close(sock);
						close(request_sock);
						printf("Client was terminated by an error, servers is shutting down.\n");
						return EXIT_FAILURE;
					case 0x40: // klient avslutter
						fclose(file);
						close(sock);
						close(request_sock);
						printf("Client was terminated by user, server is shutting down.\n");
						return EXIT_SUCCESS;				
					case 0x04: // alle jobber
						end = 0;i=0;
						while(1){
						  msg2client = read_job(file,&job_type,&len,&end);
						//	printf("END %d\t",end);
						//	printf("JOBBTYPE[%d]=%c\n",i,job_type);
							printf("JOBB[%d]>%s<SLUTT\n",i, msg2client);

				      msg2client = msg_format(msg2client,job_type, len);
							tot_len += len+5;		
							if(i==0){
							jobs = msg2client;	
							}else{
							jobs = pack_jobs(jobs,msg2client, len,tot_len);	
							}
							if(end){
							break;
							}
							i++;
							}
							if(write(sock,jobs,tot_len) == -1){
								perror("Hent alle write");
						    close(sock);
						    close(request_sock);
						    free(msg2client);
				        msg2client = NULL;
				        return EXIT_FAILURE;	
						}
						break; 
					case 0x02: // ant jobber
						printf("ANT %d\n",ant);
						end = 0;
						for(i=0;i<ant;i++){
							
				      msg2client = read_job(file,&job_type,&len,&end);
							if(end){
								break;
							}		
					//		printf("job type: %c\n",job_type);			
				      msg_format(msg2client,job_type, len);
							tot_len += len+5;		
							if(i==0){
							jobs = msg2client;		
							}else{
							jobs = pack_jobs(jobs,msg2client, len,tot_len);	
							}
							
							printf("jobb[%d]=>%s<END\n\n",i,jobs+(tot_len-(len)));
						  }
							 if(write(sock,jobs,tot_len) == -1){
									perror("X jobb write");
								  close(sock);
								  close(request_sock);
								  free(msg2client);
						      msg2client = NULL;
							    return EXIT_FAILURE;	
							  }
								if(jobs != NULL){
								free(jobs);jobs=NULL;
								jobs == NULL;
								}else if(msg2client!=NULL && jobs == NULL){
								free(msg2client);
								msg2client = NULL;
								}
							break;
						case 0x01: // en jobb
							end = 0;
							job_txt = read_job(file,&job_type,&len,&end);		
							msg2client = msg_format(job_txt,job_type,len);
							tmp = write(sock,msg2client,len+5);
						 break;
							} // switch end	
								       
							if(msg2client == NULL && job_txt != NULL){
								free(job_txt);
							job_txt = NULL;
						}else if(msg2client != NULL && job_txt == 	NULL){
							free(msg2client);
							msg2client = NULL;
						}

							
					//return 1;
					}	// while end	
		
					fclose(file);
					close(sock);
					close(request_sock);
					return EXIT_SUCCESS;
				} // main end

		

