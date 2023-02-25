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
  int a =1;
  int b =2;
  printf("SCHEDULING BETWEEN GROUPS TEST\n");
  printf("Run three processes with GID in {3, 2, 1}\nand execution should be unconditionaly (1, 2, 3)\n");
  for(i=3;i>0;i--){
    if((pid=fork())==0) break;
  }
  if(pid==0){
    pid = 3-i;
    printf("Started: %d GID: %d\n", pid, i);
    if(i>1) set_psched(i, 1);
    for(i=0;i<1000000;i++) a*=b;
    printf("Ended: %d\n", pid);
    return 0;
  }
  return 0;
}

