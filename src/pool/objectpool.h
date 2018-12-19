/*
 * ObjectPool.h
 *
 *  Created on: Aug 7, 2018
 *      Author: c
 */

#ifndef _POOL_OBJECTPOOL_H_
#define _POOL_OBJECTPOOL_H_

#include <list>
#include <unordered_map>
#include <algorithm>

#include "lock/threadlock.h"


template<typename Object, typename Lock = RecursiveMutex>
class ObjectPool {
public:
	const static int BUCKET_SIZE 	= 131073;

	struct ObjectNode {
		bool 	_inUse;
		void*	_object;
	};
	typedef std::list<ObjectNode *>						ObjectNodeList;
	typedef std::unordered_map<uint64_t, ObjectNode *> 	ObjectNodeMap;

public:
	ObjectPool();
	virtual ~ObjectPool();

	virtual Object *pop(void);
	virtual int push(Object *obj);

	void clear(void);

	void debufInfo(void) const;

protected:
	Lock 			lock_;

	size_t 			totalSize_;

	ObjectNodeList	list_;
	ObjectNodeMap	map_;
};



template<typename Object, typename Lock>
ObjectPool<Object, Lock>::ObjectPool(void)
	: totalSize_(0), map_(BUCKET_SIZE) {

}

template<typename Object, typename Lock>
ObjectPool<Object, Lock>::~ObjectPool(void) {
	this->clear();
}

template<typename Object, typename Lock>
Object *ObjectPool<Object, Lock>::pop(void) {
	GUARD(Lock, g, this->lock_);

	Object *obj = 0;
	ObjectNode *node = 0;
	if (!this->list_.empty()) {
		node = this->list_.front();
		this->list_.pop_front();
	}
	else {
		while (1) {
			if ((obj = new Object) == 0)
				continue;
			if ((node = new ObjectNode) == 0)
				continue;

			node->_object = obj;
			this->map_.insert(ObjectNodeMap::value_type((uint64_t) obj), node);
			break;
		}
	}

	node->_inUse = true;
	obj = (Object *) node->_object;

	return obj;
}

template<typename Object, typename Lock>
int ObjectPool<Object, Lock>::push(Object *obj) {
	GUARD(Lock, g, this->lock_);

	if (obj == 0)
		return -1;

	ObjectNodeMap::iterator it = this->map_.find((uint64_t) obj);
	if (it == this->map_.end())
		return -1;

	ObjectNode *node = it->second;
	if (node->_inUse)
		return -1;

	node->_inUse = false;
	this->list_.push_back(node);

	return 0;
}

template<typename Object, typename Lock>
void ObjectPool<Object, Lock>::clear(void) {
	GUARD(Lock, g, this->lock_);

	for (ObjectNodeList::iterator it = this->list_.begin(); it != this->list_.end(); ++it) {
		if ((*it)->_object != 0)
			delete ((Object *) ((*it)->object));
	}

	this->list_.clear();
	this->map_.clear();
}

template<typename Object, typename Lock>
void ObjectPool<Object, Lock>::debufInfo(void) {
	GUARD(Lock, g, this->lock_);

	std::cout << "Total=" << this->map_.size() << ","
			<< "unused=" << this->list_.size() << std::endl;
}

#endif /* _POOL_OBJECTPOOL_H_ */
