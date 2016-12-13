#include<stdio.h>
#include<string.h>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include "cli.h"

void lire_car(){
	car_cour=transaction[iterateurDeTransaction++];
}

int lire_mot_cle(char mot[]){

	//tableau des mots cles
	char *mots_cles[nb_mots_cles]={"db","insert","ensureIndex","showCollections","createCollection","use","find","remove","update"};

	int i;
	for(i=0;i<nb_mots_cles;i++){
		if(strcmp(mot,mots_cles[i])==0){
			sym_cour.cls = i;
			strcpy(sym_cour.code,mot);
			return 1;
		}
	}
	return 0;
}

int lire_boolean(char mot[]){

	char *booleaan[2]={"true","false"};

	int i;
	for(i=0;i<2;i++){
		if(strcmp(mot,booleaan[i])==0)
        {
			if(i==0)
                sym_cour.cls = TRUE_TOKEN;
            else if (i==1)
                    sym_cour.cls = FALSE_TOKEN;
			strcpy(sym_cour.code,mot);
			return 1;
		}
	}
	return 0;
}

int lire_mc_update(char mot[]){

	char *mcu[2]={"upsert","multi"};

	int i;
	for(i=0;i<2;i++){
		if(strcmp(mot,mcu[i])==0)
        {
			if(i==0)
                sym_cour.cls = UPSERT_TOKEN;
            else if (i==1)
                    sym_cour.cls = MULTI_TOKEN;
			strcpy(sym_cour.code,mot);
			return 1;
		}
	}
	return 0;
}

int lire_num(int signe){

	//la longueur max d'un num est 11
	char num[11];
	//le compteur de la boucle de remplissage du tab num[]
	int i = 1;
	num[0]=car_cour;
	lire_car();

	while( ( car_cour>='0' && car_cour<='9' ) && i<11 ){
		num[i]=car_cour;
		i++;
		lire_car();
	}


	if(i>=11){
		sym_cour.cls=ERREUR_TOKEN;
	}
	else{
		if((car_cour>='A' && car_cour <='Z') || (car_cour>='a' && car_cour<='z')){
			sym_cour.cls=ERREUR_TOKEN;
			//Erreur(1);
		}
		else{
			num[i]='\0';
			int j;
			int numFinal=0;
			int p=1;
			for(j=0;j<i;j++){
				numFinal+=(num[j]-'0')*p;
				p*=10;
			}
			switch(numFinal){
				case 1 :
						sym_cour.cls=UN_TOKEN;
						strcpy(sym_cour.code,num);
						break;
				case -1 :
						sym_cour.cls=MUN_TOKEN;
						strcpy(sym_cour.code,num);
						break;
				case 0 :
						sym_cour.cls=ZERO_TOKEN;
						strcpy(sym_cour.code,num);
						break;
				default :
						sym_cour.cls=NUM_TOKEN;
						strcpy(sym_cour.code,num);
						break;
			}

		}
	}


}

void lire_mots(){

	//la longueur max d'un mot est 20
	char mot[20];

	//on déjà lu un car_cour dans sym_suiv, on le place en tête du mot
	mot[0]=car_cour;
	lire_car();
	int  i=1;

	while(  ( (car_cour>='A' && car_cour <='Z') || (car_cour>='a' && car_cour<='z') || ( car_cour>='0' && car_cour<='9' ) ) && i<20 ){
		mot[i]=car_cour;
		i++;
		lire_car();
	}

	if(i>=20){
		sym_cour.cls = ERREUR_TOKEN;
		//Erreur(2);
	}
	else{
		mot[i]='\0';
		if(lire_boolean(mot) == 1)
        {
			return;
		}
		if(lire_mc_update(mot) == 1)
        {
			return;
		}
		if(lire_mot_cle(mot) == 0)
        {
			sym_cour.cls = KEY_TOKEN;
			strcpy(sym_cour.code,mot);
		}

	}

}

