/***********************************************/
/*    Titel : Assignment3_shm_semaphore.c      */
/*    Author: Khanjan Desai                    */
/*    Date  : Feb 13, 2016                     */
/*    Shared Memory using Semaphore            */
/*    Compile Using Below function:            */
/*gcc <filename>.c -o <filename> -lrt -pthread */
/*    Run: ./<filename> str1 str2              */
/***********************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>

#define MEM_Size 1024

sem_t mutex;
int main(int argc,char * argv[])
{
	key_t key;
	int shmid;
	char *data;
	int mode;

	int PID;
	void *child_ptr,*parent_ptr;
	// Check if more than two Inputs are given
	if (argc < 2) 
	{
		printf("Error: Cant read enough data\nData Format: execution file <message1> <message2>");
        	exit(1);
	}
	// Create Semaphore mutex
	if(sem_init(&mutex,1,1)==-1)
	{
		perror("Error mutex:");
		exit(1);
	}
	// Get unique PID for program
	if ((key = ftok("Assignment3_shm_semaphore.c", 'R')) == -1) 
	{
	        perror("ftok");
	        exit(1);
	}

	/* connect to (and possibly create) the segment: */
    	if ((shmid = shmget(key, MEM_Size, 0644 | IPC_CREAT)) == -1) 
	{
	        perror("shmget");
	        exit(1);
	}

    	/* attach to the segment to get a pointer to it: */
    	data = shmat(shmid, (void *)0, 0);
    	if (data == (char *)(-1)) 
	{
	        perror("shmat");
	        exit(1);
	}
	// Decrement mutex by 1 if value != 0 else wait till it becomes zero  
	if(sem_wait(&mutex)==-1)
	{
		perror("Error sem_wait:");
		exit(1);
	}

    	/* read or modify the segment, based on the command line: */
    	if (argc == 2) 
	{
		printf("writing to segment: \"%s\"\n", argv[1]);
	        strncpy(data, argv[1], MEM_Size);
    	}
	else
	{
	        printf("segment contains: \"%s\"\n", data);
	}
    	/* detach from the segment: */
    	if (shmdt(data) == -1) 
	{
	        perror("shmdt");
	        exit(1);
	}
	// Incerment mutex by 1 and release semaphore
	if(sem_post(&mutex)==-1)
	{
		perror("Error sem_post:");
		exit(1);
	}

	return(0);
	

}
