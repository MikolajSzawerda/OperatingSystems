#include<lib.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

_PROTOTYPE(int get_pinfo, (int*));
_PROTOTYPE(void set_psched, (int, int));
_PROTOTYPE(void sig_handler, (void));

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
int pid=getpid();
printf("Killed: %d\n", pid);
  kill(pid, SIGKILL);
}

int main(int argc, char** argv){
  int gid = 2;
  int i=0;
  int a=1;
  int b=2;
  int pid=-1;
  printf("OLDING TEST\n");
  printf("Run process with higher pri, \nthen after some time should run and execute process who started second\n");
  for(i=0;i<2;i++){
    pid = fork();
    if(pid == 0){
      break;
    }
  }
  if(pid==0){
      pid = i; 
      set_psched(2,50*i+1);
  	signal(SIGALRM, sig_handler);
  alarm(10);
      printf("Started ID: %d PRI: %d\n", pid, 50*i+1);
     if(i==1){
	while(TRUE)for(i=0;i<10000000;i++)a*=b;	
	} else for(i=0;i<100000000;i++)a*=b;
      printf("Ended ID: %d\n", pid);
     }
     return 0;
}

