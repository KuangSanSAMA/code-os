#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#define MAPSIZE	100

struct map{
	int m_addr;
	int m_size;
};

struct map map[MAPSIZE];

int BF_malloc(struct map *mp,int size){
	register int a,s;
	register struct map *bp,*bpp;
	for(bp=mp;bp->m_size;bp++){
		if(bp->=m_size>size){
			a = bp->m_addr;
			s = bp->m_size;
			for(bpp=bp;bpp->m_size;bpp++){
				if(bpp->m_size>=size && bpp->m_size<s){
					a=bpp->m_addr;
					s=bpp->m_size;
					bp=bpp;
				}
			}
			bp->m_addr+=size;
			if((bp->m_size-=size)==0)
				do{
					bp++;
					(bp-1)->m_addr = bp->m_addr;
				}while((bp-1)->m_size=bp->m_size);
			return(a);
		}
	}
	return(-1);
}

int WF_malloc(struct map *mp,int size){
	register int a,s;
	register struct map *bp,*bpp;
	for(bp=mp;bp->m_size;bp++){
		if(bp->m_size>=size){
			a=bp->m_addr;
			s=bp->m_size;
			for(bpp=bp;bpp->m_size;bpp++){
				if(bpp->m_size > s){
					a=bpp->m_addr;
					s=bpp->m_size;
					bp=bpp;
				}
			}
			bp->m_addr+=size;
			if((bp->m_size-=size)==0)
				do{
					bp++;
					(bp-1)->m_addr=bp->m_addr;
				}while((bp-1)->m_size=bp->m_size);
			return(a);
		}
	}
	return(-1);
}

void mfree(struct map *mp,int aa,int size){
	register struct map *bp;
	register int t;
	register int a;
	a=aa;
	for(bp=mp;bp->m_addr<=a&&bp->m_size!=0;bp++);
	if(bp>mp&&(bp-1)->m_addr+(bp-1)->m_size==a){
		(bp-1)->m_size+=size;
		if(a+size==bp->m_addr){
			(bp-1)->m_size+=bp->m_size;
			while(bp->m_size)
			{
				bp++;
				(bp-1)->m_addr=bp->m_addr;
				(bp-1)->m_size=bp->m_size;
			}
		}
	}
	else{
		if(a+size==bp->m_addr&&bp->m_size){
			bp->m_addr-=size;
			bp->m_size+=size;
		}
		else if(size)
			do{
				t=bp->m_addr;
				bp->m_addr=a;
				a=t;
				t=bp->m_size;
				bp->m_size=size;
				bp++;
			}while(size=t);
	}
}

void init(){
	struct map *bp;
	int addr,size;
	int i = 0;
	bp = map;
	printf("Please input starting addr and total size:");
	scanf("%d,%d",&addr,&size);
	bp->m_addr=addr;
	bp->m_size=size;
	(++bp)->m_size=0;
}

void show_map(){
	int i = 0;
	system("clear");
	struct map *bp;
	bp = map;
	printf("\nCurrent memory map...\n");
	printf("Address\t\tSize\n");
	while(bp->m_size!=0){
		printf("<%d\t\t%d\n",bp->m_addr,bp->m_size);
		bp++;
	}
	printf("\n");
}

main(){
	int a,s;
	char c;
	int i;
	init();
	printf("Please input,b for BF,W for WF:");
	scanf("%c",&c);
	do{
		show_map();
		printf("Please input ,1 for request,2 for release,0 for exit:");
		scanf("%d",&i);
		switch(i)
		{
			case 1:
				printf("Please input size:");
				scanf("%d",&s);
				if(c=='b')
					a=BF_malloc(map,s);
				else
					a=WF_malloc(map,s);
				if(a==-1)
					printf("request can not be satisfied\n");
				else
					printf("alloc memory at address:%d,size:%d\n",a,s);
				break;
			case 2:
				printf("Please input addr and size:");
				scanf("%d,%d",&a,&s);
				mfree(map,a,s);
				break;
			case 0:
				exit(0);
		}
	}while(1);
}
