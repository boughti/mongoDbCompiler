#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<windows.h>
#include<dirent.h>
#include "server.h"


char* split(const char *string,char delim,int *debut){
			
	int i;
	int k=0;
	char *part = calloc(strlen(string),sizeof(char));
		
	for(i=*debut;string[i+1]!='\0';i++){
		if(string[i]==delim || string[i+2]=='\0'){
			if(string[i+2]=='\0'){
				part[k]=string[i];
				k++;
			}
			part[k]='\0';
			*debut=i+1;
			
			return part;
			
		}else{
		
			part[k]=string[i];
			k++;
		}
	}
	
	return NULL;
}

char *get_value(char *string){
	
	char *value = calloc(data_length,sizeof(char));
	
	int i,j=0;
	
	for(i=0;i<strlen(string) && string[i]!=':' && string[i]!='!' && string[i]!='<' && string[i]!='>' ;i++);
	i++;
	if(string[i]=='=') i++;
	while(i<strlen(string)){
		value[j]=string[i];
		j++;
		i++;
	}
	value[j]= '\0';
	
	return value ;
	
}

char *get_field(char *string){
	
	char *field = calloc(data_length,sizeof(char));
	
	int i;
	for(i=0;i<strlen(string) && string[i]!=':' && string[i]!='!' && string[i]!='<' && string[i]!='>' ;i++){
		field[i]=string[i];
	}
	field[i]= '\0';
	
	return field ;
	
}

int check_condition(char *part ,char *condition ){
	
	switch(condition_operation(condition)){
		case 0: 
					return(strcmp(part,condition));
		case 1: 
				if(strcmp(get_value(part),get_value(condition)) != 0 ){
					return 0;
				} 
				return 1;
				
		case 2: if(strcmp(get_value(part),get_value(condition)) < 0  )
						return 0;
				return 1;
		case 3: if(strcmp(get_value(part),get_value(condition)) <= 0  )
						return 0;
				return 1;
		case 4: if(strcmp(get_value(part),get_value(condition)) > 0  )
						return 0;
				return 1;
		case 5: if(strcmp(get_value(part),get_value(condition)) >= 0  )
						return 0;
				return 1;
				
		default : return 1; 
				
	}
	
}

int condition_operation(char *condition){
	int i;
	for(i=0;i<strlen(condition);i++){
		switch(condition[i]){
			case ':' : return 0;
			case '!' : return 1;
			case '<' : if(condition[i+1] == '=') return 3;
					    return 2;	
			case '>' : if(condition[i+1] == '=') return 5;
					    return 4;
			default : break;
		}
	}	
}

void display(char *fields[],char *document){
	
	if(fields[0]==NULL){
		send(client_socket,document,data_length,0);
		return;
	}
	
	char *fieldCopy;
	char *inside_doc = calloc(data_length,sizeof(char));
	inside_doc[0]='{';
	inside_doc[1]='\0';
	
	int document_counteur=1;
	char *field_name = split(document,',',&document_counteur);
	while(field_name!=NULL){
		
		int i=0;
		while(fields[i]!=NULL){
			
			int zero_or_one = fields[i][strlen(fields[i])-1]-'0';
			fieldCopy = calloc(data_length,sizeof(char));
			strcpy(fieldCopy,fields[i]);
			fieldCopy[strlen(fieldCopy)-1]='\0';
			
			if(strcmp(get_field(field_name),fieldCopy)==0 && zero_or_one == 1 ){
				strcat(inside_doc,field_name);
				strcat(inside_doc,",");	
			}
			
			i++;
		}
		field_name = split(document,',',&document_counteur);
	}
	
	int fin = 0;
	if(strlen(inside_doc)<=1){
		fin = 1;
	}
	
	if(strcmp(inside_doc,"{")!=0){
		inside_doc[strlen(inside_doc)-1+fin]='}';
		inside_doc[strlen(inside_doc)+fin]='\0';
		
		//printf("doc returned :%s\n",inside_doc);
		send(client_socket,inside_doc,data_length,0);
	}
}

