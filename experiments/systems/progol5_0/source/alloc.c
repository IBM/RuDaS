#include 	<stdio.h>
#include        "progol.h"
/*
 * #######################################################################
 *
 *			Allocation Routines
 *                      -------------------
 *
 * #######################################################################
 */

/*
 * Allocation is done via a table of blocks. The table is indexed
 *	by the size of the block required, and each entry in the
 *	block table is a stack of blocks of that size. Each stack is
 *	made up of a linked list where next-element pointers
 *	are written as the first four bytes on every block.
 * The blocks_table contains blocks in an exponential scale ranging from
 *	2^2 to 2^31. Blocks required of any particular size are
 *	rounded up to the nearest power of 2. Although this requires
 *	an overhead of up to half the used memory, it avoids memory
 *	fragmentation. When a stack for a particular size of block is
 *	empty then a bunch of blocks equal in in number to those
 *	allocated so far of this size is malloc'ed and linked together
 *	to replenish the stack. Thus frequency of replenishment
 *	for commonly used block sizes decreases exponentially.
 */

#define         MAXBLOCK        32l
#define         MINBLOCK        2l

LONG blocks_taken[MAXBLOCK];
LONG blocks_in_table[MAXBLOCK];
char *block_table[MAXBLOCK];

int
a_zero_table()
        {
        register char **x;
        register LONG i,*y,*z;
        for (i=MAXBLOCK,x=block_table,y=blocks_taken,z=blocks_in_table;
                        i--;x++,y++,z++) {
                *x=(char *)NULL;
                *y=(*z=0l);
        }
}

int
a_pr_block_stats()
        {
        register LONG i,x,y,size;
        register LONG total_taken=0,total_table=0l;
        printf("Block Statistics Table\n");
        printf("\tSIZE\tTAKEN\tTABLE\tBYTES\tBYTES\n");
        printf("\t\t\t\tTAKEN\tTABLE\n");
        for (i=0l; i < MAXBLOCK; i++) {
	  size=1l<<i;
	  x=blocks_taken[i];
	  y=blocks_in_table[i];
	  total_taken += size*x;
	  total_table += size*y;
	  if (x || y)
	    printf("\t%ld.\t%ld\t%ld\t%ld\t%ld\n",size,x,y,size*x,size*y);
        }
        printf("Total bytes taken = %ld\n",total_taken);
        printf("Total bytes in table = %ld\n",total_table);
        printf("Grand total = %ld bytes\n",total_taken+total_table);
}


/*
 * a_dalloc indexes into block_table using the size of the required block
 *      and attempts to pop off the head of the list. if the list is
 *      empty, a_dalloc asks a_get_blocks to calloc some more space and
 *      place it in the appropriate free list, links
 *      them into a list and then takes the head of the list.
 */

char *
a_dalloc(number,size)
        LONG number,size;
        {
	unsigned long int block_size,index;
	char **newblock,**pos;

  block_size=number*size;

	memout+=(block_size);
	block_size--;

  index=LOG2(block_size)+1l;

 	if(((index))<MINBLOCK) index=MINBLOCK;
	pos=block_table+index;

	if(!(*(pos))) { /* Get new bunch of blocks */

	   unsigned long int i,n,bunch;
	   char **p;
			bunch=blocks_taken[index]+1l;

// MJH override. Oly expand by 2
			bunch=2;

			size=1l<<index;

    if(!(*pos=(char *)calloc(bunch,size))) {
			d_error("a_get_blocks - calloc failed");
		}



		p=(char**)*pos;
	  n=(unsigned long int)(p);
    for(i=bunch;--i;) {
			*p=(char*)n;
			p=(char **)(*p);
			n+=size;
		}
    *p=(char *)NULL;

    blocks_in_table[index]+=bunch;
	}

	blocks_in_table[index]--;
	blocks_taken[index]++;
	newblock=(char **)*pos;		/* Pop block */
	*pos= *newblock;

// ensure that the memory returned is zeroed

	bzero( newblock, 1l<<index );

	return((char *)newblock);
}

/*
 * a_dfree takes a block of memory of given size and pushes onto the appropriate
 *      stack list.
 */

int
a_dfree(block, block_size)
	char **block;
	register LONG block_size;
	{
	register char **pos;
	register unsigned long int index;
	memout-=(block_size--);
	if ((index=LOG2(block_size)+1) < MINBLOCK)
		index = MINBLOCK;
	*block= *(pos=block_table+index);	/* Push block */
	*pos=(char *) block;
	blocks_in_table[index]++;
	blocks_taken[index]--;
}