void lire_special(){

	switch(car_cour){
		case '"': 	sym_cour.cls=COTE_TOKEN;
					lire_car();
					break;
		case '-':  	lire_car();
					lire_num(-1);
					break;
		case '.':	sym_cour.cls=POINT_TOKEN;
					lire_car();
					break;
		case ',':	sym_cour.cls=VIR_TOKEN;
					lire_car();
					break;
		case ':':	sym_cour.cls=DP_TOKEN;
					lire_car();
					break;
		case '(':   sym_cour.cls=PO_TOKEN;
					lire_car();
					break;
		case ')':	sym_cour.cls=PF_TOKEN;
					lire_car();
					break;
		case '{':   sym_cour.cls=ACO_TOKEN;
					lire_car();
					break;
		case '}':	sym_cour.cls=ACF_TOKEN;
					lire_car();
					break;
		case '\0': 	sym_cour.cls=FIN_TOKEN;
					break;
		default :   sym_cour.cls=ERREUR_TOKEN;
					break;
	}

}

int lire_clause(){

	char mot[20];

	//on déjà lu un car_cour dans sym_suiv, on le place en tête du mot
	mot[0]=car_cour;
	lire_car();
	int  i=1;

	while( (car_cour>='A' && car_cour <='Z') || (car_cour>='a' && car_cour<='z') ){
		mot[i]=car_cour;
		i++;
		lire_car();
	}

	if(i>=20){
		sym_cour.cls = ERREUR_TOKEN;
	}
	else{
		mot[i]='\0';
		int existe = 0;
		char *indicateur_possible[nb_indicateur]={"$eq","$ne","$lt","$lte","$gt","$gte"};
		int i;
		for(i=0;i<nb_indicateur;i++){
			if(strcmp(mot,indicateur_possible[i])==0){
				sym_cour.cls = i + debut_indicateur;
				existe = 1;
				strcpy(sym_cour.code,mot);
			}
		}
		if(existe == 0)
			sym_cour.cls = CLAUSE_ERR;
	}
}

void sym_suiv(){

	if(car_cour==' ' || car_cour=='\t'){
		lire_car();
		sym_suiv();
	}
	else{
		if( (car_cour>='A' && car_cour <='Z') || (car_cour>='a' && car_cour<='z') ){
			lire_mots();
		}
		else{
			if (car_cour>='0' && car_cour <='9'){
				lire_num(1);
			}
			else{
				if(car_cour == '$'){
					lire_clause();
				}
				else{
					lire_special();
				}
			}
		}
	}
}

void AfficherToken(){
	printf("%s\n",token[sym_cour.cls]);
	getch();
}

void Ouvrir_Fichier(char* f){
	file = fopen(f,"r");
}

int Erreur(Erreurs COD_ERR){
	if(erreur>=1){
		printf("%s\n",err[COD_ERR]);
		erreur=0;
	}
}

void Test_Symbole(CODE_LEX cl, Erreurs COD_ERR){
  	if (sym_cour.cls == cl)
    {
  		//AfficherToken();
	  	sym_suiv();
	}
	else
    {
		Erreur(COD_ERR);
	}
}

void preparing_db_name(char *database){
	db=calloc(20,sizeof(char));
	strcpy(db,database);

	strcpy(trame.collection,dbpath);
	strcat(trame.collection,"/");
	strcat(trame.collection,db);

}

void preparing_collection_name(char *collection){

	if(db == NULL) {
		printf("no database selected\n");
		erreur = 0;
		return;
	}
	else{
		strcpy(trame.collection,dbpath);
		strcat(trame.collection,"/");
		strcat(trame.collection,db);
		strcat(trame.collection,"/");
		strcat(trame.collection,collection);
		strcat(trame.collection,".txt");
	}
}

//insert
void VALUE(){
	switch(sym_cour.cls){
		case KEY_TOKEN : sym_suiv();
						break;
		case NUM_TOKEN :sym_suiv();
						 break;
		case ACO_TOKEN :DOCUMENT();
						break;
		default : 	Erreur(SYNTAX_ERR);
					break;
	}
}

void ATTRIBUTE(){

	switch(sym_cour.cls){
		case KEY_TOKEN : sym_suiv();
						 Test_Symbole(DP_TOKEN,DP_ERR);
						 VALUE();
						 break;
		default :
					Erreur(SYNTAX_ERR);
					break;
	}


}

