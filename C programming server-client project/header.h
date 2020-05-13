#ifndef header
#define header


/*
Interrupt_handeler(int signal)
---------------------------------------------------------
takes in interrupt signal and prints the number, it also sets the global variabel inerrupt_recieved to 1. 
---------------------------------------------------------
*/
int interrupt_recieved = 0;
void interrupt_handler(int signal){
	printf("\nInterrupt signal recieved: %d\n", signal);
	interrupt_recieved = 1;
	exit(1);
}

/*
int usage(int argc, int nr);
---------------------------------------------------------
takes in argument counter, argc from main and and compares
it to the the other arguments nr. if argc is less it wil return 1, otherwise 0.

Inspired by the usage function in lecture 23.October
---------------------------------------------------------
*/

int usage(int argc, int nr) {
    if(argc < nr) {
        return 1;
    }
    return 0;
}
/*
int *port2int(unsigned short *dest, char *src);
---------------------------------------------------------
port2int takes in port values as string, and casts it to an unsigned short int.
if successfull port2int wil return 0, and 1 if unsuccesfull.

Inspired by the get_port function in lecture 23.October
---------------------------------------------------------
*/
int port2int(unsigned short *port, char *str_port) {
    char* nullpointer;
    int tmp = strtol(str_port, &nullpointer, 10);
    if(nullpointer == str_port && tmp == 0) {   
        return 1;
    }
    *port = (unsigned short) tmp;
    return 0;
}

/*
int ordrelokke(unsigned char msg[4]);
----------------------------------------------------------
Ordrelokke handles commands from the user which wil later be sent to server 
----------------------------------------------------------
*/
int ordrelokke(unsigned char msg[4],int* ordre_p, int* ant_p){
	int tmp;int ordre;short int ant;
  printf("----------------------------------------------------\n");
	printf("Ordreliste\n");
	printf("----------------------------------------------------\n");
	printf("Ordre\n1\t\t\t henter en jobb fra serveren\n2\t\thenter X antall jobber fra serveren\n3\t\t\thenter alle jobber på serveren\n4\t\t\t avslutter programmet\n");
	printf("----------------------------------------------------\n");
	printf("\n");
	printf("Skriv ordrenr:\t");
	msg[2] = 0x00;msg[1] = 0x00;msg[0] = 0x00;
  tmp = scanf("%d", &ordre);
	if(tmp == EOF){
		msg[3] = 0x80;
		return 0;
	}
	*ordre_p = ordre;
	if(ordre ==1 ){
			msg[3] = 0x01;		
			return 1;
	}else if(ordre == 2){
			printf("Skriv inn antall jobber fra serveren:\t");
			tmp = scanf("%hu", &ant);
			*ant_p = ant;
			msg[3] = 0x02;
			if(ant>0xff){
				msg[1] = (unsigned char)(ant>>8);
			}
			msg[0] = ((unsigned char)ant & 0xff);
			return 2;
	}else if(ordre == 3){
			msg[3] = 0x04;	
			return 3;
	}else if(ordre == 4){
			msg[3] = 0x40;
			return 0;		
	}else{
		printf("Ordrenr må være et tall som oppgitt, bruker skrev %d",ordre);
	}
	return 1;
}

/*

*/

/*
int checksum(char* str)
----------------------------------------------------------------
checksum takes in string and adds the value of evry char, then calculates the modulo 32 the sum. 

if string is NULL -1 will be returned. Otherwise the sum wil be returned.
----------------------------------------------------------------
*/

int checksum(unsigned char* str, int len){
	if(str == NULL){
		return -1;
	}
	int sum = 0;int i;
	int ant = len;
	for(i=0;i<ant;i++){
	sum = sum + str[i];
	}
	sum = sum % 32;
	return sum;
	
} 

/*
char* read_job(FILE *file, char *ch, int *len)
----------------------------------------------------------

Arguments
  - file pointer file,  file to read from
  - char pointer job_type, destination for job type
  - int pointet len, destination for length of job text

Return value 
  - char pointer job_txt to job text, if sucessfull. 
    Otherwise returns NULL

read job, takes in filepointer to opened file, from which
it reads the jobtype to destination of ch pointer, and length
of job text to destination of len pointer, then the function
mallocs to the size of the job text, and reads the job text to the job_txt. And returns buf, the pointer buf to the malloced area. If unsuccessful NULL is returned. 

----------------------------------------------------------
*/

