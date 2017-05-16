#include <stdio.h>
#include <string.h>

#define ROW 100
#define COL 2

int x=0;
char value[20];
char key[20];

int inputKey();
char* get(char *key, char *keyval[ROW][COL]);
int put(char *key, char *value, char *keyval[ROW][COL]);
char* del(char *key, char *keyval[ROW][COL]);

int main(){
  int itemNum=0;
  char *kv[ROW][COL];


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
        printf("Value: %s\n", get(key, kv));
        break;
      case 3:
        printf("3.Delete selected.\n");
        inputKey();
        del(key,kv);
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

char* get(char *key, char *keyval[ROW][COL]){
  strcpy(value,"-1");
  for (x=0; x <ROW;x++) {
    if(strcmp(keyval[x][0],key)==0){
      return keyval[x][1];
    }
  }
  return value;
}

int put(char *key, char *value, char *keyval[ROW][COL]){
  int i;
  for (x=0;x<ROW;x++) {
    if(strcmp(keyval[x][0],key)==0){
      strcpy(keyval[x][1],value);
      return 1;
    }
  }
  for(i=0;i<ROW;i++) {
    if(keyval[x][0]==NULL){
      strcpy(keyval[x][0],key);
      strcpy(keyval[x][1],value);
      return 1;
    }
  }
  return -1;
}

char* del(char *key, char *keyval[ROW][COL]){
  strcpy(value,"-1");
  char* tmp;
  strcpy(value,tmp);
  for (x=0;x<ROW;x++) {
    if(strcmp(keyval[x][0],key)==0){
      *keyval[x][0]=NULL;
      *keyval[x][1]=NULL;
      break;
    }
    return tmp;

  }

  return value;
}
