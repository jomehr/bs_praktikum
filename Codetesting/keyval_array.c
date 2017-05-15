#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define ROW 100
#define COL 2
int x=0;
int value=0;
int key=0;

int inputKey();
int genrandnum();
int genrandnumhun();
int get(int key, int keyval[ROW][COL]);
int put(int key, int value, int keyval[ROW][COL]);
int del(int key, int keyval[ROW][COL]);

int main(){
  srand(time(NULL));
  int itemNum=0;
  int kv[ROW][COL];
  int i=0,j=0;
  for (i=0;i<100;i++) {
    kv[i][0]=i;
    kv[i][1]=0;
    //kv[i][1]=genrandnum();
  }
  while(1){
    printf("Select 1:Put 2:Get 3:Delete 4:Exit\n");
    scanf("%i",&itemNum);
    switch(itemNum){
      case 1:
        printf("1.Put selected.\n");
        inputKey();
        printf("Current value %i. Enter new value:", kv[key][1]);
        scanf("%i",&value);
        put(key, value, kv);
        printf("New value is %i at key %i\n",kv[key][1],key);
        break;
      case 2:
        printf("2.Get selected.\n");
        inputKey();
        printf("%i\n", get(key, kv));
        printf("Zeile %i : %i, %i\n",key+1, kv[key][0], kv[key][1]);
        break;
      case 3:
        printf("3.Delete selected.\n");
        inputKey();
        value = del(key, kv);
        printf("The value at key %i is now %i\n", key, value);
        break;
      default:
        printf("Program ended.\n");
        return 0;
    }
  }
}

int inputKey(){
  printf("Enter key between 0 and %i:",ROW-1);
  do{
    scanf("%i",&key);
  }while(key<0||key>=ROW);
  return 0;
}

int genrandnum(){
  return (rand()%1000);
}

int genrandnumhun(){
  return (rand()%100);
}

int get(int key, int keyval[ROW][COL]){
  value=-1;
  for(x=0;x<ROW;x++){
    if(keyval[x][0]==key){
      value=keyval[x][1];
      return value;
    }
  }
  return value;
}

int put(int key, int value, int keyval[ROW][COL]){
  keyval[key][1]=value;
  return 1;//success
}

int del(int key, int keyval[ROW][COL]){
  value=-1;
  for(x=0;x<ROW;x++){
    if(keyval[x][0]==key){
      keyval[x][0]=0;
      value=0;
      return value;
    }
  }
  return value;
}
