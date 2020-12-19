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


void file_write(char* filename, int semid, void* memry){
	
	struct sembuf mybuf[4];
	
	int fh = -1, rd = -1;

	if((fh = open(filename, O_RDONLY)) < 0) {
		perror("opening file");
		exit(-1);
	}	


	
	semctl(semid, 3, SETVAL, 0);
	
	
	mybuf[2].sem_num = 4;
	mybuf[2].sem_op = 1;
	mybuf[2].sem_flg = SEM_UNDO;
	semop(semid, mybuf, 1);

	
	while(1) {
		
		mybuf[0].sem_num = 2;
		mybuf[0].sem_op = -1;
		mybuf[0].sem_flg = 0;					
		semop(semid, mybuf, 1);
			
	
		if(semctl(semid, 4, GETVAL) != 2){

			break;
		}
		
		if((rd = read(fh, memry,  PAGE_SIZE)) < 0) {
			perror("reading");
			exit(-1);
		}
		
		if(!rd )
			break;
			
		mybuf[0].sem_num = 3;
		mybuf[0].sem_op = 1;
		mybuf[0].sem_flg = 0;					
		semop(semid, mybuf, 1);
		
	}
	
	return;
}



int main(int argc, char* argv[]){
	
	int semid, shmid;
	
	if (argc != 2) {
		perror("Please, make sure, your arguments are correct!\n");
		return errno;
	}

	
	if((semid = semget(ftok(pathname, 0), 5, 0666 | IPC_CREAT)) < 0) {
		perror("getting semid");
		return errno;
	}
	
	if((shmid = shmget(ftok(pathname, 0), PAGE_SIZE, 0666 | IPC_CREAT)) < 0) {
		perror("getting shmid");
		return errno;
	}
	
	
	void* memry = shmat(shmid, NULL, 0);


	file_write(argv[1], semid, memry);
		
	
	return 0;
}
