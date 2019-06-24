/* ####################################################################### */

/*                      PROGOL incremental learning control		   */
/*                      ----------------------------------		   */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"

ITEM ct_pairs();
LONG b_findnth();

ITEM
ct_pairs(m,bits)
	LONG m;
	ITEM bits;
	{
	LONG size; ITEM pairs=L_EMPTY;
	BLOCK bsum=b_bitsum((BLOCK)I_GET(bits),&size);
	struct otree *tree=ot_sample(m,(size*(size-1))>>1);
	ct_pairs1(tree,(BLOCK)I_GET(bits),bsum,size,pairs);
	b_delete(bsum); ot_delete(tree);
	return(pairs);
}

int
ct_pairs1(tree,b,bsum,size,pairs)
	struct otree *tree;
	BLOCK b,bsum; LONG size; ITEM pairs;
	{
	LONG x,y,bitx,bity;
	if(tree) {
		ct_pairs1(tree->left,b,bsum,size,pairs);
		prdecode(tree->val,size,&x,&y);
		bitx=b_findnth(x+1l,b,bsum); bity=b_findnth(y+1,b,bsum);
		l_suf(i_dec(l_push(i_dec(b_elem(bitx,spatoms)),
		    l_push(i_dec(b_elem(bity,spatoms)),L_EMPTY))),pairs);
		ct_pairs1(tree->right,b,bsum,size,pairs);
	}
}
