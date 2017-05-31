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

	char readingRow[500];

    FILE *fp;

    int i;
	
    /*
		Opens data savedKVStore.txt for reading
	*/
    fp = fopen("savedKVStore.csv","r");
    if(fp == NULL){
            printf("Dataerror\n");
            return 1;
    }
	
	fscanf(fp,"%s",readingRow);
    Data.size = atoi(strtok(readingRow,";"));
    Data.realSize = atoi(strtok(readingRow,";"));
    Data.delFlag[i] = atoi(strtok(readingRow,";"));
    for(i=0;i<BUF;i++){
		/*
			[WIP] Has to jump over size;realSize;delflag; to key;value
		*/
		for(k=0;k<KV_STRING;k++){
			strcpy(KVStore.key[i][k], strtok(NULL,"w"));
			strcpy(KVStore.value[i][k], strtok(NULL,"w"));
		}
	}
    fclose(fp);

    /*
		Our while loop
    */

    /*
		Opens data savedKVStore.csv for writing
		Alternative: .txt
	*/
    fp = fopen("savedKVStore.csv","w");
    if(fp == NULL){
            printf("Dataerror\n");
            return 1;
    }

    /*  Writes in order:
        size;realSize
		flag1;key1;value1
		flag2;key2;value2
    */
	
	fprintf(fp , "%i;%i\n" , Data.size,Data.realSize);
    for(i=0;i<BUF;i++){
		for(k=0;k<KV_STRING;k++){
            fprintf(fp , "%i;%s;%s/n" , Data.delFlag[i][k],Data.key,Data.value[i][k]);
		}
	}
	
    fclose(fp);
	
    return 0;
}