char* read_job(FILE *file, char* job_type, unsigned int* len, int* end){
  unsigned int tmp;char* job_txt;
	if(feof(file)){
		*job_type = 'Q';
		*len = 0;
		*end = 1;
	  return NULL; 
	}
	tmp = fread(job_type,1,sizeof(char),file);
	//printf("FREAD TYPE: %d\t",tmp);
	if(ferror(file)||tmp != 1){  
	  *job_type = 'Q';
		*len = 0;
		*end = 1;
	  return NULL; 
	}
	tmp = fread(len,1,sizeof(int),file);
	if(ferror(file)||tmp != sizeof(int)){  
		*job_type = 'Q';
		*len = 0;
		*end = 1;
	  return NULL; 
	}
	//printf("FREAD LEN: %d\n",tmp);

	//printf("job len: %d\n",*len);
	
	job_txt = calloc(*len*sizeof(char)+1,1);
	//job_txt[*len];
	tmp = fread(job_txt,sizeof(char),*len,file);
	//printf("FREAD JobbLEN: %d\t len %d\n",tmp,*len);
	if(tmp != *len || ferror(file)){  
	  free(job_txt);
	  job_txt = NULL;
		*job_type = 'Q';
		*len = 0;
		*end = 1;
	  return job_txt; 
	}
	// printf("JOBB>%s<END\n",(job_txt));
	return job_txt;
	
}

/*
char* msg_format

Arguments:
	-	char* job_txt
	-	char job_type
	- unsigned int len

Return value:
	-	char* msg2client

Description:
Msg_format takes in 3 arguments the dynamic allocated array Job_txt, the char Job_type, and the unsigned int len. The function reallocates the array job_txt to increased size of 5 more elements, and shifts the already contained elements 5 placed to the right. Then it fills in the first byte with the job type description bits and the check sum value(see checksum). The next 4 bytes is filled by the int value of len. This reallocated and modified array is returned as msg2client if successfull, other wist the return value is NULL.       

*/
char* msg_format(char* job_txt,char job_type,unsigned int len){
	unsigned int bits = 0;
	char* msg2client;
	unsigned char ch_sum = 0;
  
	if(job_txt == NULL){
		msg2client = calloc(5,1);
  }else{  	
		unsigned char ch_sum = checksum(job_txt,len);
		msg2client = realloc(job_txt,len+5);
		memset(msg2client+len,0,5);
 		memmove(msg2client+5, msg2client,len);
		printf("CHSUM %x\t",ch_sum);
		printf("LEN %d\n",len);
		printf("JOBB TYPE BITS %x\n",bits);
	}
	switch(job_type){
		case 'O':
			bits = 0x00;
			break;
		case 'E':
			bits = 0x01;
			break;
		case 'Q':
			bits = 0x07;
			break;
	}

	msg2client[0] = (bits<<5)|ch_sum;
  msg2client[1] = (unsigned char)(len);
	msg2client[2] = (unsigned char)(len>>8);
  msg2client[3] = (unsigned char)((len>>16));
  msg2client[4] = (unsigned char)((len>>24));
  return msg2client;
}


char* pack_jobs(char* jobs, char* msg2Client,int len,int tot_len){
	
	char* new_jobs = realloc(jobs,tot_len);
	if(len!=0){
	memcpy(new_jobs+(tot_len-(len+5)),msg2Client,len+5);
	}else{
	memcpy(new_jobs+(tot_len-5),msg2Client,5);
	/*
	if(msg2Client!=NULL){
		printf("msg >%s<",msg2Client);
	}else{
		printf("NULL");
	}*/
}
	
	return new_jobs;
}
	

 


/*
----------------------------------------------------------
char* msg_unformat

Arguments:
	-	unsigned char msg2client[]
	-	unsigned char* jobbtype
	-	int* ch_match
	- int* len

Return value:
	-	char* jobb_text

Description:
Msg_unformat takes in static array, containing message from server. Moreover it takes in char pointer jobbtype, which is the destination to the information containing jobbtype. It also takes in the pointer ch_match which contains 1 if calculated check sum of recieved job text is equal to the check sum contained in the message. Otherwise 0. Last parameter is the pointer len, which is the destination for the length of the job text. The return value if sucessfull is a dynamically allocated array job_text conataining the job text. If unsucessfull the return value is NULL. 

----------------------------------------------------------

*/
/*
char* msg_unformat(unsigned char msg2client[], unsigned char* jobbtype, int* ch_match, unsigned int* len){
	if(msg2client == NULL){
		return NULL;
	}

  *jobbtype = (unsigned char)(msg2client[0] >> 5);
	unsigned char msg_ch_sum = (msg2client[0] & 0x1f);

	//unsigned char ch_sum = checksum(jobb_txt);
	if(msg_ch_sum == ch_sum){
		*ch_match = 1;
	}else{
		*ch_match = 0;
	}
	return jobb_txt;
 
}*/

unsigned int bytes2int(unsigned char int_bytes[]){
  unsigned int len = (unsigned char)((int_bytes[0])|(int_bytes[1]<<8)|(int_bytes[2]<<16)|(int_bytes[3]<<24));
  return len;
}

void type_sum(unsigned char byte, unsigned char* jobbtype,unsigned char* msg_sum){
  *jobbtype = (unsigned char)(byte >> 5);
	*msg_sum = (unsigned char)(byte & 0x1f);
	//printf("jobbtype %x",*jobbtype);
  }

int sum_match(unsigned char* jobtekst, unsigned char msg_sum,int len){
  unsigned char ch_sum = checksum(jobtekst,len);
	printf("msg %x chk %x\n",msg_sum,ch_sum);
  if(msg_sum == ch_sum){
		return 1;
	}else{
		return 0;
	}
}







#endif
	

