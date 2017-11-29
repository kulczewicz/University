#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

int mysliwi;
int kucharze;
int zwierzyna;
int pozywienie;
pthread_mutex_t semaforZwierzyny;
pthread_mutex_t semaforPozywienia;

void *polowanie(void*) 
{
   for (int i=0; i<200; i++)
   {
      if (rand()%6+1 > rand()%6+1)
      {
         zwierzyna++;
      }
      pthread_mutex_lock(&semaforPozywienia);
      if (pozywienie == 0) 
      {
         mysliwi--;
         pthread_mutex_unlock(&semaforPozywienia);
         break;
      }
      else
         pozywienie--;
      pthread_mutex_unlock(&semaforPozywienia);
      printf("watek mysliwego ---- mysliwi: %d kucharze: %d zwierzyna: %d pozywienie: %d obieg petli: %d\n", mysliwi, kucharze, zwierzyna, pozywienie, i);
      usleep(100000);
   }
   pthread_exit(NULL);
}

void *gotowanie(void*)
{
   for (int i=0; i<200; i++)
   {
      pthread_mutex_lock(&semaforZwierzyny);
      if (zwierzyna>0)
      {
         zwierzyna--;
         pozywienie += rand()%6+1;
      }
      pthread_mutex_unlock(&semaforZwierzyny);
      pthread_mutex_lock(&semaforPozywienia);
      if (pozywienie == 0) 
      {
         kucharze--;
         pthread_mutex_unlock(&semaforPozywienia);
         break;
      }
      else
         pozywienie--;
      pthread_mutex_unlock(&semaforPozywienia);
      printf("watek kucharza ----- mysliwi: %d kucharze: %d zwierzyna: %d pozywienie: %d  obieg petli: %d\n", mysliwi, kucharze, zwierzyna, pozywienie, i);
      usleep(100000);
   }
   pthread_exit(NULL);
}

int main (int argc, char** argv) 
{
   printf("argv[0]: %d\n", atoi(argv[1]));
   printf("argv[1]: %d\n", atoi(argv[2]));
   printf("argv[2]: %d\n", atoi(argv[3]));
   printf("argv[3]: %d\n", atoi(argv[4]));
   mysliwi = atoi(argv[1]);
   kucharze = atoi(argv[2]);
   zwierzyna = atoi(argv[3]);
   pozywienie = atoi(argv[4]);

   pthread_t mysliwi_w[atoi(argv[1])];
   pthread_t kucharze_w[atoi(argv[2])];  
   
   pthread_mutex_init(&semaforZwierzyny, NULL);
   pthread_mutex_init(&semaforPozywienia, NULL);

   for (int i=0; i<atoi(argv[1]); i++)
   {
      pthread_create(&mysliwi_w[i], NULL, polowanie, NULL);
   }
   for (int i=0; i<atoi(argv[2]); i++)
   {
      pthread_create(&kucharze_w[i], NULL, gotowanie, NULL);
   }
   for (int i=0; i<atoi(argv[1]); i++)
   {
      pthread_join(mysliwi_w[i], NULL);
   }
   for (int i=0; i<atoi(argv[2]); i++)
   {
      pthread_join(kucharze_w[i], NULL);
   }
   pthread_mutex_destroy(&semaforPozywienia);
   pthread_mutex_destroy(&semaforZwierzyny);

   printf("mysliwi: %d\nkucharze: %d\nzwierzyna: %d\npozywienie: %d\n", mysliwi, kucharze, zwierzyna, pozywienie);
   return 0;
}