char* find(char *collection,char* conditions[],char *fields[]){
	
	file=fopen(collection,"r");
	
	if(file==NULL){
			printf("collection doesn't existe\n");
			return "collection doesn't existe.";		
	}
	
	char document[data_length];
	char docComplet[data_length];
	
	int justProjection = 0;
	if(conditions[0] == NULL){
		justProjection = 1;
	}
	
	while(fgets(document,data_length,file)!=NULL){
		
		// removing '\n' from the end 
		document[strlen(document)-1]='\0';
		
		strcpy(docComplet,document);
		
		int conditions_nbr = 0;
		int conditions_counter=0;
		
		if(justProjection == 0 ){
			//spliting the document for the first time
			int document_beginning=1;
			char *part = split(document,',',&document_beginning);
			//if(conditions_nbr == conditions_counter) means that all conditions are verified for that document

			while(part!=NULL){
				
				for(conditions_nbr=0;conditions[conditions_nbr]!=NULL;conditions_nbr++){
					
					if(strcmp(get_field(part),get_field(conditions[conditions_nbr]))==0){
						if(check_condition(part,conditions[conditions_nbr])==0){
							conditions_counter++;
							strcpy(document,docComplet);
						}
					}
				}
				part = split(document,',',&document_beginning);
			}
		}
		if(conditions_nbr==conditions_counter){
			display(fields,docComplet);
		}
		
	}
	
	fclose(file);
	return "";
}

char *remoove(char *collection,char* conditions[],char *fields[]){
	
	file=fopen(collection,"r");
	
	int removeOne = 1, done =0;
	
	if(fields[0] == NULL || strcmp(fields[0],"false") == 0 ){
		removeOne = 0;
	}
	
	if(file==NULL){
			printf("collection doesn't existe\n");
			return "collection doesn't existe." ;		
	}
	
	FILE *fileCopy = fopen("cache.txt","w");
	
	// document is a file data_length how will be splited and docComplet is a copy of document	
	char document[data_length];
	char docComplet[data_length];
	
	while(fgets(document,data_length,file)!=NULL){
		
		// removing '\n' from the end 
		document[strlen(document)-1]='\0';
		
		strcpy(docComplet,document);
		//spliting the document for the first time
		int document_beginning=1;
		char *part = split(document,',',&document_beginning);
		//if(conditions_nbr == conditions_counter) means that all conditions are verified for that document
		int conditions_nbr;
		int conditions_counter=0;
		
		while(part!=NULL){
			
			for(conditions_nbr=0;conditions[conditions_nbr]!=NULL;conditions_nbr++){
				
				if(strcmp(get_field(part),get_field(conditions[conditions_nbr]))==0){
					if(check_condition(part,conditions[conditions_nbr])==0){
						conditions_counter++;
						strcpy(document,docComplet);
					}
				}
			}
			part = split(document,',',&document_beginning);
		}
		
		if(removeOne == 0)	{
			if(conditions_nbr != conditions_counter){
				fputs(docComplet,fileCopy);
				fputs("\n",fileCopy);
			}
		}else{
			if(conditions_nbr != conditions_counter || done == 1 ){
				fputs(docComplet,fileCopy);
				fputs("\n",fileCopy);
			}
			if(conditions_nbr == conditions_counter){
				done = 1;
			}
		}		
		
	}
	
	fclose(file);
	fclose(fileCopy);
	
	return "";
	
	remove(trame.collection);
	rename("cache.txt",trame.collection);
	
}

void splitData(){
	//db.dd.update({n:a},{p:k})
	char *data = calloc(data_length,sizeof(char));
	strcpy(data,trame.data[0]);
	int k = 0;
	int i=1;
	while(data[i]!='\0'){
		trame.data[k] = calloc(data_length,sizeof(char));
		int j= 0;
		while(data[i] != ',' && data[i] != '}' ){
			trame.data[k][j] = data[i];
			i++;j++;
		}
		i++;k++;
	}
}

