//#include "readwritestore.h"
#include <stdio.h>
#include <string.h>

FILE *fp;

void saveData(struct Data *sdata){
	fp = fopen("writing.csv","w");
	
	if (fp == NULL) {
		printf("\nNo file found! - Creating File named: ./writing.csv\n");
	}
	fprintf(fp, "%i;\n", sdata->realSize);
	fprintf(fp, "%i;\n", sdata->size);
	
	int i;
	for (i=0; i<sdata->size; i++) {
		fprintf(fp, "%i;%s;%s;\n", sdata->delFlag[i], sdata->key[i], sdata->value[i]);
	}
	printf("\nWrote Dataset to File ./writing.csv\n");
	fclose(fp);
}

void readData(struct Data *sdata){
	char readingRow[BUF];
	fp = fopen("writing.csv","r");
    if (fp == NULL) {
            printf("\nNo reading file found!\n");
            return;
    }else {
		/*Each line seperatly, first realSize, then size, then the dataset*/
		fscanf(fp,"%s",readingRow);
		sdata->realSize= atoi(strtok(readingRow,";"));
		fscanf(fp,"%s",readingRow);
		sdata->size= atoi(strtok(readingRow,";"));
		fscanf(fp,"%s",readingRow);
		int i;
		for (i=0;i<sdata->size;i++) {
			sdata->delFlag[i]= atoi(strtok(readingRow,";"));
			strcpy(sdata->key[i], strtok(NULL,";"));
			strcpy(sdata->value[i], strtok(NULL,";"));
			fscanf(fp,"%s",readingRow);
		}
		printf("\nDataset has been read from ./writing.csv\n");
		fclose(fp);
	}
}