#include<stdio.h>

int Ps[2][100],realTimeCapacity[2][100], processesNumber = 0, LCM;
void getSystem()
{
    FILE *InputFile;

	InputFile = fopen("sys.txt", "r");
	while(!feof(InputFile))
	{
        fscanf(InputFile, "%d %d", &Ps[0][processesNumber], &Ps[1][processesNumber]);
        processesNumber++;
    }

}
int lcm(int t[], int tSize)
{
    int initialArray[100];
    for(int i=0; i<tSize; i++)
    {
            initialArray[i] = t[i];
    }
    int index,m,x,b = 1;
    while(b == 1)
    {
          b = 0;
          x = t[0];
          m = t[0];
          index = 0;
          for(int i=0 ; i<tSize; i++)
          {
                  if(x != t[i])
                  {
                       b = 1;
                  }
                  if(m > t[i])
                  {
                       m = t[i];
                       index = i;
                  }
          }
          if(b==1)
          {
                t[index] = t[index] + initialArray[index];
          }
    }
    return t[0];
}
int priorProcess()
{
    // this function returns the id of the prior process and returns -1 if no process is ready to execute
    int prior = -1, lessPeriode = LCM;
    for(int i=0; i<processesNumber; i++)
    {
            if(realTimeCapacity[0][i] != 0)
            {
               if(lessPeriode > realTimeCapacity[1][i])
               {
                              prior = i;
                              lessPeriode = realTimeCapacity[1][i];
               }
            }
    }
    return prior;
}


void schedule()
{
     int t,prior;
      ////////////////////////////////////////////intializing real time Capacity/restTime
      for(int i=0; i<processesNumber; i++)
      {
              realTimeCapacity[0][i] = Ps[0][i];
              realTimeCapacity[1][i] = Ps[1][i];
      }

      for(t=0; t<LCM; t++)
      {
               // schedulability necessery condition
              for(int i= 0; i< processesNumber; i++)
              {
                      if(realTimeCapacity[0][i] > realTimeCapacity[1][i])
                      {
                           printf("\nthe system is not schedulable because the task %d cant finish its job in its specified period",i);
                           return;
                      }
              }
              /// fetch if periode finished without Pi finishes its work
              prior = priorProcess();
              if(prior != -1)
              {
                       printf("\n(%d-%d)| t%d",t,t+1, prior);
              }
              else
              {
                  printf("\n(%d-%d)",t,t+1);
              }

              realTimeCapacity[0][prior]--;
              ////////////////////////// update period time
              for(int i= 0; i< processesNumber; i++)
              {
                      realTimeCapacity[1][i]--;
                      if(realTimeCapacity[1][i] == 0)
                      {
                                                realTimeCapacity[0][i] = Ps[0][i];
                                                realTimeCapacity[1][i] = Ps[1][i];
                      }
              }



      }
}
int main()
{
      int x,help[100];
      getSystem();
      //schedulability...
      float sum=0;
      for(int i=0; i< processesNumber; i++)
      {
            sum = sum +  ((float)Ps[0][i]/(float)Ps[1][i]);
      }
      if(sum>1)
      {
               printf("\nthis system isn't schedulable because\nthe sum of Ei/Pi = %f > 1\n",sum);

      }
      else
      {
             /////////////////////////////////////////// getting LCM...
             for(int i=0; i<processesNumber; i++)
             {
                     help[i] = Ps[1][i];
             }
             LCM = lcm(help, processesNumber);
             /////////////////////////////////////////// start scheduling
             schedule();
      }
      scanf("%d",&x);
}

// gcc RateMonot.c -o RMA