char *setUpdate (char *document){
		
	int document_counteur=1;
	char *field_name = split(document,',',&document_counteur);
	
	char *inside_doc = calloc(80,sizeof(char)) ;
	inside_doc[0]='{';
	inside_doc[1]='\0';

	
	
	while(field_name!=NULL){
		int i=0;
		while(trame.data[i]!=NULL){
			if(strcmp(get_field(field_name),get_field(trame.data[i]))==0){
				strcat(inside_doc,trame.data[i]);
				strcat(inside_doc,",");
			}else{
				strcat(inside_doc,field_name);
				strcat(inside_doc,",");
			}
			i++;
		}
		field_name = split(document,',',&document_counteur);
	}
	
	int fin = 0;
	if(strlen(inside_doc)<=1){
		fin = 1;
	}
	
	inside_doc[strlen(inside_doc)-1+fin]='}';
	inside_doc[strlen(inside_doc)+fin]='\0';
	
	printf("doc returned :%s\n",inside_doc);
	return inside_doc;
}

char *update(char *collection,char *data[],char* conditions[],char *fields[]){
		
	file=fopen(collection,"r");
	
	int updateOne = 1, done =0;
	
	if(fields[0] == NULL || strcmp(fields[0],"false") == 0 ){
		updateOne = 0;
	}
	
	if(file==NULL){
			printf("collection doesn't existe\n");
			return "collection doesn't existe.";		
	}
	
	FILE *fileCopy = fopen("cache.txt","w");
	
	// document is a file data_length how will be splited and docComplet is a copy of document	
	char document[data_length];
	char docComplet[data_length];
	
	while(fgets(document,data_length,file)!=NULL){
		
		// removing '\n' from the end 
		document[strlen(document)-1]='\0';
		
		strcpy(docComplet,document);
		//spliting the document for the first time
		int document_beginning=1;
		char *part = split(document,',',&document_beginning);
		//if(conditions_nbr == conditions_counter) means that all conditions are verified for that document
		int conditions_nbr;
		int conditions_counter=0;
		
		while(part!=NULL){
			
			for(conditions_nbr=0;conditions[conditions_nbr]!=NULL;conditions_nbr++){
				
				if(strcmp(get_field(part),get_field(conditions[conditions_nbr]))==0){
					if(check_condition(part,conditions[conditions_nbr])==0){
						conditions_counter++;
						strcpy(document,docComplet);
					}
				}
			}
			part = split(document,',',&document_beginning);
		}
		
		if(updateOne == 0)	{
			if(conditions_nbr != conditions_counter){
				fputs(docComplet,fileCopy);
				fputs("\n",fileCopy);
			}else{
				splitData();
				fputs(setUpdate(docComplet),fileCopy);
				fputs("\n",fileCopy);
			}
		}else{
			if(conditions_nbr != conditions_counter || done == 1 ){
				fputs(docComplet,fileCopy);
				fputs("\n",fileCopy);
			}
			if(conditions_nbr == conditions_counter){
				done = 1;
			}
		}		
		
	}
	
	fclose(file);
	fclose(fileCopy);
	
	return "";
	
	remove(trame.collection);
	rename("cache.txt",trame.collection);
	
}

void insert(char *collection,char *document){
	file = fopen(collection,"a");
	
	fputs(document,file);
	fputs("\n",file);
	
	fclose(file);
	
}

void createCollection(char *collection){
	file = fopen(collection,"w");
	fclose(file);
	printf("%s created\n",collection);
	
}

char *use(char *database){
	
	if ( CreateDirectory(trame.collection, NULL) || ERROR_ALREADY_EXISTS == GetLastError() )
	{	
		 char *msg = calloc(data_length,sizeof(char)) ;	 
		 sprintf(msg,"switch to %s",trame.collection);		
		 return msg;
	  	 
	}
	else
	{
	     return "erreur de creation de base de données";
	}
}

char *showCollections(char *db){
	
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (db)) != NULL) {
	  while ((ent = readdir (dir)) != NULL) {
		if(strcmp(ent->d_name,".") ==0 || strcmp(ent->d_name,"..") ==0 ){
			
		}else{
	  		send(client_socket,ent->d_name,data_length,0);
	  	}
	  }
	  closedir (dir);
	} else {
	  return "database error";
	}
	return "";
}

