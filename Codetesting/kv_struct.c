#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

int put(char* key, char* value, char* res);
int get();
int del();
void list();

struct Data{
  char key[BUFFER_SIZE][1];
  char value[BUFFER_SIZE][1];
  int delFlag[BUFFER_SIZE];//1 represents deleted
  int size;//letzter Index im Array
  int realSize;//Menge der Eintraege
};

struct Data KVStore;

int main(){

  char key[BUFFER_SIZE];
  char value[BUFFER_SIZE];
  char res[BUFFER_SIZE];
  KVStore.size=0;
  KVStore.realSize=0;
  int itemNum=0;
  while(1){
    printf("Select 1:Put 2:Get 3:Delete 4:Display 5:Exit\n");
    scanf("%i",&itemNum);
    switch(itemNum){
      case 1:
        printf("Put selected.\n");
        printf("Enter key followed by value\n");
        scanf("%s",key);
        scanf("%s",value);
        put(key, value, res);break;
      case 2:
        printf("Get selected.\n");
        get();break;
      case 3:
        printf("Delete selected.\n");
        del();break;
      case 4:
        printf("Display selected.\n");
        list();break;
      default:
        printf("Program ended.\n");
        return 0;
    }
  }
  return 0;
}

int put(char* key, char* value, char* res){
  int i;
  for(i=0;i<KVStore.size;i++){
    if(strcmp(KVStore.key[i],key)==0){
      strcpy(KVStore.value[i],value);
      printf("Existing entry replaced\n");
      return 1;//success
    }
  }
  for(i=0;i<KVStore.size;i++){
    if(KVStore.delFlag[i]==1){
        strcpy(KVStore.key[i],key);
        strcpy(KVStore.value[i],value);
        KVStore.delFlag[i]=0;
        KVStore.realSize++;
        printf("Deleted entry replaced\n");
        return 1;//success
    }
  }
  printf("Before put: %s\n", KVStore.key[KVStore.size]);
  printf("Before put: %s\n", KVStore.value[KVStore.size]);
  strcpy(KVStore.key[KVStore.size],key);
  strcpy(KVStore.value[KVStore.size],value);
  printf("After put: %s\n", KVStore.key[KVStore.size]);
  printf("After put: %s\n", KVStore.value[KVStore.size]);
  KVStore.size++;
  KVStore.realSize++;
  printf("Put completed\n");

  return 0;
}

int get(){
  return 0;
}

int del(){
  return 0;
}

void list(){
  int i;
  if(KVStore.size==0){
    printf("No elements found\n");
    return;
  }
  printf("KV Store size: %i\n",KVStore.size );
  for(i=0;i<KVStore.size;i++){
    if(KVStore.delFlag[i]!=1){
      printf("Index: %i - Key: %s, Value: %s\n", i, KVStore.key[i], KVStore.value[i]);
    }
  }
  return;
}