void DOCUMENT(){

	switch(sym_cour.cls){
		case ACO_TOKEN :
						if(num_sub_doc == 0){
							debut_doc=iterateurDeTransaction-2;
						}
						num_sub_doc++;
						sym_suiv();
						ATTRIBUTE();
						while(sym_cour.cls==VIR_TOKEN){
							sym_suiv();
							ATTRIBUTE();
						}

						num_sub_doc--;
						if(num_sub_doc == 0){
						 	fin_doc = iterateurDeTransaction-1;
						 	int i, k=0;
							memset(trame.data[num_doc],'\0',data_length);
							for(i=debut_doc;i<fin_doc;i++){
								trame.data[num_doc][k]=transaction[i];
								k++;
							}
							//printf("**%s\n",trame.data[num_doc]);
							num_doc++;
						}

						Test_Symbole(ACF_TOKEN,ACF_ERR);

						break;
		default : 	Erreur(OPTION_ERR);
					break;
	}
}

void INSERT(){

	Test_Symbole(PO_TOKEN,PO_ERR);

	num_doc = 0;
	DOCUMENT();

	while(sym_cour.cls==VIR_TOKEN){
		sym_suiv();
		DOCUMENT();
	}

	Test_Symbole(PF_TOKEN,PF_ERR);

	trame.data[num_doc][0] = '\0';
}
//fin insert

//find ,update and remove
void VALUE_FIND(){
	switch(sym_cour.cls){
		case KEY_TOKEN :strcat(trame.conditions[num_cond],sym_cour.code);
						sym_suiv();
						break;
		case NUM_TOKEN :strcat(trame.conditions[num_cond],sym_cour.code);
						sym_suiv();
						 break;
		default : 	Erreur(SYNTAX_ERR);
					break;
	}
}

void CLAUSE(){

	switch(sym_cour.cls){

		case EQ_TOKEN : strcat(trame.conditions[num_cond],":");
						 sym_suiv(); break;
		case NE_TOKEN : 	strcat(trame.conditions[num_cond],"!=");
							sym_suiv(); break;
		case LT_TOKEN : strcat(trame.conditions[num_cond],"<");
							sym_suiv(); break;
		case LTE_TOKEN :strcat(trame.conditions[num_cond],"<=");
						 sym_suiv(); break;
		case GT_TOKEN : strcat(trame.conditions[num_cond],">");
							sym_suiv(); break;
		case GTE_TOKEN : strcat(trame.conditions[num_cond],">=");
							sym_suiv(); break;

		default: Erreur(CLAUSE_ERR) ;break;
	}
	Test_Symbole(DP_TOKEN,DP_ERR);
	VALUE_FIND();

}

void EXPRESSION(){

	debut_cond=iterateurDeTransaction-1;

	Test_Symbole(KEY_TOKEN,KEY_ERR);
	Test_Symbole(DP_TOKEN,DP_ERR);

	 switch(sym_cour.cls){
	 	case KEY_TOKEN : sym_suiv();
		 				fin_cond = iterateurDeTransaction-2;
						break;
		case NUM_TOKEN :sym_suiv();
						fin_cond = iterateurDeTransaction-2;
						 break;
		case ACO_TOKEN :sym_suiv();
						CLAUSE();
						while(sym_cour.cls==VIR_TOKEN){
							sym_suiv();
							CLAUSE();
						}
						Test_Symbole(ACF_TOKEN,ACF_ERR);
						break;
		default : Erreur(SYNTAX_ERR); break;
	 }
}

void BOOLEAAN(){

		debut_field = iterateurDeTransaction-1;
		memset(trame.fields[num_field],'\0',data_length);
		strcpy(trame.fields[num_field],sym_cour.code);

		fin_field = iterateurDeTransaction-2;

		int i, k = strlen(trame.fields[num_field]);
		for(i = debut_field; i < fin_field; i++)
        {
			trame.fields[num_field][k]=transaction[i];
			k++;
		}
		trame.fields[num_field][k] = '\0';
		num_field++;

		switch(sym_cour.cls){
			case TRUE_TOKEN : sym_suiv();
							break;
			case FALSE_TOKEN : sym_suiv();
							break;
			default : Erreur(SYNTAX_ERR);
						break;
		}

}

