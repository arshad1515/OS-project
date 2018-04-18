#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>

extern int initialize_bowls(unsigned int bowlcount);
extern void cat_eat(unsigned int bowlnumber);
extern void mouse_eat(unsigned int bowlnumber);
extern void cat_sleep(void);
extern void mouse_sleep(void);

int NumBowls;  
int Number_of_Cats;  
int Number_of_Mice;   
int Number_of_Loops;  

struct semaphore *CatMouseWait;

static void cat_simulation(void * unusedpointer,unsigned long catNumber_of_ber)
{
  int i;
  unsigned int bowl;
  (void) unusedpointer;
  (void) catNumber_of_ber;
for(i=0;i<Number_of_Loops;i++) 
{
  cat_sleep();
  bowl = ((unsigned int)random() % Number_of_Bowls) + 1;
  cat_eat(bowl);
}

  V(CatMouseWait); 
}
	


static void mouse_simulation(void * unusedpointer,unsigned long mouseNumber_of_ber)
{
  int i;
  unsigned int bowl;
  (void) unusedpointer;
  (void) mouseNumber_of_ber;

  for(i=0;i<Number_of_Loops;i++) 
{
  mouse_sleep();
  bowl = ((unsigned int)random() % Number_of_Bowls) + 1;
  mouse_eat(bowl);

}
V(CatMouseWait); 
}

 

int catmouse(int nargs,char ** args)
{
  int index, error;
  int i;
  if (nargs != 5)
 {
    printf("Usage: Number_of__BOWLS Number_of__CATS Number_of__MICE Number_of__LOOPS\n");
    return 1;  
  }

  Number_of_Bowls = atoi(args[1]);
  if (Number_of_Bowls <= 0) 
{
    printf("\t\nCat Mouse: invalid Number_of Bowls:** %d\n",Number_of_Bowls);
    return 1;
}

  Number_of_Cats = atoi(args[2]);
  if (Number_of_Cats < 0) 
{
    printf("\t\nCat Mouse: invalid Number_ of Cats: %d\n",Number_of_Cats);
    return 1;
}

  Number_of_Mice = atoi(args[3]);
  if (Number_of_Mice < 0) 
{
    printf("\tCat Mouse: invalid Number_ of Mice: %d\n",Number_of_Mice);
    return 1;
}

  Number_of_Loops = atoi(args[4]);
  if (Number_of_Loops <= 0) 
{
    printf("catmouse: invalid Number_of Loops: %d\n",Number_of_Loops);
    return 1;
}

  printf("Using %d bowls, %d cats, and %d mice. Looping %d times.\n",Number_of_Bowls,Number_of_Cats,Number_of_Mice,Number_of_Loops);
  CatMouseWait = sem_create("CatMouseWait",0);

  if (CatMouseWait == NULL) 
{
    printf("catmouse: could not create semaphore\n");
}

  if (initialize_bowls(Number_of_Bowls)) 
{
    printf("catmouse: error initializing bowls.\n");
  }

  for (index = 0; index < Number_of_Cats; index++) 
{
    error = thread_fork("cat_simulation thread",NULL,index,cat_simulation,NULL);
    if (error) 
{
      printf("cat_simulation: thread_fork failed: %s\n", strerror(error));
}
  }

  for (index = 0; index < Number_of_Mice; index++) 
{
    error = thread_fork("mouse_simulation thread",NULL,index,mouse_simulation,NULL);
    if (error) 
{
      printf("mouse_simulation: thread_fork failed: %s\n",strerror(error));
}
  }

   
  for(i=0;i<(Number_of_Cats+Number_of_Mice);i++) 
{
    P(CatMouseWait);
 }

  sem_destroy(CatMouseWait);

  return 0;
}