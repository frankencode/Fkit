/*
 * Set.hpp -- ordered set
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_SET_HPP
#define FTL_SET_HPP

#include "containers.hpp"
#include "OrdinalTree.hpp"

namespace ftl
{

template<class T>
class Set: public Container< T, Set<T> >, public Sequence<T, int>
{
public:
	typedef T Item;
	typedef GenericIterator<Set> Iterator;

	Set() {}

	Set(const Set& b): tree_(b.tree_) {}
	inline const Set& operator=(const Set& b) { tree_ = b.tree_; return *this; }

	inline Iterator iterator() const { return Iterator(this); }

	inline bool isEmpty() const { return tree_.weight() == 0; }
	inline int length() const { return tree_.weight(); }
	inline int size() const { return tree_.weight(); }

	inline bool has(int index) const {
		return index < length();
	}
	inline Item get(int index) const {
		return at(index);
	}
	inline const Item& at(int index) const {
		Node* node = 0;
		if (tree_.lookupByIndex(index, &node)) return node->item_;
		else return nullItem_;
	}

	/** Return the index of the first item greater or equal _a_
	  */
	inline int first(const Item& a) const { return tree_.first(a); }

	/** Return the index of the first item lower or equal _b_
	  */
	inline int last(const Item& b) const { return tree_.last(b); }

	/** Insert a new item if no item with the same value exists already.
	  * If currentItem is non-null the item with the same value is returned.
	  * The function returns true if the new item was inserted successfully.
	  */
	inline bool insert(const Item& item, Item* currentItem = 0, int* index = 0)
	{
		bool found = false;
		bool below = true;
		Node* k = tree_.find(item, &found, &below, index);
		if (found) {
			if (currentItem)
				*currentItem = k->item_;
		}
		else {
			tree_.attach(k, new Node(item), below);
		}
		return !found;
	}

	inline bool remove(const Item& item, int* index = 0)
	{
		bool found;
		Node* k = tree_.find(item, &found, 0, index);
		if (found)
			tree_.remove(k);
		return found;
	}

	/** Convenience wrapper to lookup()
	  */
	inline bool contains(const Item& item) { return tree_.lookup(item); }

	inline Set& push(const Item& item)
	{
		bool found = false;
		bool below = true;
		Node* k = tree_.find(item, &found, &below);
		if (found)
			k->item_ = item;
		else
			tree_.attach(k, new Node(item), below);
		return *this;
	}

	inline Set& pop(Item* item)
	{
		FTL_ASSERT(!isEmpty());
		Node* k = tree_.min();
		*item = k->item_;
		tree_.remove(k);
		return *this;
	}

	inline Item pop() {
		Item item;
		pop(&item);
		return item;
	}

	inline void clear() { tree_.clear(); }

private:
	typedef OrdinalTree< OrdinalNode<Item> > Tree;
	typedef typename Tree::Node Node;
	Tree tree_;
	Item nullItem_;
};

} // namespace ftl

#endif // FTL_SET_HPP