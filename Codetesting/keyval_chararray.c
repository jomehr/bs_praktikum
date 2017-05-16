#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#define ROW 100
#define COL 2
int x=0;
int j=0;
char value[20];
char key[20];

int inputKey();
int genrandnum();
int genrandnumhun();
char get(char key, char *keyval[ROW][COL]);
int put(char key, char value, char *keyval[ROW][COL]);
char del(char key, char *keyval[ROW][COL]);

int main(){
  srand(time(NULL));
  int itemNum=0;
  char *kv[ROW][COL];
  int i=0;

  for (i=0;i<100;i++) {
    kv[i][0]="IDX";
    kv[i][1]="VLE";
    //kv[i][1]=genrandnum();
  }

  while(1){
    printf("Select 1:Put 2:Get 3:Delete 4:Exit\n");
    scanf("%i",&itemNum);
    switch(itemNum){
      case 1:
        printf("1.Put selected.\n");
        inputKey();
        //printf("Current value %s. Enter new value:", kv[key][1]);
        scanf("%s",value);

        put(*key, *value, kv);
        //printf("New value is %i at key %i\n",kv[key][1],key);
        break;
      case 2:
        printf("2.Get selected.\n");
        inputKey();
        printf("%i\n", get(*key, kv));
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

int genrandnum(){
  return (rand()%1000);
}

int genrandnumhun(){
  return (rand()%100);
}

char get(char key, char *keyval[ROW][COL]){
  //strcpy(value,"-1");
  //for(x=0;x<ROW;x++){
    if(keyval[j][0]==&key){
      //strcpy(value,keyval[j][1]);
      //return *value;
      return *keyval[j][1];
    }
  //}
  return *value;
}

int put(char key, char value, char *keyval[ROW][COL]){
  keyval[j][0]=&key;
  keyval[j][1]=&value;
  //strcpy(keyval[j][1],&value);
  //j++;
  return 1;//success
}

char del(char key, char *keyval[ROW][COL]){
  strcpy(value,"-1");
  for(x=0;x<ROW;x++){
    if(strcmp(keyval[x][0],&key)==0){
      keyval[x][0]=0;
      strcpy(value,"0");
      return *value;
    }
  }
  return *value;
}
