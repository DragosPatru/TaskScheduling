 #define _GNU_SOURCE
 #include <unistd.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <linux/unistd.h>
 #include <linux/kernel.h>
 #include <linux/types.h>
 #include <sys/syscall.h>
 #include <pthread.h>

 #define gettid() syscall(__NR_gettid)

 #define SCHED_DEADLINE	6

 /* XXX use the proper syscall numbers */
 #ifdef __x86_64__
 #define __NR_sched_setattr		314
 #define __NR_sched_getattr		315
 #endif

 #ifdef __i386__
 #define __NR_sched_setattr		351
 #define __NR_sched_getattr		352
 #endif

 #ifdef __arm__
 #define __NR_sched_setattr		380
 #define __NR_sched_getattr		381
 #endif

 static volatile int done;

 struct sched_attr {
	__u32 size;

	__u32 sched_policy;
	__u64 sched_flags;

	/* SCHED_NORMAL, SCHED_BATCH */
	__s32 sched_nice;

	/* SCHED_FIFO, SCHED_RR */
	__u32 sched_priority;

	/* SCHED_DEADLINE (nsec) */
	__u64 sched_runtime;
	__u64 sched_deadline;
	__u64 sched_period;
 };

 int sched_setattr(pid_t pid,
		  const struct sched_attr *attr,
		  unsigned int flags)
 {
	return syscall(__NR_sched_setattr, pid, attr, flags);
 }

 int sched_getattr(pid_t pid,
		  struct sched_attr *attr,
		  unsigned int size,
		  unsigned int flags)
 {
	return syscall(__NR_sched_getattr, pid, attr, size, flags);
 }

 void *run_deadline(void *data)
 {
	struct sched_attr attr;
	int x = 0;
	int ret;
	unsigned int flags = 0;
	cpu_set_t *cpuset;

	printf("deadline thread started [%ld]\n", gettid());

/* set thread affinity */
	cpuset = CPU_ALLOC(8);
	CPU_SET(0, cpuset);
	CPU_SET(1, cpuset);
	
	ret = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), cpuset);
	if (ret != 0) {
		perror("pthread_setaffinity_np");
		pthread_exit(NULL);
	}

	attr.size = sizeof(attr);
	attr.sched_flags = 0;
	attr.sched_nice = 0;
	attr.sched_priority = 0;

	/* This creates a 10ms/30ms reservation */
	attr.sched_policy = SCHED_DEADLINE;
	attr.sched_runtime = 1 * 1000 * 1000;
	attr.sched_period = attr.sched_deadline = 10 * 1000 * 1000;


	ret = sched_setattr(gettid(), &attr, flags);
	if (ret < 0) {
		done = 0;
		perror("sched_setattr");
		exit(-1);
	}


	/* generate matrix */
			unsigned index, row, col; //loop indicies
	unsigned matrix_size, squared_size;
	double *A, *B, *C;

	matrix_size = 6000;///atoi(argv[1]);

	squared_size = matrix_size * matrix_size;

	printf("Generating matrices...\n");

	A = (double*) malloc( sizeof(double) * squared_size );
	B = (double*) malloc( sizeof(double) * squared_size );
	C = (double*) malloc( sizeof(double) * squared_size );

	for( index = 0; index < squared_size; index++ ){
		A[index] = (double) rand();
		B[index] = (double) rand();
		C[index] = 0.0;
	}



	//while (!done) {


	printf("Multiplying matrices...\n");

	for( row = 0; row < matrix_size; row++ ){
		for( col = 0; col < matrix_size; col++ ){
			for( index = 0; index < matrix_size; index++){
			C[row*matrix_size + col] += A[row*matrix_size + index] *B[index*matrix_size + col];
				}	
			}
		}
	
	//}

/*
	for(int i=0;i<squared_size;i++) {
		printf("%.2f ..",C[i]);
	} */
	printf("deadline thread dies [%ld]\n", gettid());
	return NULL;
 }

 int main (int argc, char **argv)
 {
	pthread_t thread;
	pthread_t thread2;

	printf("main thread [%ld]\n", gettid());

	pthread_create(&thread, NULL, run_deadline, NULL);
	pthread_create(&thread2, NULL, run_deadline, NULL);
	
	sleep(10);

	//done = 1;
	pthread_join(thread, NULL);
	pthread_join(thread2, NULL);
	
	printf("main dies [%ld]\n", gettid());
	return 0;
 }
