#include<lib.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

_PROTOTYPE(int get_pinfo, (int*));
_PROTOTYPE(void set_psched, (int, int));
_PROTOTYPE(void sig_handler, (void));
_PROTOTYPE(void do_interactive, (void));
_PROTOTYPE(void do_batch, (void));

int get_pinfo(int* gid){
  message m;
  int r;
  r = _syscall(MM, GETPPRI, &m);
  *gid = _syscall(MM, GETPGID, &m);
  return r;
}

void set_psched(int gid, int pri){
message m;
m.m1_i2 = pri;
m.m1_i1 = gid;
_syscall(MM, SETPSCHED, &m);
}

void sig_handler(){
int gid;
int pri = get_pinfo(&gid);
printf("Killed: %d PRI: %d\n", getpid(), pri);
  kill(getpid(), SIGKILL);
}


void do_interactive(){
  int i;
  for(i=0;i<4;i++){
    printf("Interactive running\n");
    sleep(1);
  }
}

void do_batch(){
int i=0;
  int j=0;
  int a=1;
  int b=2;
for(j=0;j<1000;j++){
for(i=0;i<1000000;i++) a*=b;
}
}

int main(int argc, char** argv){
  int gid = 2;
  int i=0;
  int a=1;
  int b=2;
  int pid=-1;
  int pid2;
  printf("SHORTEST-JOB-FIRST TEST\n");
  printf("Run interactive and batch processes and interactive process should end first\n");
  for(i=0;i<2;i++){
    if((pid=fork())==0){
      printf("Started ID: %d\n", i);
      set_psched(3,1);
      if(i) do_interactive();
      else do_batch();
      printf("Ended ID: %d\n", i);
      break;
    }
  }
  return 0;
}

