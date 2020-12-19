#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>


#define PAGE_SIZE 4096
#define pathname "server.c"


void file_read(int semid, void* memry){
	
	int wr = -1;

	
	struct sembuf mybuf[4];

	
	
	semctl(semid, 2, SETVAL, 2);

	
	mybuf[1].sem_num = 2;
	mybuf[1].sem_op = -1;
	mybuf[1].sem_flg = SEM_UNDO;
	semop(semid, mybuf, 1);

	
	mybuf[2].sem_num = 4;
	mybuf[2].sem_op = 1;
	mybuf[2].sem_flg = SEM_UNDO;
	semop(semid, mybuf, 1);
	
	while (1) {
		
		mybuf[0].sem_num = 3;
		mybuf[0].sem_op = -1;
		mybuf[0].sem_flg = 0;					
		semop(semid, mybuf, 1);
		
		//sprintf("6\n");
		//sleep(1);	
		
		if(semctl(semid, 4, GETVAL) != 2)
			break;
		
		
		if((wr = write(1, memry, PAGE_SIZE)) < 0) {
			perror("writing from shmemory");
			exit(-1);
		}	
		
		if(!wr)
			break;
		
		mybuf[0].sem_num = 2;
		mybuf[0].sem_op = 1;
		mybuf[0].sem_flg = 0;					
		semop(semid, mybuf, 1);
		
		//printf("7\n");
		//sleep(1);
	}
	
	return;
}

int main(int argc, char* argv[]) {
	
	int semid = -1, shmid = -1;
	
	if((semid = semget(ftok(pathname, 0), 5, 0666 | IPC_CREAT)) < 0) {
		perror("getting semid");
		return errno;
	}
	
	if((shmid = shmget(ftok(pathname, 0), PAGE_SIZE, 0666 | IPC_CREAT)) < 0) {
		perror("getting shmid");
		return errno;
	}
	
	void* memry = shmat(shmid, NULL, 0);


	file_read(semid, memry);
	
	return 0;
}