void FIELD_STATE(){
	
		debut_field=iterateurDeTransaction-1;
		memset(trame.fields[num_field],'\0',data_length);
		strcpy(trame.fields[num_field],sym_cour.code);
		int k;
		
		Test_Symbole(KEY_TOKEN,KEY_ERR);
		while(sym_cour.cls == POINT_TOKEN ){
			sym_suiv();
			Test_Symbole(KEY_TOKEN,KEY_ERR);
		}

		fin_field = iterateurDeTransaction-2;

		int i; 
		k=strlen(trame.fields[num_field]);
		for(i=debut_field;i<fin_field;i++){
			trame.fields[num_field][k]=transaction[i];
			k++;
		}
		//trame.fields[num_field][k] = '\0';
		//num_field++;

		Test_Symbole(DP_TOKEN,DP_ERR);
		switch(sym_cour.cls){
			case UN_TOKEN : trame.fields[num_field][k] = '1';
							k++;
							trame.fields[num_field][k] = '\0';
							num_field++;
							sym_suiv();
							break;
			case ZERO_TOKEN : 
							trame.fields[num_field][k] = '0';
							k++;
							trame.fields[num_field][k] = '\0';
							num_field++;
							sym_suiv();
							break;
			default : Erreur(SYNTAX_ERR);
						break;
		}
}

void PROJECTION(){
	switch(sym_cour.cls){
		case VIR_TOKEN :
						sym_suiv();
						Test_Symbole(ACO_TOKEN,ACO_ERR);
						FIELD_STATE();
						while(sym_cour.cls == VIR_TOKEN){
							sym_suiv();
							FIELD_STATE();
						}
						Test_Symbole(ACF_TOKEN,ACF_ERR);

		case PF_TOKEN : break;

		default : Erreur(SYNTAX_ERR);
				break;

	}
}

void justOne(){
	switch(sym_cour.cls){
		case VIR_TOKEN :
						sym_suiv();
                        BOOLEAAN();
		case PF_TOKEN : break;

		default : Erreur(SYNTAX_ERR);
				break;

	}
}

void optionsUpdate(){
	switch(sym_cour.cls){
		case VIR_TOKEN :
                        sym_suiv();
                        Test_Symbole(ACO_TOKEN,ACO_ERR);
                        switch(sym_cour.cls)
                        {
                            case UPSERT_TOKEN : sym_suiv();
                                                Test_Symbole(DP_TOKEN,DP_ERR);
                                                BOOLEAAN();
                                                switch(sym_cour.cls)
                                                {
                                                    case VIR_TOKEN : sym_suiv();
                                                                     switch(sym_cour.cls)
                                                                     {
                                                                        case MULTI_TOKEN :  sym_suiv();
                                                                                            Test_Symbole(DP_TOKEN,DP_ERR);
                                                                                            BOOLEAAN();
                                                                                            Test_Symbole(ACF_TOKEN,ACF_ERR);
                                                                                            break;
                                                                        default : Erreur(SYNTAX_ERR);
                                                                                  break;
                                                                     }
                                                                     break;
                                                    case ACF_TOKEN :    sym_suiv();
                                                                        break;

                                                    default : Erreur(SYNTAX_ERR);
                                                              break;
                                                }

                                                break;
                            case MULTI_TOKEN : sym_suiv();
                                                Test_Symbole(DP_TOKEN,DP_ERR);
                                                BOOLEAAN();
                                                switch(sym_cour.cls)
                                                {
                                                    case VIR_TOKEN : sym_suiv();
                                                                     switch(sym_cour.cls)
                                                                     {
                                                                        case UPSERT_TOKEN :  sym_suiv();
                                                                                            Test_Symbole(DP_TOKEN,DP_ERR);
                                                                                            BOOLEAAN();
                                                                                            Test_Symbole(ACF_TOKEN,ACF_ERR);
                                                                                            break;
                                                                        default : Erreur(SYNTAX_ERR);
                                                                                  break;
                                                                     }
                                                                     break;
                                                    case ACF_TOKEN :    sym_suiv();
                                                                        break;

                                                    default : Erreur(SYNTAX_ERR);
                                                              break;
                                                }

                                                break;

                             default : Erreur(SYNTAX_ERR);
                                       break;
                        }
                        break;
		case PF_TOKEN : break;

		default : Erreur(SYNTAX_ERR);
				  break;

	}
}

