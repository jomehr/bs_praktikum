//KEY-VALUE STORE mit einer Strukt-Datenstruktur
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define KV_STRING 50

int put(char* key, char* value, char* res);
int get(char* key, char* res);
int del(char* key, char* res);
void list(char* res);

struct Data{
  char key[BUFFER_SIZE][KV_STRING];
  char value[BUFFER_SIZE][KV_STRING];
  int delFlag[BUFFER_SIZE];           //1 = geloescht
  int size;                           //letzter Index vom Array
  int realSize;                       //Menge der Eintraege im Array
};
//Hier wird nur die Variable KVStore um Funktionalitaet erweitert
//und nicht das struct selbst!
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
        put(key, value, res);
        //res hat 'Put successful![...]'
        printf("Result: %s\n", res);
        break;
      case 2:
        printf("Get selected.\n");
        printf("Enter key\n");
        scanf("%s",key);
        get(key,res);
        //res hat KVStore Value oder 'Key not found!'
        printf("Result: %s\n", res);
        break;
      case 3:
        printf("Delete selected.\n");
        printf("Enter key\n");
        scanf("%s",key);
        del(key,res);
        //res hat KVStore Value oder 'Key not found!'
        printf("Result: %s\n", res);
        break;
      case 4:
        printf("Display selected.\n");
        list(res);
        //res hat KVStore Groesse
        printf("Result(size): %s\n", res);
        break;
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
      strcpy(res,"Put successful! Existing entry replaced.");
      return 1;
    }
  }
  for(i=0;i<KVStore.size;i++){
    if(KVStore.delFlag[i]==1){
        strcpy(KVStore.key[i],key);
        strcpy(KVStore.value[i],value);
        KVStore.delFlag[i]=0;
        KVStore.realSize++;
        strcpy(res,"Put successful! Deleted entry replaced.");
        return 2;
    }
  }
  strcpy(KVStore.key[KVStore.size],key);
  strcpy(KVStore.value[KVStore.size],value);
  KVStore.size++;
  KVStore.realSize++;
  strcpy(res,"Put successful!");
  return 0;
}

int get(char* key, char* res){
  strcpy(res, "");
  int i;
  for (i=0; i<KVStore.size;i++) {
    if(strcmp(KVStore.key[i],key)==0){
      strcpy(res,KVStore.value[i]);
      return 0;
    }
  }
  strcpy(res,"Key not found!");
  return 1;
}

int del(char* key, char* res){
  strcpy(res, "");
  int i;
  for(i=0;i<KVStore.size;i++){
    if(strcmp(KVStore.key[i],key)==0){
      strcpy(res,KVStore.value[i]);
      KVStore.delFlag[i]=1;
      strcpy(KVStore.key[i],"");
      strcpy(KVStore.value[i],"");
      KVStore.realSize--;
      return 0;
    }
  }
  strcpy(res,"Key not found!");
  return 1;
}

void list(char* res){
  strcpy(res, "");
  char buf[BUFSIZ];
  snprintf(buf, sizeof(buf), "%d", KVStore.size);//int wird zum char array konvertiert
  strcpy(res, buf);
  if(KVStore.size==0){
    printf("No element found!");
    return;
  }
  int i;
  for(i=0;i<KVStore.size;i++){
    if(KVStore.delFlag[i]!=1){
      printf("Index : %i - Key : %s ; Value : %s ;\n", i, KVStore.key[i], KVStore.value[i]);
    }
  }
  return;
}
