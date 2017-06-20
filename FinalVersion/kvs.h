#ifndef KVS_H
#define KVS_H

#define BUF 1024
#define KV_STRING 1024

struct Data{
	char key[BUF][KV_STRING];
	char value[BUF][KV_STRING];
	int delFlag[BUF];				
	int size;						
	int realSize;
	int rc;
};

extern int strtoken(char* str, char* separator, char** token, int size);


extern int put(char* key, char* value, char* res, struct Data *shmdata);


extern int get(char* key, char* res, struct Data *shmdata);


extern int del(char* key, char* res, struct Data *shmdata);


extern void list(char* res, struct Data *shmdata);


#endif