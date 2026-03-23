#pragma once

template <class Item_T>
class List
{
public:
	List( void );
	List( Item_T itm );
	virtual ~List( void );

	List*	Insert( Item_T itm );
	bool	Remove( Item_T itm );

	Item_T	item;
	List*	next;
};

template <class Item_T>
List<Item_T>::List( void )
	: next(NULL), item(NULL)
{
}

template <class Item_T>
List<Item_T>::List( Item_T itm )
	: next(NULL), item(itm)
{
}

template <class Item_T>
List<Item_T>::~List( void )
{	if ( next )
		delete next;
	if ( item )
		delete item;
}

template <class Item_T>
List<Item_T>*	List<Item_T>::Insert( Item_T itm )
{	if ( item == NULL )
	{	item = itm;
		return	this;
	}
	else
	{	List<Item_T>* node = next;
		while ( node->next )
			node = node->next;
		node->next = new List( itm );
		return	node->next;
	}
}

template <class Item_T>
bool	List<Item_T>::Remove( Item_T itm )
{	List<Item_T>* node = this;
	List<Item_T>* prev = NULL;
	while ( node  &&  node->item != itm )
	{	prev = node;
		node = node->next;
	}
	if ( node->item == itm )
	{	delete item;
		item = NULL;
		if ( prev )
			prev->next = NULL;
		return	true;
	}
	return	false;
}
