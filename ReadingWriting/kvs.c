#include <stdio.h>
#include <string.h>
#define BUF 1024
#define KV_STRING 1024

struct Data{
	char key[BUF][KV_STRING];
	char value[BUF][KV_STRING];
	int delFlag[BUF];				//1 = deleted
	int size;						//last array index
	int realSize;					//no of entries in array
};
char buf[BUFSIZ];

int strtoken(char* str, char* separator, char** token, int size);
int put(char* key, char* value, char* res, struct Data *shmdata);
int get(char* key, char* res, struct Data *shmdata);
int del(char* key, char* res, struct Data *shmdata);
void list(char* res, struct Data *shmdata);

int strtoken(char* str, char* separator, char** token, int size) {
	int i=0;
	token[0] = strtok(str, separator);
	while(token[i++] && i < size)
	token[i] = strtok(NULL, separator);
	return (i);
}

int put (char* key, char* value, char* res, struct Data *shmdata) {
	int i;
	for (i=0;i<shmdata->size;i++) {
		if (strcmp(shmdata->key[i],key)==0) {
			strcpy(shmdata->value[i],value);
			strcpy(res,"Put successful! Existing entry replaced.");
			return 1;
		}
	}
	for (i=0;i<shmdata->size;i++) {
		if (shmdata->delFlag[i]==1) {
			strcpy(shmdata->key[i],key);
			strcpy(shmdata->value[i],value);
			shmdata->delFlag[i]=0;
			shmdata->realSize++;
			strcpy(res,"Put successful! Deleted entry replaced.");
			return 2;
		}
	}
	strcpy(shmdata->key[shmdata->size],key);
	strcpy(shmdata->value[shmdata->size],value);
	shmdata->size++;
	shmdata->realSize++;
	strcpy(res,"Put successful!");
	return 0;
}

int get (char* key, char* res, struct Data *shmdata) {
	strcpy(res, buf);
	strcat(res,"\n");
	if (shmdata->size == 0) {
		strcpy(res,"Store is empty!");
		return 1;
	}
	int i;
	for (i=0; i<shmdata->size;i++) {
		if (strcmp(shmdata->key[i],key)==0) {
			strcpy(res,shmdata->value[i]);
			return 0;
		}
	}
	strcpy(res,"Key not found!");
	return 1;
}

int del (char* key, char* res, struct Data *shmdata) {
	strcpy(res, "");
	if (shmdata->size == 0) {
		strcpy(res,"Store is empty!");
		return 1;
	}
	int i;
	for (i=0;i<shmdata->size;i++) {
		if (strcmp(shmdata->key[i],key)==0) {
			strcpy(res,shmdata->value[i]);
			shmdata->delFlag[i]=1;
			strcpy(shmdata->key[i],"");
			strcpy(shmdata->value[i],"");
			shmdata->realSize--;
			return 0;
		}
	}
	strcpy(res,"Key not found!");
	return 1;
}

void list (char* res, struct Data *shmdata) {
	strcpy(res, "Size: ");
	//int converts to char array
	snprintf(buf, sizeof(buf), "%d", shmdata->size);
	strcat(res, buf);
	strcat(res,"\n");
	if (shmdata->size==0) {
		printf("Store is empty!");
		return;
	}
	int i;
	for (i=0;i<shmdata->size;i++) {
		if (shmdata->delFlag[i]!=1) {
			printf("Index: %5i\tKey: %-20s\tValue: %s\n", i, shmdata->key[i], shmdata->value[i]);
			strcat(res,shmdata->key[i]);
			strcat(res," | ");
			strcat(res,shmdata->value[i]);
			strcat(res,"\n");
		}
	}
	return;
}
