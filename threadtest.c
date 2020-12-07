#include "types.h"
#include "params.h"
#include "user.h"
#include "stat.h"


// Threads should print their id, and it may be a mess.
// Then, they should change some shared memory.
lock_t* mutex;
struct shared_mem{
	int index;
	int containers[10];
}

int thread_process(void* arg1,void* arg2){
	int* index = (int*)arg1;
	int* ten = (int*)arg2;
	printf(1,"Thread %d\n",*index);
	lock_acquire(mutex);
	shared->index = *index;
	shared->containers[shared->index] = *(index) + *(ten);
	lock_release(mutex);
}

int main(int argc,char** argv){
	int i, second_arg = 10;
	// Lock
	mutex = (lock*)malloc(sizeof(lock_t));
	lock_init(mutex);

	// Shared mem
	struct shared_mem* shared = (struct shared_mem*)malloc(sizeof(shared_mem));
	shared->index = 0;
	for(i = 0; i < 10; i++){
		shared->containers[i] = 0;
	}

	
	for(i = 0; i < 10; i++){
		thread_create(&thread_process,(void*)&i,(void*)&second_arg);
	}
	for(i = 0; i < 10; i++){
		thread_join();
	}

	printf(1,"Shared mem should be 10 -> 19\n\n");
	for(i = 0; i < 10; i++){
		printf1(1,"Shared mem[%d] = %d\n",i,shared->containers[i]);
	}

	exit();
}
