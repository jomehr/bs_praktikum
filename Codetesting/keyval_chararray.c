#include <stdio.h>
#include <string.h>

#define ROW 100
#define COL 2

char value[20];
char key[20];
char res[20];

int inputKey();
char* get(char* key, char* keyval[ROW][COL], char* res);
int put(char* key, char* value, char* keyval[ROW][COL]);
char* del(char* key, char* keyval[ROW][COL], char* res);

int main(){
  int itemNum=0;
  char* kv[ROW][COL];

  while(1){
    printf("Select 1:Put 2:Get 3:Delete 4:Exit\n");
    scanf("%i",&itemNum);
    switch(itemNum){
      case 1:
        printf("1.Put selected.\n");
        inputKey();
        printf("Enter value: ");
        scanf("%s",value);
        put(key, value, kv);
        printf("Put: key %s, value %s\n",kv[0][0], kv[0][1]);
        break;
      case 2:
        printf("2.Get selected.\n");
        inputKey();
        printf("Value: %s\n", get(key, kv, res));
        break;
      case 3:
        printf("3.Delete selected.\n");
        inputKey();
        del(key, kv, res);
        break;
      default:
        printf("Program ended.\n");
        return 0;
    }
  }
}

int inputKey(){
  printf("Enter key: ");
  scanf("%s",key);
  return 0;
}

char* get(char* key, char* keyval[ROW][COL], char* res){
  strcpy(res,"-1");
  int i;
  for (i=0;i<ROW;i++) {
    if(strcmp(keyval[i][0],key)==0){
      strcpy(res,keyval[i][1]);
      return res;
    }
  }
  return res;
}

int put(char* key, char* value, char* keyval[ROW][COL]){
  int i;
  for (i=0;i<ROW;i++) {
    if(strcmp(keyval[i][0],key)==0){
      strcpy(keyval[i][1],value);
      return 0;
    }
  }
  for(i=0;i<ROW;i++) {
    if(keyval[i][0]==NULL){
      strcpy(keyval[i][0],key);
      strcpy(keyval[i][1],value);
      return 0;
    }
  }
  return -1;
}

char* del(char* key, char* keyval[ROW][COL], char* res){
  strcpy(res,"-1");
  int i;
  for (i=0;i<ROW;i++) {
    if(strcmp(keyval[i][0],key)==0){
      strcpy(res,keyval[i][0]);
      *keyval[i][0]=NULL;
      *keyval[i][1]=NULL;
      break;
    }
  }
  return res;
}
10:30am
#include <stdio.h>
#include <string.h>

#define ROW 100
#define COL 2

int inputKey(char* key);
char* get(char* key, char* keyval[ROW][COL], char* res);
int put(char* key, char* value, char* keyval[ROW][COL]);
char* del(char* key, char* keyval[ROW][COL], char* res);

int main(){
  char value[20];
  char key[20];
  char res[20];
  int itemNum=0;//switch menu
  char* kv[ROW][COL]={NULL};
  int i;
  //fill 2d array
  for(i=0;i<ROW;i++){
    kv[i][0]="key";
    kv[i][1]="value";
    /*if(i>ROW-10){
      kv[i][0]=NULL;
      kv[i][1]=NULL;
    }*/
    printf("kv now has %s and %s \n", kv[i][0],kv[i][1] );
  }

  while(1){
    printf("Select 1:Put 2:Get 3:Delete 4:Exit\n");
    scanf("%i",&itemNum);
    switch(itemNum){
      case 1:
        printf("1.Put selected.\n");
        inputKey(key);
        printf("Enter value: ");
        scanf("%s",value);
        put(key, value, kv);
        break;
      case 2:
        printf("2.Get selected.\n");
        inputKey(key);
        printf("Value: %s\n", get(key, kv, res));
        break;
      case 3:
        printf("3.Delete selected.\n");
        inputKey(key);
        del(key, kv, res);
        break;
      default:
        printf("Program ended.\n");
        return 0;
    }
  }
}

int inputKey(char* key){
  printf("Enter key: ");
  scanf("%s",key);
  return 0;
}

char* get(char* key, char* keyval[ROW][COL], char* res){
  strcpy(res,"-1");
  int i;
  for (i=0;i<ROW;i++) {
    if(strcmp(keyval[i][0],key)==0){
      strcpy(res,keyval[i][1]);
      return res;
    }
  }
  return res;
}

int put(char* key, char* value, char* keyval[ROW][COL]){
  int i;
  /*for (i=0;i<ROW;i++) {
    if(strcmp(keyval[i][0],key)==0){
      strcpy(keyval[i][1],value);
      return 0;
    }
  }*/
  for(i=0;i<ROW;i++) {
    if(keyval[i][0]==NULL){
      printf("%s found at %i\n",keyval[i][0],i);
      strcpy(keyval[i][0],key);
      printf("Yes1\n");
      strcpy(keyval[i][1],value);
      return 0;
    }
  }
  return -1;
}

char* del(char* key, char* keyval[ROW][COL], char* res){
  strcpy(res,"-1");
  int i;
  for (i=0;i<ROW;i++) {
    if(strcmp(keyval[i][0],key)==0){
      strcpy(res,keyval[i][0]);
      keyval[i][0]=NULL;
      keyval[i][1]=NULL;
      break;
    }
  }
  return res;
}
