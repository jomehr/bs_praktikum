#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF 1024
#define KV_STRING 1024
#define RES 1024


struct Data{
	char key[BUF];
	char value[BUF];
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
            printf("No file found.\n");
            return 1;
    }
	
	fscanf(fp,"%s",readingRow);
    //Data.size = atoi(strtok(readingRow,";"));
    //Data.realSize = atoi(strtok(readingRow,";"));
    /*
		[WIP] Has to jump over size;realSize to delflag;key;value
	*/
	for(i=0;i<BUF;i++){
			Data.delFlag[i] = atoi(strtok(readingRow,";"));
			strcpy(Data.key[i], strtok(NULL,";"));
			strcpy(Data.value[i], strtok(NULL,";"));
		}
	}
    fclose(fp);

    /*
		Our while loop
    */

    /*
		Opens data savedKVStore.csv for writing
	*/
    fp = fopen("savedKVStore.csv","w");
    if(fp == NULL){
            printf("Fileerror\n");
            return 1;
    }

    /*  Writes in order:
        size;realSize
		flag1;key1;value1
		delflag2;key2;value2
    */
	//fprintf(fp , "%i;%i\n" , Data.size,Data.realSize);
    for(i=0;i<BUF;i++){
            fprintf(fp , "%i;%s;%s\n" , Data.delFlag[i],Data.key[i],Data.value[i]);
	}
    fclose(fp);
	
    return 0;
}