void QUERY(){
		switch(sym_cour.cls){
			case ACO_TOKEN : sym_suiv();
							switch(sym_cour.cls){
								case ACF_TOKEN :
												break;
								case KEY_TOKEN :
							 				do{
							 					memset(trame.conditions[num_cond],'\0',data_length);
							 					strcpy(trame.conditions[num_cond],sym_cour.code);

							 					EXPRESSION();

							 					int i, k=strlen(trame.conditions[num_cond]);
												for(i=debut_cond;i<fin_cond;i++){
													trame.conditions[num_cond][k]=transaction[i];
													k++;
												}
												//printf("** %s\n",trame.conditions[num_cond]);
												num_cond++;

												if(sym_cour.cls!=VIR_TOKEN)
													break;
							 					sym_suiv();
											 }while(sym_cour.cls==KEY_TOKEN);

											break;
								default : Erreur(SYNTAX_ERR);
											break;
							}
							Test_Symbole(ACF_TOKEN,ACF_ERR);
							PROJECTION();
							break;
				case PF_TOKEN : break;
			default : 	Erreur(OPTION_ERR);
						break;

	}
}

void QUERY_REMOVE(){
		switch(sym_cour.cls){
			case ACO_TOKEN : sym_suiv();
							switch(sym_cour.cls){
								case ACF_TOKEN :
												break;
								case KEY_TOKEN :
							 				do{
							 					memset(trame.conditions[num_cond],'\0',data_length);
							 					strcpy(trame.conditions[num_cond],sym_cour.code);

							 					EXPRESSION();

							 					int i, k=strlen(trame.conditions[num_cond]);
												for(i=debut_cond;i<fin_cond;i++){
													trame.conditions[num_cond][k]=transaction[i];
													k++;
												}
												num_cond++;

												if(sym_cour.cls!=VIR_TOKEN)
													break;
							 					sym_suiv();
											 }while(sym_cour.cls==KEY_TOKEN);

											break;
								default : Erreur(SYNTAX_ERR);
											break;
							}
							Test_Symbole(ACF_TOKEN,ACF_ERR);
							justOne();
							break;
				case PF_TOKEN : break;
			default : 	Erreur(OPTION_ERR);
						break;

	}
}

void QUERY_UPDATE(){
		switch(sym_cour.cls){
			case ACO_TOKEN : sym_suiv();
							switch(sym_cour.cls){
								case KEY_TOKEN :
							 				do{
							 					memset(trame.conditions[num_cond],'\0',data_length);
							 					strcpy(trame.conditions[num_cond],sym_cour.code);

							 					EXPRESSION();

							 					int i, k=strlen(trame.conditions[num_cond]);
												for(i=debut_cond;i<fin_cond;i++){
													trame.conditions[num_cond][k]=transaction[i];
													k++;
												}
												num_cond++;

												if(sym_cour.cls!=VIR_TOKEN)
													break;
							 					sym_suiv();
											 }while(sym_cour.cls==KEY_TOKEN);

											break;
								default : Erreur(SYNTAX_ERR);
											break;
							}
							Test_Symbole(ACF_TOKEN,ACF_ERR);
							Test_Symbole(VIR_TOKEN,VIR_ERR);
							DOCUMENT();
							optionsUpdate();
							break;
			default : 	Erreur(OPTION_ERR);
						break;

	}
}


void FIND(){
	Test_Symbole(PO_TOKEN,PO_ERR);
	num_cond = 0;
	num_field = 0;
	QUERY();
	Test_Symbole(PF_TOKEN,PF_ERR);

}

void REMOVE(){
	Test_Symbole(PO_TOKEN,PO_ERR);
	num_cond = 0;
	num_field = 0;
	QUERY_REMOVE();
	Test_Symbole(PF_TOKEN,PF_ERR);
}

void UPDATE(){
	Test_Symbole(PO_TOKEN,PO_ERR);
	num_cond = 0;
	num_field = 0;
	QUERY_UPDATE();
	Test_Symbole(PF_TOKEN,PF_ERR);
}
//fin find ,update and remove

//use ,createCollection and showCollections
void USE(){

	trame.operation = 100;
	sym_suiv();
	Test_Symbole(KEY_TOKEN,KEY_ERR);
	preparing_db_name(sym_cour.code);

}

void CREATECOLLECTION(){

	Test_Symbole(PO_TOKEN,PO_ERR);
	Test_Symbole(COTE_TOKEN,COTE_ERR);
	Test_Symbole(KEY_TOKEN,KEY_ERR);

	//sending the name of the new collection
	preparing_collection_name(sym_cour.code);

	Test_Symbole(COTE_TOKEN,COTE_ERR);
	Test_Symbole(PF_TOKEN,PF_ERR);


}

