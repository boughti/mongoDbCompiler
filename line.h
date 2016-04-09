#include<stdio.h>
#include<string.h>
#include<Winsock2.h>
#include<ws2tcpip.h>

#define IPPROT_TCP 6
#define port 8888
#define nb_mots_cles 9
#define nb_indicateur 6
#define debut_indicateur 27
#define transaction_length 2048
#define data_length 81

typedef struct{
	int  operation;
	char collection[data_length];
	char data[100][data_length];
	char conditions[100][data_length];
	char fields[100][data_length];
}TRAME;

TRAME trame;
SOCKET client_socket;
struct sockaddr_in server_addr;

char car_cour;
FILE *file;
char * db;
char *dbpath = "db/data";

char transaction[transaction_length];
int iterateurDeTransaction=0;
int erreur = 1;

int num_sub_doc=0;
int num_doc=0;
int debut_doc,fin_doc;

int num_cond = 0;
int debut_cond,fin_cond;

int num_field=0;
int debut_field,fin_field;


typedef enum {
	DB_TOKEN,INSERT_TOKEN,ENSUREINDEX_TOKEN,SHOWCOLLECTIONS_TOKEN,CREATECOLLECTION_TOKEN,USE_TOKEN,FIND_TOKEN,REMOVE_TOKEN,UPDATE_TOKEN,UPSERT_TOKEN,MULTI_TOKEN,PO_TOKEN,ACO_TOKEN,KEY_TOKEN,POINT_TOKEN,DP_TOKEN,NUM_TOKEN,VIR_TOKEN,ACF_TOKEN,PF_TOKEN,COTE_TOKEN,DOLLAR_TOKEN,UN_TOKEN,MUN_TOKEN,ZERO_TOKEN,ERREUR_TOKEN,FIN_TOKEN,EQ_TOKEN,NE_TOKEN,LT_TOKEN,LTE_TOKEN,GT_TOKEN,GTE_TOKEN,TRUE_TOKEN,FALSE_TOKEN
} CODE_LEX;

char* token[]= {
	"DB_TOKEN","INSERT_TOKEN","ENSUREINDEX_TOKEN","SHOWCOLLECTIONS_TOKEN","CREATECOLLECTION_TOKEN","USE_TOKEN","FIND_TOKEN","REMOVE_TOKEN","UPDATE_TOKEN","UPSERT_TOKEN","MULTI_TOKEN","PO_TOKEN","ACO_TOKEN","KEY_TOKEN","POINT_TOKEN","DP_TOKEN","NUM_TOKEN","VIR_TOKEN","ACF_TOKEN","PF_TOKEN","COTE_TOKEN","DOLLAR_TOKEN","UN_TOKEN","MUN_TOKEN","ZERO_TOKEN","ERREUR_TOKEN","FIN_TOKEN","EQ_TOKEN","NE_TOKEN","LT_TOKEN","LTE_TOKEN","GT_TOKEN","GTE_TOKEN","TRUE_TOKEN","FALSE_TOKEN"
};

typedef enum {
	DB_ERR,INSERT_ERR,ENSUREINDEX_ERR,SHOWCOLLECTIONS_ERR,CREATECOLLECTION_ERR,USE_ERR,FIND_ERR,REMOVE_ERR,UPDATE_ERR,UPSERT_ERR,MULTI_ERR,PO_ERR,ACO_ERR,KEY_ERR,POINT_ERR,DP_ERR,NUM_ERR,VIR_ERR,ACF_ERR,PF_ERR,COTE_ERR,DOLLAR_ERR,UN_ERR,MUN_ERR,ZERO_ERR,OPTION_ERR,SYNTAX_ERR,EQ_ERR,NE_ERR,LT_ERR,LTE_ERR,GT_ERR,GTE_ERR,CLAUSE_ERR
} Erreurs;

char* err[]= {
	"DB_ERR","INSERT_ERR","ENSUREINDEX_ERR","SHOWCOLLECTIONS_ERR","CREATECOLLECTION_ERR","USE_ERR","FIND_ERR","REMOVE_ERR","UPDATE_ERR","UPSERT_ERR","MULTI_ERR","PO_ERR","ACO_ERR","KEY_ERR","POINT_ERR","DP_ERR","NUM_ERR","VIR_ERR","ACF_ERR","PF_ERR","COTE_ERR","DOLLAR_ERR","UN_ERR","MUN_ERR","ZERO_ERR","OPTION_ERR","SYNTAX_ERR","EQ_ERR","NE_ERR","LT_ERR","LTE_ERR","GT_ERR","GTE_ERR","CLAUSE_ERR"
};

typedef struct {
	char code[20];
	CODE_LEX cls;
}TSym_cour;
TSym_cour sym_cour;

void preparing_collection_name(char *collection);

void DOCUMENT();
void SHOWCOLLECTIONS();


