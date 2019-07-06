#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int mysliwi;
int kucharze;
int zwierzyna;
int pozywienie;
pthread_mutex_t mutexZwierzyny;
pthread_mutex_t mutexPozywienia;

void *polowanie(void*) 
{
   for (int i=0; i<200; i++)
   {
      if (rand()%6+1 > rand()%6+1)
      {
         zwierzyna++;
      }
      pthread_mutex_lock(&mutexPozywienia);
      if (pozywienie == 0) 
      {
         mysliwi--;
         pthread_mutex_unlock(&mutexPozywienia);
         //printf("Myśliwy opuszcza dolinę...\nwatek mysliwego ---- mysliwi: %d kucharze: %d zwierzyna: %d pozywienie: %d dzien numer %d\n", mysliwi, kucharze, zwierzyna, pozywienie, i);
         break;
      }
      else
         pozywienie--;
      pthread_mutex_unlock(&mutexPozywienia);
      //printf("watek mysliwego ---- mysliwi: %d kucharze: %d zwierzyna: %d pozywienie: %d dzien numer %d\n", mysliwi, kucharze, zwierzyna, pozywienie, i);
      usleep(1000);
   }
   pthread_exit(NULL);
}

void *gotowanie(void*)
{
   for (int i=0; i<200; i++)
   {
      pthread_mutex_lock(&mutexZwierzyny);
      if (zwierzyna>0)
      {
         zwierzyna--;
         pozywienie += rand()%6+1;
      }
      pthread_mutex_unlock(&mutexZwierzyny);
      pthread_mutex_lock(&mutexPozywienia);
      if (pozywienie == 0) 
      {
         kucharze--;
         pthread_mutex_unlock(&mutexPozywienia);
         //printf("Kucharz opuszcza dolinę...\nwatek kucharza  ---- mysliwi: %d kucharze: %d zwierzyna: %d pozywienie: %d dzien numer %d\n", mysliwi, kucharze, zwierzyna, pozywienie, i);
         break;
      }
      else
         pozywienie--;
      pthread_mutex_unlock(&mutexPozywienia);
      //printf("watek kucharza  ---- mysliwi: %d kucharze: %d zwierzyna: %d pozywienie: %d dzien numer %d\n", mysliwi, kucharze, zwierzyna, pozywienie, i);
      usleep(1000);
   }
   pthread_exit(NULL);
}

int main (int argc, char** argv) 
{
   mysliwi = atoi(argv[1]);
   kucharze = atoi(argv[2]);
   zwierzyna = atoi(argv[3]);
   pozywienie = atoi(argv[4]);

   pthread_t mysliwi_w[atoi(argv[1])];
   pthread_t kucharze_w[atoi(argv[2])];  
   
   pthread_mutex_init(&mutexZwierzyny, NULL);
   pthread_mutex_init(&mutexPozywienia, NULL);

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
   pthread_mutex_destroy(&mutexPozywienia);
   pthread_mutex_destroy(&mutexZwierzyny);

   printf("mysliwi: %d\nkucharze: %d\nzwierzyna: %d\npozywienie: %d\n", mysliwi, kucharze, zwierzyna, pozywienie);
   return 0;
}