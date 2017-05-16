#include <stdio.h>
//#include <time.h>
//#include <stdlib.h>
#include <string.h>
#define ROW 100
#define COL 3
int x=0;
int j=0;
int index_int=1;
char value[20];
char key[20];

int inputKey();
char* get(char *key, char *keyval[ROW][COL]);
int put(char *key, char *value, char *keyval[ROW][COL]);
char* del(char *key, char *keyval[ROW][COL]);

int main(){
  int itemNum=0;
  char *kv[ROW][COL];
  int i=0;
/*
  for (i=0;i<100;i++) {
    strcpy(kv[i][0],"IDX");
    strcpy(kv[i][1],"VLE");
    //kv[i][1]=genrandnum();
  }
*/
  while(1){
    printf("Select 1:Put 2:Get 3:Delete 4:Exit\n");
    scanf("%i",&itemNum);
    switch(itemNum){
      case 1:
        printf("1.Put selected.\n");
        inputKey();
        //printf("Current value %s. Enter new value:", kv[key][1]);
        scanf("%s",value);

        put(key, value, kv);
        printf("Put: key %s, value %s\n",kv[0][1], kv[0][2]);
        //printf("New value is %i at key %i\n",kv[key][1],key);
        break;
      case 2:
        printf("2.Get selected.\n");
        inputKey();
        printf("%s\n", get(key, kv));
        //printf("Zeile %i : %i, %i\n",key+1, kv[key][0], kv[key][1]);
        break;
      case 3:
        printf("3.Delete selected.\n");
        inputKey();
        strcpy(value,"haha");//test
        //strcpy(value, del(*key, kv));
        printf("The value at key %s is now %s\n", key, value);
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
  //strcpy(value,"-1");
  for(x=0;x<ROW;x++){
    if(strcmp(keyval[j][1],key)==0){
      //strcpy(value,keyval[j][1]);
      //return *value;
      return keyval[j][2];
    }
  }
  return value;
}

int put(char *key, char *value, char *keyval[ROW][COL]){
  keyval[j][1]=key;
  keyval[j][2]=value;
  //strcpy(keyval[j][1],&value);
  index_int++;
  return 1;//success
}

char* del(char *key, char *keyval[ROW][COL]){
  strcpy(value,"-1");
  for(x=0;x<ROW;x++){
    if(strcmp(keyval[x][1],key)==0){
      keyval[x][1]=0;
      strcpy(value,"0");
      index_int--;
      return value;
    }
  }
  return value;
}
