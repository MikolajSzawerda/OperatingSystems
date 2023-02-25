
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
  int gid;
  int pri = get_pinfo(&gid);
  printf("Terminate: %d PRI: %d\n", getpid(), pri);
  kill(getpid(), SIGKILL);
}

int main(int argc, char** argv){
  int pid;
  int i=0;
  int j=0;
  int a =1;
  int b =2;
  printf("ROUND-ROBIN TEST\n");
  printf("Run and execute processes in order of creation\n");
  for(i=0;i<6;i++){
    if(!(pid=fork())){
      printf("Started ID: %d\n", i);
      break;
    }
  }
  if(pid==0)
  printf("Ended ID: %d\n", i);
}

