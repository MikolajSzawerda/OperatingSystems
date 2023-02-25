## Operating Systems

Set of projects that I had to implement during Operating Systems course at Warsaw University of Technology.

Languages: C, CPP
SYS: Minix 2.0, Linux

## Content

### Kernel compilation and system calls

During the exercise, add a new syscall to the Minix 2.0 system that returns the process number (index) in the process table.

### Process scheduling

The aim of the exercise is to design a scheduling mechanism in the system MINIX.
Implement a process scheduling algorithm for the following problem. We have three new process groups: 1, 2, 3. The relationship between the priorities of the processes in each group is priority(1) > priority(2) > priority(3). Within each group, processes are prioritized as follows. Group 1: round-robin, group 2: aging (waiting process priority increases over time), group 3: shortest job first.

Develop at least four test scenarios that demonstrate that the scheduling algorithm performs as intended.

### Synchronization of processes using semaphores

You must write a C program in a Linux environment
solving the problem set by the instructor. The problem is
associated with the organization of N communication buffers between which they are
additional constraints imposed.

We have 4 types of producers who produce: dough, meat, cheese and cabbage, then put the produced product into the appropriate buffer. Consumers create dumplings of 3 types: with meat, with cheese, with cabbage. The condition for making a dumpling is that the buffers contain one portion of the dough and one portion of the filling at the same time. The number of producer and consumer processes/threads should be a program parameter.

### Synchronization of processes using monitors

You must write a C++ program in a Linux executing environment.

We have 4 types of producers who produce: dough, meat, cheese and cabbage, then put the produced product into the appropriate buffer. Consumers create dumplings of 3 types: with meat, with cheese, with cabbage. The condition for making a dumpling is that the buffers contain one portion of the dough and one portion of the filling at the same time. The number of producer and consumer processes/threads should be a program parameter.

### Memory management

In this exercise, you will change the default memory allocation algorithm in
minix system. It should be possible to select the block selection algorithm from the list
free blocks between the standard first fit and the so-called worst algorithm fit.

### Filesystems

(Implemented using inodes)

In a file on disk, a single-level file system should be organized catalog.
A directory element is a file description containing at least name, size and arrangement of the file on the virtual disk.
Belongs implement the following operations available to the user of the program:
- creating a virtual disk,

- copying a file from a Minix disk to a virtual disk,

- copying a file from a virtual disk to a Minix disk,

- displaying the virtual disk directory,

- deleting a file from a virtual disk,

- deleting a virtual disk,

- displaying a summary with the current map of the virtual disk occupancy -
i.e. lists of successive areas of the virtual disk with the description: address, type
area, size, state (e.g. for data blocks: free/busy).

