#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}


// Proj 3 - Thread create/join, lock_init/acquire/release
// Locks from 3-easy-pieces
/*************************
1 typedef struct __lock_t { int flag; } lock_t;
2
3 void init(lock_t *mutex) {
4 // 0 -> lock is available, 1 -> held
5 mutex->flag = 0;
6 }
7
8 void lock(lock_t *mutex) {
9 while (mutex->flag == 1) // TEST the flag
10 ; // spin-wait (do nothing)
11 mutex->flag = 1; // now SET it!
12 }
13
14 void unlock(lock_t *mutex) {
15 mutex->flag = 0;
16 }
*************************/

// Atomic add from wikipedia

static inline int fetch_and_add(int* variable, int value)
{
    __asm__ volatile("lock; xaddl %0, %1"
      : "+r" (value), "+m" (*variable) // input + output
      : // No input-only
      : "memory"
    );
    return value;
}
// Seems to be implemented as xchg() ?
void
lock_init(lock_t* mutex)
{
	mutex->flag = 0;
}

void
lock_acquire(lock_t* mutex)
{
	while(mutex->flag == 1){

	}
	fetch_and_add(&mutex->flag,1);
}

void
lock_release(lock_t* mutex)
{
	fetch_and_add(&mutex->flag,0);
}


int
thread_create(void (*start_routine)(void*, void*), void* arg1, void* arg2)
{
	int retval;	
	// Thread create calls clone 
	lock_t mutex;
	lock_init(&mutex);
	lock_acquire(&mutex);
	void* stack = malloc(2 * PGSIZE);
	if((uint)stack % PGSIZE)
		return -1;
	lock_release(&mutex);

	retval = clone(start_routine, arg1, arg2, stack);
	return retval;	
}

int
thread_join()
{
	// Thread join calls syscall join and frees 
	// user stack. User stack is obtained via join
	// (puts user stack into stack variable)
	void* stack = malloc(sizeof(void*));
	int retval = join(stack);
	
	lock_t mutex;
	lock_init(&mutex);
	lock_acquire(&mutex);
	free(stack);
	lock_release(&mutex);

	return retval;
}
