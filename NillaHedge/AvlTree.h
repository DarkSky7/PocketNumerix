// Copyright Mike Landis July 15, 2009.  All rights reserved.
#pragma once
#include <iostream>
#include <ostream>
#define	DEBUG	0

/* This is an AVL tree interface that implements a variation on a priority queue
 *
 * Refer to: p.10 of http://benpfaff.org/papers/libavl.pdf for 
 * relative performance of binary, AVL, splay, and Red-Black trees
 * refer to p.11 of same for recommendations on optimal tree structure
 * based on access and insertion expectations
 *
 * Pfaff's paper suggests splay trees might be more attractive than
 * AVL trees as the store for pricer, but http://en.wikipedia.org/wiki/Splay_tree
 * indicates that sequential accesses in a splay tree will leave the tree unordered,
 * so I'm betting on AVL trees.
 *
 * for AVL rotations, see: http://en.wikipedia.org/wiki/AVL_tree
 *
 * Implementation details...
 * This is a three pointer (right, left, parent) AVL implementation.  It's more
 * of a memory hog than a two pointer version, but implemenation is more intuitive
 * and having the parent pointer available enables walking up the hierarchy from any
 * node on demand.  That saves building a speculative breadcrumb stack in Insert()
 * and Delete() for the rebalancing operations that can follow as often as half of
 * the time.  The advantages are better speed and simpler code at the expense of memory.
 * Pred() and Succ() need no state information (push down stack) on the traversal,
 * yet have the same big O() complexity as a breadcrumb implementation.
 *
 * AVL tree implementations sometimes provide simple LeftRotate() and RightRotate()
 * and use them to accomplish left-left, left-right, right-left, and right-right
 * rotations.  This implementation provides single step double rotations for each
 * (LL, LR, RL, and RR) rotations, for a slight performance improvement.
 */

template <class Payload_T, class Key_T, class Pred_T>
class AvlTree
{
public:
	AvlTree( void );
	virtual ~AvlTree( void );

	Payload_T*							GetPayload( void )			{	return	payload;		}
	AvlTree<Payload_T,Key_T,Pred_T>*	GetRight( void )			{	return	right;			}
	AvlTree<Payload_T,Key_T,Pred_T>*	GetLeft( void )				{	return	left;			}
	unsigned short						GetHeight( void )			{	return	height;			}
	short								GetBalance( void )			{	return	(short)balance;	}
	bool		IsEmpty( void )				{	return	 payload == NULL;	}
	bool		OnlyHasRightChild( void )	{	return	 right  &&  !left;	}
	bool		OnlyHasLeftChild( void )	{	return	!right  &&   left;	}

	bool		Delete(	Key_T key, AvlTree<Payload_T,Key_T,Pred_T>*& root );				// only if key matches node payload

	bool		Find( Key_T& key, AvlTree<Payload_T,Key_T,Pred_T>*& it ) const;
	AvlTree<Payload_T,Key_T,Pred_T>*	Insert(	Payload_T* py, Key_T& key, AvlTree<Payload_T,Key_T,Pred_T>*& root );
	AvlTree<Payload_T,Key_T,Pred_T>*	Succ( void ) const;	// the next larger payload
	AvlTree<Payload_T,Key_T,Pred_T>*	Pred( void ) const;	// the next smaller payload
	AvlTree<Payload_T,Key_T,Pred_T>*	MaxNode( void ) const;
	AvlTree<Payload_T,Key_T,Pred_T>*	MinNode( void ) const;

protected:
	void		SetLeft( AvlTree<Payload_T,Key_T,Pred_T>* child );
	void		SetRight( AvlTree<Payload_T,Key_T,Pred_T>* child );
	void		SetPayload( Payload_T* py )		{	payload = py;	}

	bool	IsRightChild( void ) const	{	return	parent  &&  parent->right == this;	}
	bool	IsLeftChild( void )	 const	{	return	parent  &&  parent->left == this;	}
	void	Unlink( AvlTree<Payload_T,Key_T,Pred_T> const * const node );

