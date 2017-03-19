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
	int sh_mem_ptr;
	char *data;
	int mode;

	int PID;
	void *child_ptr,*parent_ptr;
	// Check if more than two Inputs are given
	if (argc < 3) 
	{
		printf("Error: Cant read enough data\nData Format: execution file <message1> <message2>");
        	exit(1);
	}
	
	

	// Error Check for shared memory
	if((sh_mem_ptr = shm_open("Shared_mem",O_CREAT|O_RDWR,0666))<0)
	{
		perror("Error:");
		exit(1);
	}
	ftruncate(sh_mem_ptr,MEM_Size); // Allocate size to memory pointer
	// Init Semaphore
	if(sem_init(&mutex,1,1)==-1)
	{
		perror("Error sem_init:");
	}
	// Check if child is created 
	if((PID =fork())<0)
	{
		perror("Error:");
		exit(1);
	}
	// Go to child if zero
	if(PID == 0)
	{
		// Create Semaphore mutex
		if(sem_wait(&mutex) ==-1)
		{
			perror("Error sem_wait:");
			exit(1);
		}

		// Create map of child to Shared Memory with error check
		if((child_ptr = mmap(0,MEM_Size,PROT_READ|PROT_WRITE,MAP_SHARED,sh_mem_ptr,0))== MAP_FAILED)
		{
			perror("Error:");
		}
		printf("Child reads: %s\n",(char *)child_ptr);		// print the data read by child
		char *str = argv[1];
		strncpy(child_ptr,str,MEM_Size);			// Store data from cmdline argv[1] to Memory
		printf("Child writes: %s\n",str);
	    	
		// Incerment mutex by 1 and release semaphore
		if(sem_post(&mutex) ==-1)
		{
			perror("Error sem_post:");
			exit(1);
		}
	}	
	else
	{
		// Create Semaphore mutex
		if(sem_wait(&mutex)==-1)
		{
			perror("Error sem_wait:");
			exit(1);
		}


		// Create map of parent to shared memory with error check
		if((parent_ptr = mmap(0,MEM_Size,PROT_READ|PROT_WRITE,MAP_SHARED,sh_mem_ptr,0))== MAP_FAILED)
		{
			perror("Error:");
		}
		printf("Parent reads: %s\n",(char *)parent_ptr);	// print data read by parent
		char *str = argv[2];
		strncpy(parent_ptr,str,MEM_Size);			// Store data from cmdline argv[2] to memory
		printf("Patent writes: %s\n",str);

	    	// Incerment mutex by 1 and release semaphore
		if(sem_post(&mutex)==-1)
		{
			perror("Error sem_post:");
			exit(1);
		}


	}
	close(sh_mem_ptr);

	return(0);
	

}
