typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

// Proj 3 - Need to define lock_t type here. As defined ticket lock
typedef struct __lock_t {
	int ticket;
	int turn;
} lock_t;

#define NULL 0x0000
