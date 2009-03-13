/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_QUEUE_HPP
#define PONA_QUEUE_HPP

#include "atoms"

namespace pona
{

template<class T>
class Queue: public Instance
{
public:
	Queue(int size)
		: fill_(0),
		  size_(size),
		  head_(size_-1),
		  tail_(size_-1),
		  bufOwner_(true),
		  buf_(new T[size_])
	{}
	
	Queue(T* buf, int size)
		: fill_(0),
		  size_(size),
		  head_(size_-1),
		  tail_(size_-1),
		  bufOwner_(false),
		  buf_(buf)
	{}
	
	~Queue()
	{
		if (bufOwner_)
		{
			delete[] buf_;
			buf_ = 0;
		}
	}
	
	inline int size() const { return size_; }
	inline int fill() const { return fill_; }
	
	inline void pushBack(T e)
	{
		assert(fill_ != size_);
		++head_;
		if (head_ >= size_) head_ = 0;
		++fill_;
		buf_[head_] = e;
	}
	
	inline void pushFront(T e)
	{
		assert(fill_ < size_);
		buf_[tail_] = e;
		--tail_;
		if (tail_ < 0) tail_ = size_ - 1;
		++fill_;
	}
	
	inline T popBack()
	{
		assert(fill_ > 0);
		T h = buf_[head_];
		--head_;
		if (head_ < 0) head_ = size_ - 1;
		--fill_;
		return h;
	}

	inline T popFront()
	{
		assert(fill_ > 0);
		++tail_;
		if (tail_ >= size_) tail_ = 0;
		--fill_;
		return buf_[tail_];
	}
	
	inline void clear()
	{
		fill_ = 0;
		head_ = size_ - 1;
		tail_ = size_ - 1;
	}
	
	inline T back(int i = 0)
	{
		assert((0 <= i) && (i < fill_));
		i = -i;
		i += head_;
		if (i < 0) i += size_;
		return buf_[i];
	}
	
	inline T front(int i = 0)
	{
		assert((0 <= i) && (i < fill_));
		i += tail_ + 1;
		if (i >= size_) i -= size_;
		return buf_[i];
	}
	
private:
	PONA_DISABLE_COPY(Queue)
	
	int fill_;
	int size_;
	int head_;
	int tail_;
	bool bufOwner_;
	T* buf_;
};

} // namespace pona

#endif // PONA_QUEUE_HPP