	void	FixHeightBalance( void );
	void	FixRoot( AvlTree<Payload_T,Key_T,Pred_T>* pivot, AvlTree<Payload_T,Key_T,Pred_T>*& root );
	void	RebalanceTree( AvlTree<Payload_T,Key_T,Pred_T>*& root );
	void	RebalanceNode( AvlTree<Payload_T,Key_T,Pred_T>*& root );
	void	LL_Rotate( AvlTree<Payload_T,Key_T,Pred_T>*& root );
	void	LR_Rotate( AvlTree<Payload_T,Key_T,Pred_T>*& root );
	void	RL_Rotate( AvlTree<Payload_T,Key_T,Pred_T>*& root );
	void	RR_Rotate( AvlTree<Payload_T,Key_T,Pred_T>*& root );


	AvlTree<Payload_T,Key_T,Pred_T>*	parent;
	AvlTree<Payload_T,Key_T,Pred_T>*	left;
	AvlTree<Payload_T,Key_T,Pred_T>*	right;
	Payload_T*							payload;
	char								balance;	// < 0, when left is taller
	unsigned short						height;		// of the tree below
};			// AvlTree
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
		AvlTree<Payload_T,Key_T,Pred_T>::AvlTree( void )
			: balance(0), height(0), left(NULL), right(NULL), parent(NULL), payload(NULL)
{
}			// AvlTree()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
		AvlTree<Payload_T,Key_T,Pred_T>::~AvlTree( void )
{
	if ( payload )	delete	payload;
	if ( right )	delete	right;
	if ( left )		delete	left;
}			// ~AvlTree()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::SetLeft( AvlTree<Payload_T,Key_T,Pred_T>* child )
{
	left = child;
	if ( child )
		child->parent = this;
	FixHeightBalance();
}			// SetLeft()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::SetRight( AvlTree<Payload_T,Key_T,Pred_T>* child )
{
	right = child;
	if ( child )
		child->parent = this;
	FixHeightBalance();
}			// SetRight()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void		AvlTree<Payload_T,Key_T,Pred_T>::FixHeightBalance( void )
{		// in fixing nodes while climbing up the tree, one
		// could compare the 'unfixed' height with the 'fixed' height
		// if there's no change, abandon the rest of the climb
	unsigned short	lf_ht = left   ?  left->height  + 1  :  0;
	unsigned short	rg_ht = right  ?  right->height + 1  :  0;
	height  = rg_ht > lf_ht  ?  rg_ht  :  lf_ht;
	balance = rg_ht - lf_ht;
}			// FixHeightBalance()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::FixRoot(
	AvlTree<Payload_T,Key_T,Pred_T>*		pivot,
	AvlTree<Payload_T,Key_T,Pred_T>*&		root				)
{		// only called from XX_Rotate where topology gets modified
	if ( root == this )
	{	root = pivot;					// new root node
		pivot->parent = NULL;			// handles the up == NULL case in XX_Rotate's
	}
}			// FixRoot()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::Unlink( AvlTree<Payload_T,Key_T,Pred_T> const * const node )
{
	if ( left == node )
		left = NULL;
	if ( right == node )
		right = NULL;
	FixHeightBalance();
}			// Unlink()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
AvlTree<Payload_T,Key_T,Pred_T>*		AvlTree<Payload_T,Key_T,Pred_T>::MinNode( void ) const
{
	AvlTree<Payload_T,Key_T,Pred_T>* node = (AvlTree<Payload_T,Key_T,Pred_T>*)this;
	while ( node->left )
		node = node->left;				// smaller keys to the left
	return	node;
}			// MinNode()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
AvlTree<Payload_T,Key_T,Pred_T>*		AvlTree<Payload_T,Key_T,Pred_T>::Succ( void ) const
{		// navigate to the next larger key from this
	if ( right )						// return the leftmost (min) node in the right subtree
		return	right->MinNode();		// e.g. H --> J, R --> T, V --> Z
	if ( IsLeftChild() )
		return	parent;					// e.g. F --> H, H --> R, T --> V
		// for the remaining cases... need parent with a right child that isn't the node asking
	AvlTree<Payload_T,Key_T,Pred_T>* node = (AvlTree<Payload_T,Key_T,Pred_T>*)this;
	while ( node &&  node->IsRightChild() )
		node = node->parent;
	return	node->parent;				// e.g. J --> R, Z --> NULL
}			// Succ()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//			  .---- R ----.			
//			 /			   \			//
//		 .- H -.		 .- V -.
//		/		\		/		\		//
//	   F		 J	   T		 Z
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
AvlTree<Payload_T,Key_T,Pred_T>*		AvlTree<Payload_T,Key_T,Pred_T>::Pred( void ) const
{		// navigate to the next smaller key from this
	if ( left )							// return the rightmost node (max) in the left subtree
		return	left->MaxNode();		// e.g. V --> T, R --> J, H --> F
	if ( IsRightChild() )
		return	parent;					// e.g. Z --> V, V --> R, Z --> H
		// for the remaining cases... need parent with a left child that isn't the node asking
	AvlTree<Payload_T,Key_T,Pred_T>* node = (AvlTree<Payload_T,Key_T,Pred_T>*)this;
	while ( node  &&  node->IsLeftChild() )
		node = node->parent;
	return	node->parent;				// e.g. T --> R, F --> NULL
}			// Pred()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
AvlTree<Payload_T,Key_T,Pred_T>*		AvlTree<Payload_T,Key_T,Pred_T>::MaxNode( void ) const
{
	AvlTree<Payload_T,Key_T,Pred_T>* node = (AvlTree<Payload_T,Key_T,Pred_T>*)this;
	while ( node->right )
		node = node->right;				// larger keys to the right
	return	node;
}			// MaxNode()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
bool	AvlTree<Payload_T,Key_T,Pred_T>::Find(
	Key_T& key,
	AvlTree<Payload_T,Key_T,Pred_T>*& node		) const
{		// return true & node where py matches node->payload, else false & best place to insert py
	node = (AvlTree<Payload_T,Key_T,Pred_T>*)this;
	while ( node )
	{	Payload_T*	nodePy = node->payload;
		if ( !nodePy )
			return	false;				// root can have a NULL payload
		if ( *nodePy == key )
			return	true;				// payload must be the message receiver
			// smaller keys to the left
		AvlTree<Payload_T,Key_T,Pred_T>* maybe = *nodePy > key ?  node->left  :  node->right;
		if ( !maybe )
			break;						// leaves node pointing to a valid node
		node = maybe;
	}
	return	false;
}			// Find()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
AvlTree<Payload_T,Key_T,Pred_T>*	AvlTree<Payload_T,Key_T,Pred_T>::Insert(
	Payload_T*							py,
	Key_T&								key,
	AvlTree<Payload_T,Key_T,Pred_T>*&	root								)
{
	if ( !py )
		throw	"AvlTree::Insert with NULL py argument";

#if DEBUG
	std::cerr << std::endl << "Insert(" << key << ") before tree..." << std::endl;
	DumpTree( root );
#endif

	AvlTree<Payload_T,Key_T,Pred_T>* it;	// Find() writes 'it'
	if ( Find( key, it ) )
		;									// already indexed py, return it below
	else if ( ! it->payload )
		it->payload = py;
	else
	{		// insert py at it
		AvlTree<Payload_T,Key_T,Pred_T>* node = new AvlTree<Payload_T,Key_T,Pred_T>;
		node->SetPayload( py );
		if ( *(it->payload) > key )						// payload has to receive the message
		{	if ( it->left )
			{	char buf[128];
				sprintf( buf, "AvlTree::Insert(%.2f) would overwrite left %.2f",
					payload->GetKey(), left->payload->GetKey() );		// not generic
				throw	buf;
			}
			it->SetLeft( node );										// sets node->parent too
		}
		else
		{	if ( it->right )
			{	char buf[128];
				sprintf( buf, "AvlTree::Insert(%.2f) would overwrite right %.2f",
					payload->GetKey(), right->payload->GetKey() );		// not generic
				throw	buf;
			}
			it->SetRight( node );		// sets node->parent too
		}
		it->RebalanceTree( root );		// fix height & balance; then rebalance the tree as needed
	}

#if DEBUG
	std::cerr << std::endl << "Insert(" << key << ") after tree..." << std::endl;
	DumpTree( root );
#endif

	return	it;
}			// Insert()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
bool	AvlTree<Payload_T,Key_T,Pred_T>::Delete(
	Key_T									key,
	AvlTree<Payload_T,Key_T,Pred_T>*&		root		)
{	AvlTree<Payload_T,Key_T,Pred_T>*		it;
	if ( payload  &&  *(payload) == key )
		it = (AvlTree<Payload_T,Key_T,Pred_T>*)this;
	else if ( root->Find( key, it ) )
		return	false;					// not matched & not found, nothing to delete 

#if DEBUG
	std::cerr << std::endl << "Delete(" << key << ") before tree..." << std::endl;
	DumpTree( root );
#endif

		// deal with 'it'
	if ( it->left  &&  it->right )
	{		// 'it' has two children... deleting it wouldn't leave enough links to accomodate
			// the rest of the nodes in the tree, so we overwrite it's payload with a payload
			// from an appropriately chosen node (an extremal twig within the subtree)
		AvlTree<Payload_T,Key_T,Pred_T>* twig;
		delete	it->payload;
			// select a min or max twig from it's taller subtree
		if ( it->left->height > it->right->height )
		{		// left subtree height > right subtree height
			twig = it->left->MaxNode();					// rightmost node has largest key on left side
			if ( !twig )
				throw "AvlTree::Delete twig is NULL";
			it->payload = twig->payload;				// steal twig payload so we can keep 'it'
			if ( it->left == twig )
				it->SetLeft( twig->left );				// nothing below twig to the right
			else
				twig->parent->SetRight( twig->left );	// arg could be NULL, but that's okay
		}
		else
		{		// right subtree height >= left subtree height
			twig = it->right->MinNode();				// leftmost node has smallest key on right side
			if ( !twig )
				throw "AvlTree::Delete twig is NULL";
			it->payload = twig->payload;				// steal twig payload so we can keep 'it'
			if ( it->right == twig )
				it->SetRight( twig->right );			// nothing below twig to the left
			else
				twig->parent->SetLeft( twig->right );	// arg could be NULL, but that's okay
		}
			// twig->parent is still valid even though it doesn't point back to twig
			// fix height & balance up the tree, rebalancing as needed
		AvlTree<Payload_T,Key_T,Pred_T>* tp = twig->parent;
		twig->payload = NULL;
		twig->right = twig->left = NULL;
		delete	twig;
		tp->RebalanceTree( root );
	}
	else	// 'it' has less than two children
	{	AvlTree<Payload_T,Key_T,Pred_T>* up = it->parent;
		bool	isl = it->IsLeftChild();				// false when 'it' has no parent
		bool	isr = it->IsRightChild();
		if ( it->OnlyHasRightChild() )
		{	if ( isl )
				up->SetLeft( it->right );			// when up == NULL... 
			if ( isr )
				up->SetRight( it->right );			// ...	isr == isl == false
			if ( !up )
			{	if ( root != it )
					throw "AvlTree::Delete: root doesn't point to parentless node";
				root = it->right;
			}
			it->right->parent = up;
			it->SetRight( NULL );
		}
		else if ( it->OnlyHasLeftChild() )
		{	if ( isl )
				up->SetLeft( it->left );
			if ( isr )
				up->SetRight( it->left );			// can't use else because isr != ~isl
			if ( !up )
			{	if ( root != it )
					throw "AvlTree::Delete: root doesn't point to parentless node";
				root = it->left;
			}
			it->left->parent = up;
			it->SetLeft( NULL );
		}
		else										// 'it' has no children
		{	if ( up )
				up->Unlink( it );
			else if ( root != it )					// no up, but root != it ?
				throw "AvlTree::Delete: root doesn't point to parentless, childless node";
		}
			// shared epilogue
		delete it->payload;		it->payload = NULL;
		if ( root != it )
			delete	it;
		if ( up )
			up->RebalanceTree( root );				// allow whole tree Rebalance
	}
#if DEBUG
	std::cerr << std::endl << "Delete(" << key << ") after tree..." << std::endl;
	DumpTree( root );
#endif
	return	true;
}			// Delete()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::RebalanceTree(
	AvlTree<Payload_T,Key_T,Pred_T>*&		root		)
{
	AvlTree<Payload_T,Key_T,Pred_T>*		node = (AvlTree<Payload_T,Key_T,Pred_T>*)this;
	while ( node )
	{	node->RebalanceNode( root );
		node = node->parent;
	}
}			// RebalanceTree()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::RebalanceNode( AvlTree<Payload_T,Key_T,Pred_T>*& root )
{
	FixHeightBalance();
	if ( balance < -1 )
	{		// rebalance left - choose between LL & LR rotations
		if ( !left )
			throw	"AvlTree::RebalanceNode LL or LR with NULL left?";
		if ( left->balance < 0 )
			LL_Rotate( root );		// left subtree longer
		else
			LR_Rotate( root );		// right subtree longer
	}
	else if ( balance > 1 )
	{		// rebalance right - choose between RL & RR rotations
		if ( !right )
			throw	"AvlTree::RebalanceNode RL or RR with NULL right?";
		if ( right->balance < 0 )
			RL_Rotate( root );		// left subtree longer
		else
			RR_Rotate( root );		// right subtree longer
	}
}			// RebalanceNode()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::RR_Rotate( AvlTree<Payload_T,Key_T,Pred_T>*& root )
{		// pivot, colors, ... defined by:
		// http://en.wikipedia.org/wiki/File:Tree_Rebalancing.gif
		// RR_Rotate is a single subtree rotation
		// Rotate common prologue
	AvlTree<Payload_T,Key_T,Pred_T>* up = parent;			// going to step on parent
	bool	isl = IsLeftChild();
	bool	isr = IsRightChild();							// Note: isr != ~isl
		// pivot moves from right to top; this becomes pivot->left
	AvlTree<Payload_T,Key_T,Pred_T>* pivot = right;			// lime 5
	if ( !pivot )
		throw "AvlTree::RR_Rotate: pivot is NULL";
	AvlTree<Payload_T,Key_T,Pred_T>* bb = pivot->left;		// gray B
		// 3->A & pivot->right(7) are unchanged
	pivot->SetLeft( this );				// steps on parent;	   green 3
	SetRight( bb );
		// Rotate common epilogue (fix the down pointer)
	if ( isl )
		up->SetLeft( pivot );
	if ( isr )
		up->SetRight( pivot );
	FixRoot( pivot, root );		// recompute height & balance here
}			// RR_Rotate()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::RL_Rotate( AvlTree<Payload_T,Key_T,Pred_T>*& root )
{		// pivot, colors, ... defined by:
		// http://en.wikipedia.org/wiki/File:Tree_Rebalancing.gif
		// RL_Rotate is a double subtree rotation
		// Rotate common prologue
	AvlTree<Payload_T,Key_T,Pred_T>* up = parent;			// going to step on parent
	bool	isl = IsLeftChild();
	bool	isr = IsRightChild();							// Note: isr != ~isl
		// pivot moves from RL to top, 
	AvlTree<Payload_T,Key_T,Pred_T>* lime = right;			// lime 5
	if ( !lime )
		throw "AvlTree::RL_Rotate: lime is NULL";
	AvlTree<Payload_T,Key_T,Pred_T>* pivot = lime->left;	// red 4
	if ( !pivot )
		throw "AvlTree::RL_Rotate: pivot is NULL";
	AvlTree<Payload_T,Key_T,Pred_T>* cc = pivot->right;
	AvlTree<Payload_T,Key_T,Pred_T>* black = pivot->left;
		// 3->A & 5->B are unchanged
	pivot->SetLeft( this );				// steps on parent;	   green 3
	pivot->SetRight( lime );
	SetRight( black );
	lime->SetLeft( cc );
		// Rotate common epilogue (fix the down pointer)
	if ( isl )
		up->SetLeft( pivot );
	if ( isr )
		up->SetRight( pivot );
	FixRoot( pivot, root );		// recompute height & balance here
}			// RL_Rotate()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::LR_Rotate( AvlTree<Payload_T,Key_T,Pred_T>*& root )
{		// pivot, colors, ... defined by:
		// http://en.wikipedia.org/wiki/File:Tree_Rebalancing.gif
		// LR_Rotate is a double subtree rotation
		// Rotate common prologue
	AvlTree<Payload_T,Key_T,Pred_T>* up = parent;			// going to step on parent
	bool	isl = IsLeftChild();
	bool	isr = IsRightChild();							// Note: isr != ~isl
		// pivot moves from RL to top, 
	AvlTree<Payload_T,Key_T,Pred_T>* green = left;			// green 3
	if ( !green )
		throw "AvlTree::LR_Rotate: green is NULL";
	AvlTree<Payload_T,Key_T,Pred_T>* pivot = green->right;	// red 4
	if ( !pivot )
		throw "AvlTree::LR_Rotate: pivot is NULL";
	AvlTree<Payload_T,Key_T,Pred_T>* cc = pivot->left;
	AvlTree<Payload_T,Key_T,Pred_T>* black = pivot->right;
		// 5->A & 3->B are unchanged
	pivot->SetRight( this );		// steps on parent;		   lime 5
	pivot->SetLeft( green );
	green->SetRight( cc );
	SetLeft( black );
		// Rotate common epilogue (fix the down pointer)
	if ( isl )
		up->SetLeft( pivot );
	if ( isr )
		up->SetRight( pivot );
	FixRoot( pivot, root );		// recompute height & balance here
}			// LR_Rotate()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Payload_T, class Key_T, class Pred_T>
void	AvlTree<Payload_T,Key_T,Pred_T>::LL_Rotate( AvlTree<Payload_T,Key_T,Pred_T>*& root )
{		// pivot, colors, ... defined by:
		// http://en.wikipedia.org/wiki/File:Tree_Rebalancing.gif
		// LL_Rotate is a single subtree rotation
		// Rotate common prologue
	AvlTree<Payload_T,Key_T,Pred_T>* up = parent;			// going to step on parent
	bool	isl = IsLeftChild();
	bool	isr = IsRightChild();							// Note: isr != ~isl
		// pivot moves from left to top, this moves to pivot->right
	AvlTree<Payload_T,Key_T,Pred_T>* pivot = left;			// green 3
	if ( !pivot )
		throw "AvlTree::LL_Rotate: pivot is NULL";
	AvlTree<Payload_T,Key_T,Pred_T>* bb = pivot->right;		// pink 2
		// 5->A & 2->C are unchanged
	pivot->SetRight( this );			// steps on parent	   lime 5
	SetLeft( bb );						
		// Rotate common epilogue (fix the down pointer)
	if ( isl )
		up->SetLeft( pivot );
	if ( isr )
		up->SetRight( pivot );
	FixRoot( pivot, root );		// recompute height & balance here
}			// LL_Rotate()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
