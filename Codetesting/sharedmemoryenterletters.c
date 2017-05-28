#include <sys/ipc.h>
#include <sys/shm.h>
#define MAXSM 30
int main(int argc, char **argv)
{
int shID;
char *myPtr;
int i;
/* Shared Memory erzeugen */
    shID = shmget(2404, MAXSM, IPC_CREAT | 0666);
    if (shID >= 0) {
        /* nun holen wir den Speicher */
        myPtr = shmat(shID, 0, 0);
        if (myPtr==(char *)-1) {
            perror("shmat");
        } else {
            /* Speicher ist zugreifbar: füllen! */
            for (i=0; i<MAXSM; i++) {
                myPtr[i] = 'A'+i;
            }
            getchar(); /* Warte mal auf eine Taste */
            /* gebe den Speicher auf */
            shmdt(myPtr);
        }
    } else { /* shmget lief schief */
        perror("shmget");
    }
}