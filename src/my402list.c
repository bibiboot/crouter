#include<stdio.h>
#include<stdlib.h>
#include "cs402.h"

#include "my402list.h"

int My402ListLength(My402List *list)
{
	return list->num_members;
}

int My402ListEmpty(My402List *list)
{
	if(list->num_members>0)
		return 0;

	else
		return 1;
}

int My402ListPrepend(My402List *list, void *item)
{

	/*Obtaining a memory block*/
	My402ListElem *elem=(My402ListElem*)malloc(sizeof(My402ListElem));

	/*Checking if Malloc Failed*/
	if(elem==NULL)
			return FALSE;

	/*Assisning values to newly created node*/
	elem->next=NULL;
	elem->prev=NULL;
	elem->obj=(void*)item;

	/*Obtaining first and last for the list*/

	/*If List is not empty then adding after last*/
	if (list->num_members==0)
	{
		((list->anchor).next)=elem;
        	((list->anchor).prev)=elem;
        	elem->next=&(list->anchor);
        	elem->prev=&(list->anchor);


    }

	/*If list is empty then first and last point to the only element*/
	else
	{
	    	elem->next=((list->anchor).next);
		(((list->anchor).next)->prev)=elem;
		elem->prev=&list->anchor;
		((list->anchor).next)=elem;

    }

	list->num_members++;
	return TRUE;
}

//int My402ListAppend(My402List *list, void *item, )
int My402ListAppend(My402List *list, void *item, My402ListElem **elem )
{

	*elem=(My402ListElem*)malloc(sizeof(My402ListElem));

	/*Checking if Malloc failed*/
	if(*elem==NULL)
		return FALSE;

	/*Assigning values to the node*/
    (*elem)->next=NULL;
	(*elem)->prev=NULL;
	(*elem)->obj=(void*)item;


	/*If List is not empty then adding before first*/
	if(list->num_members==0)
	{
	    ((list->anchor).next)=*elem;
        ((list->anchor).prev)=*elem;
        (*elem)->next=&(list->anchor);
        (*elem)->prev=&(list->anchor);
        list->num_members++;
        return TRUE;

    }

	/*If list is empty then first and last point to the only element*/
	else
	{
        (((list->anchor).prev)->next)=*elem;
        (*elem)->next=&(list->anchor);
        (*elem)->prev=((list->anchor).prev);
        ((list->anchor).prev)=*elem;
        list->num_members++;
        return TRUE;

	}

    return FALSE;
}

void My402ListUnlink(My402List *list,My402ListElem *item )
{


		((item->prev)->next)=(item->next);
        ((item->next)->prev)=(item->prev);
        list->num_members--;
}


void My402ListUnlinkAll(My402List *list)
{

	My402ListElem *elem=NULL;
	elem=My402ListFirst(list);


        while((list->num_members)!=0)
        {
            elem->next=((list->anchor).next)->next;


			(list->anchor).next=elem->next;
			(elem->next)->prev=&(list->anchor);
			list->num_members--;

		}


	My402ListInit(list);
}

int  My402ListInsertAfter(My402List *list, void *item, My402ListElem *elem)
{

    My402ListElem *temp=NULL;

    if(elem==NULL)
    {
        My402ListElem *temp;
        My402ListAppend(list,item, &temp);
    }
    else{
        My402ListElem *newElem=(My402ListElem*)malloc(sizeof(My402ListElem));
        if(newElem==NULL)
			return FALSE;
	newElem->obj=(void*)item;
	temp=elem->next;
	/*assigning the new pointers*/
	elem->next=newElem;
	newElem->next=temp;
	temp->prev=newElem;
	newElem->prev=elem;
	list->num_members++;

    return TRUE;
}
return FALSE;
}

int My402ListInsertBefore(My402List *list, void *item, My402ListElem *elem)
{

    My402ListElem *temp=NULL;

	if(elem==NULL)
    {
            My402ListPrepend(list,item);
    }

    else{

        My402ListElem *newElem=(My402ListElem*)malloc(sizeof(My402ListElem));
        if(newElem==NULL)
			return FALSE;
    temp=elem->prev;


	newElem->obj=(void*)item;
	/*assigning the new pointers*/
	temp->next=newElem;
	newElem->next=elem;
	elem->prev=newElem;
	newElem->prev=temp;
	list->num_members++;
	return TRUE;
    }
    return FALSE;
}

My402ListElem *My402ListFirst(My402List *list)
{
	if(list->num_members==0)
		return NULL;

	else
		return ((list->anchor).next);
}

My402ListElem *My402ListLast(My402List *list)
{
	if(list->num_members==0)
		return NULL;

	else
		return ((list->anchor).prev);
}

My402ListElem *My402ListNext(My402List *list,My402ListElem *item)
{

	if(((list->anchor).prev)==item)
		return NULL;

	else
		return (item->next);
}

My402ListElem *My402ListPrev(My402List *list,My402ListElem *item)
{

	if(((list->anchor).next)==item)
		return NULL;
	else
		return (item->prev);
}

My402ListElem *My402ListFind(My402List *list, void *item)
{
	My402ListElem *elem=NULL;


	/*loop for searching the element*/
	for (elem=My402ListFirst(list);elem != NULL;elem=My402ListNext(list, elem))
	{
		if(elem->obj==item)
		{
			return elem;
		}
	}

	return NULL;

}

int My402ListInit(My402List *list)
{
	if(list==NULL)
    {
        list=(My402List *)malloc(sizeof(My402List));
    }
	list->num_members=0;
	list->anchor.prev=&list->anchor;
	list->anchor.next=&list->anchor;

	if ((list->num_members==0)&&(list->anchor.prev==&list->anchor)&&(list->anchor.next==&list->anchor))
		return TRUE;

	else
		return FALSE;
}
