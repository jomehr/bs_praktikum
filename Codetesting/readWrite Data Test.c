#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF 1024
#define KV_STRING 1024
#define RES 1024

struct Data{
	char key[BUF][KV_STRING];
	char value[BUF][KV_STRING];
	int delFlag[BUF];
	int size;
	int realSize;
};

struct Data *KVStore;

int main()
{
    char key[BUF];
	char value[BUF];
	Data->size=0;
	Data->realSize=0;

	char zeileEinlesen[500];

    FILE *fp;

    int i;

    fp = fopen("savedData.txt","w");
    if(fp == NULL){
            printf("Dateifehler\n");
            return 1;
    }

    for(i=0;i<BUF;i++){
        fscanf(fp,"%s",zeileEinlesen);
        /*
            atoi -> char number to int
        */
        Data[i].size = atoi(strtok(zeileEinlesen,";"));
        Data[i].realSize = atoi(strtok(zeileEinlesen,";"));
        Data[i].delFlag[i] = atoi(strtok(zeileEinlesen,";"));
        strcpy(KVStore[i].key, strtok(NULL,"w"));
        strcpy(KVStore[i].value, strtok(NULL,"w"));
    }
    fclose(fp);

    /*
    Hier unsere While-Schleifen
    */

    /*öffnet die datei savedData.txt zum beschreiben*/
    fp = fopen("savedData.txt","w");
    if(fp == NULL){
            printf("Dateifehler\n");
            return 1;
    }

    /*  schreibt im Stil
        size;realSize;
        flag;key;value
    */
    for(i=0;i<BUF;i++){
            fprintf(fp , "%i;%i;/n" , Data[i].size , Data[i].realSize);
            fprintf(fp , "%i;%s;%s/n" , Data[i].delFlag[i] , Data[i].key,Data[i].value);
    }
    fclose(fp);
    return 0;
}
