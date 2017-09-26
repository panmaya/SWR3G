/*
 * wm_list.c
 *
 * Created: 23/9/2557 14:08:01
 *  Author: pan
 */ 
#include <adl_global.h>
#include <FreeRTOSConfig.h>


#if (!WM_TYPES_MALLOC_C_LIB)
#define wm_memalloc	adl_memGet
#define wm_memfree	adl_memRelease
#else
#define wm_memalloc	malloc
#define wm_memfree	free
#endif
 
typedef struct	WM_LIST_ITEM {
	int num;
	void* pData;
	struct WM_LIST_ITEM* next;
}wm_list_item_t;

typedef struct WM_LIST_T {
	int16_t add_last_pos;
	u16 Attr;
	wm_lstTable_t * tab;
	wm_list_item_t* head;
	wm_list_item_t* tail;
}wm_List_t;
 
 
/* Will always return the pointer to my_list */
s16 _wm_lstAddItem ( wm_lst_t list, void * item, const char * file, uint32_t line ) {
	wm_List_t* s = list;
	wm_list_item_t* p = wm_memalloc(sizeof(wm_list_item_t));
	
	if( NULL == p )
	{
		return -1;
	}
	
	p->pData = item;
	p->next = NULL;
	
	
	if( NULL == s )
	{
		//printf("Queue not initialized\n");
		wm_memfree(p);
		return -2;
	}
	else if( NULL == s->head && NULL == s->tail )
	{
		/* printf("Empty list, adding p->num: %d\n\n", p->num);  */
		s->head = s->tail = p;
		//s->add_last_pos = 1;
		s->add_last_pos = 0;
		return s->add_last_pos;
	}
	else if( NULL == s->head || NULL == s->tail )
	{
		//fprintf(stderr, "There is something seriously wrong with your assignment of head/tail to the list\n");
		wm_memfree(p);
		return -3;
	}
	else
	{
		/* printf("List not empty, adding element to tail\n"); */
		s->tail->next = p;
		s->tail = p;
		s->add_last_pos++;
		
	}
	
	return s->add_last_pos;
}


/* This is a queue and it is FIFO, so we will always remove the first element */
int16_t wm_lstDeleteItem ( wm_lst_t list, s16 index ) {
	wm_List_t* s = list;
	wm_list_item_t* h = NULL;
	wm_list_item_t* p = NULL;
	
	if( NULL == s ) {
		//printf("List is not created\n");
		return -1;
	}
	else if( NULL == s->head && NULL == s->tail )
	{
		//printf("Well, List is empty\n");
		return -2;
	}
	else if( NULL == s->head || NULL == s->tail )
	{
		//printf("There is something seriously wrong with your list\n");
		//printf("One of the head/tail is empty while other is not \n");
		return -3;
	}
	
	h = s->head;
	p = h->next;
	if(h->pData) wm_memfree(h->pData);
	wm_memfree(h);
	s->head = p;
	if( NULL == s->head )  s->tail = s->head;   /* The element tail was pointing to is free(), so we need an update */
	if(s->add_last_pos >= 0 ){s->add_last_pos--;}
	return (s->add_last_pos >= 0) ? (s->add_last_pos + 1) : 0;
}



wm_lst_t _wm_lstCreate ( u16 Attr, wm_lstTable_t * funcTable, const char * file, uint32_t line ) {
	wm_List_t * p = wm_memalloc(sizeof(wm_List_t));

	if(!p) {
		return NULL;
	}
	p->head = p->tail = NULL;
	p->add_last_pos = -1;
	p->Attr = Attr;
	p->tab = funcTable;
	return p;
}

void wm_lstDestroy ( wm_lst_t list ) {
	wm_List_t * s = list;
	while( s->head ) {
		wm_lstDeleteItem(s,0);
	}
	wm_memfree(s);
}

void * wm_lstGetItem ( wm_lst_t list, s16 index ) {
	wm_list_item_t* p = NULL;
	wm_List_t * ps = list;
	if(ps) {
		for( p = ps->head; p; p = p->next )
		{
			if(p)
			{
				//printf("Num = %d\n", p->num);
				if(index==0) return p->pData;
				else index--;

			}
			else
			{
				//printf("Can not print NULL struct \n");
				return NULL;
			}
		}
	}
	//printf("------------------\n");
	return NULL;
}

u16 wm_lstGetCount ( wm_lst_t list ) {
	wm_list_item_t* p = NULL;
	wm_List_t * ps = list;
	u16 index = 0;
	if(ps) {
		for( p = ps->head; p; p = p->next )
		{
			if(p)
			{
				index++;

			}
			else
			{
				//printf("Can not print NULL struct \n");
				return index;
			}
		}
	}
	//printf("------------------\n");
	return index;
}