void connexion(){
	WSADATA wsa;
	
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Erreur de chargement de 'startup' : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    if((server_socket = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {	printf("socket erreur"); }
 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons( port );
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
     
    if( bind(server_socket ,(struct sockaddr *)&server_addr , sizeof(server_addr)) == SOCKET_ERROR)
    {	printf("Bind erreur");
        exit(EXIT_FAILURE);
    }

    listen(server_socket , 3);
     
}

void recv_data(){
	int i=0;
	trame.data[i] = calloc(data_length,sizeof(char));
	while(recv(client_socket , trame.data[i] , data_length , 0) > 0 && strcmp(trame.data[i],"end data transmition")!=0 )
	{	printf("data est: %s\n",trame.data[i]);
		i++;
		trame.data[i] = calloc(data_length,sizeof(char));
	}
	trame.data[i] = NULL;
}

void recv_conditions(){
	int i = 0;
	trame.conditions[i] = calloc(data_length,sizeof(char));	
	while(recv(client_socket , trame.conditions[i] , data_length , 0) > 0 && strcmp(trame.conditions[i],"end conditions transmition")!=0 )
	{	printf("condition est: %s\n",trame.conditions[i]);
		i++;
		trame.conditions[i] = calloc(data_length,sizeof(char));
	}
	trame.conditions[i]=NULL;
}

void recv_fields(){
	int i = 0;
	trame.fields[i] = calloc(data_length,sizeof(char));
	while(recv(client_socket , trame.fields[i] , data_length , 0) > 0 && strcmp(trame.fields[i],"end fields transmition")!=0 )
	{	printf("field est: %s\n",trame.fields[i]);
		i++;
		trame.fields[i] = calloc(data_length,sizeof(char));
	}
	trame.fields[i]=NULL;
}

void recv_trame(){
	recv_data();
	recv_conditions();
	recv_fields();
}

void ending_transaction(char *msg){
	send(client_socket,msg,data_length,0);
	send(client_socket,"end results transmition",data_length,0);
}

int main(){
	
	connexion();
	
    int c = sizeof(struct sockaddr_in);
    while( (client_socket = accept(server_socket , (struct sockaddr *)&client_addr, &c)) != INVALID_SOCKET )
    {
        printf("Connection accepted\n");
        
        char *response = calloc(8,sizeof(char));
    
	    while(1){
	    	
	    	printf("recv\n");
	    	memset(response,'\0',8);
			recv(client_socket,response,8,0);
			if(response[0] == '\0'){
				break;
			}
			trame.operation = atoi(response);
			printf("l'operation est -%s-\n",response);
			
			
			memset(trame.collection,'\0',20);
			recv(client_socket,trame.collection,data_length,0);
			printf("la collection est %s\n",trame.collection);

			int i =0;
			recv_trame();
			switch(trame.operation){
				case 0 :  
						createCollection(trame.collection);
						ending_transaction("collection created");
						break;
				case 1 : 
						while(trame.data[i]!= NULL)
						{	insert(trame.collection,trame.data[i]);
							i++;
						}
						ending_transaction("insertion done");
						break;
				case 2 : 
						ending_transaction(find(trame.collection,trame.conditions,trame.fields));
						break;
				case 3 : 
						remoove(trame.collection,trame.conditions,trame.fields);
						ending_transaction("remove done");
						break;
				case 4 : 
						update(trame.collection,trame.data,trame.conditions,trame.fields);
						ending_transaction("update done");
						break;
				case 99 :  
						ending_transaction(showCollections(trame.collection));
						break;
				case 100 :
						ending_transaction(use(trame.collection));
						break;
				default : printf("command erreur\n");
							break;
			}
			
		}
    }
     
    if (client_socket == INVALID_SOCKET)
    {
        printf("Erreur : socket client");
        return 1;
    }
 
    closesocket(server_socket);
    WSACleanup();
     
     getch();
     
    return 0;
}