void SHOWCOLLECTIONS(){

	Test_Symbole(PO_TOKEN,PO_ERR);
	Test_Symbole(PF_TOKEN,PF_ERR);
	
	if(db == NULL) {
		printf("no database selected\n");
		erreur = 0;
		return;
	}else{
		strcpy(trame.collection,dbpath);
		strcat(trame.collection,"/");
		strcat(trame.collection,db);
	}
}
//fin use and crateCollection


void LINE(){

	switch(sym_cour.cls){

		case USE_TOKEN : USE(); break;

		case DB_TOKEN  :
				num_doc = 0;
				num_cond = 0;
				num_field = 0;

				sym_suiv();
				Test_Symbole(POINT_TOKEN,POINT_ERR);

				switch(sym_cour.cls){
					case SHOWCOLLECTIONS_TOKEN :
												trame.operation = 99;
												sym_suiv();
												SHOWCOLLECTIONS();
												break;
					case CREATECOLLECTION_TOKEN :
							trame.operation = 0;
							sym_suiv();
							CREATECOLLECTION();
							break;
					case KEY_TOKEN :
							preparing_collection_name(sym_cour.code);
							sym_suiv();
							Test_Symbole(POINT_TOKEN,POINT_ERR);
							switch(sym_cour.cls){
								case INSERT_TOKEN : trame.operation = 1;
													sym_suiv();
													INSERT();
													break;
								case FIND_TOKEN :trame.operation = 2;
												sym_suiv();
												FIND();
												break;
								case REMOVE_TOKEN :trame.operation = 3;
												sym_suiv();
												REMOVE();
												break;
								case UPDATE_TOKEN :trame.operation = 4;
												sym_suiv();
												UPDATE();
												break;
								default : Erreur(SYNTAX_ERR); break;
							}
							break;
					default : 	Erreur(SYNTAX_ERR);
									break;
				}
				break;
		default : Erreur(SYNTAX_ERR); break;
	}
}


void connexion(){

	WSADATA wsa;

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {	printf("Error de chargement %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    //Create a socket
    if((client_socket = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {	printf("Erreur socket"); }

    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons( port );

    //Connect to remote server
    if (connect(client_socket , (struct sockaddr *)&server_addr , sizeof(server_addr)) < 0)
    {	puts("Error connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected");
}

int main(int argc,char **argv){

	while(argc--){
        if(strcmp(*argv,"-dbpath") == 0 ) {
        	*argv++;
        	dbpath = calloc(data_length,sizeof(char));
        	strcpy(dbpath,*argv);
        	break;
		}
		*argv++;
	}

	connexion();

	while(1){

		erreur=1;
		iterateurDeTransaction=0;
		memset(transaction,'\0',transaction_length);

		printf("\nmongo> ");

		gets(transaction);
		transaction[strlen(transaction)]='\0';

		lire_car();
		sym_suiv();

		LINE();

		if (sym_cour.cls==FIN_TOKEN && erreur==1 ){

			char str[8];
			sprintf(str,"%d",trame.operation);
			send(client_socket , str , 8 , 0);
			send(client_socket , trame.collection , data_length , 0);

			//sending data
			int i;
			for(i=0;i<num_doc;i++){
				send(client_socket , trame.data[i] , data_length , 0);
				//printf("data : %s\n",trame.data[i]);
			}
			send(client_socket , "end data transmition" , data_length , 0);

			//sending conditions
			for(i=0;i<num_cond;i++){
				send(client_socket , trame.conditions[i] , data_length , 0);
				//printf("condition : %s\n",trame.conditions[i]);
			}
			send(client_socket , "end conditions transmition" , data_length , 0);

			//sending fields
			for(i=0;i<num_field;i++){
				send(client_socket , trame.fields[i] , data_length , 0);
				//printf("field : %s\n",trame.fields[i]);
			}
			send(client_socket , "end fields transmition" , data_length , 0);

			//recv results
			char *result = calloc(data_length,sizeof(char));
			while(recv(client_socket , result , data_length , 0) > 0 )
			{
				if(strcmp(result,"end results transmition")==0){
					break;
				}
				printf("%s\n",result);
				result = calloc(data_length,sizeof(char));
			}
		}else{
			num_doc = 0;
			num_cond = 0;
			num_field = 0;
		}


	}

	getch();
	return 0;


}

