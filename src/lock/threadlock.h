/*
 * ThreadLock.h
 *
 *  Created on: Aug 7, 2018
 *      Author: c
 */

#ifndef _LOCK_THREADLOCK_H_
#define _LOCK_THREADLOCK_H_

#include <pthread.h>


class ILock {
public:
	virtual int acquire(void) = 0;
	virtual int release(void) = 0;

	virtual ~ILock(void) {};
};


/**
 * Mutex
 */
class Mutex : virtual public ILock {
public:
	enum EMutexType {
		MUTEX_TYPE_NORMAL 	= PTHREAD_MUTEX_NORMAL,
		MUTEX_TYPE_RECURSIVE 	= PTHREAD_MUTEX_RECURSIVE,
		MUTEX_TYPE_ERRORCHECK 	= PTHREAD_MUTEX_ERRORCHECK,
		MUTEX_TYPE_DEFAULT 		= PTHREAD_MUTEX_DEFAULT
	};

public:
	Mutex(EMutexType type = MUTEX_DEFAULT) {
		::pthread_mutexattr_init(&attr_);
		::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		::pthread_mutex_init(&lock_);
	}

	virtual ~Mutex(void) {
		::pthread_mutexattr_destroy(&attr_);
		::pthread_mutex_destroy(&lock_);
	}

	virtual int acquire(void) {
		return ::pthread_mutex_lock(&lock_);
	}

	virtual int release(void) {
		return ::pthread_mutex_unlock(&lock_);
	}

private:
	void operator=(const Mutex &);
	Mutex(const Mutex &);

private:
	pthread_mutexattr_t attr_;
	pthread_mutex_t lock_;
};


/**
 * RWLock
 */
class RWLock : virtual public ILock {
public:
	RWLock(void) {
		::pthread_rwlock_init(&lock_);
	}

	virtual ~RWLock(void) {
		::pthread_rwlock_destroy(&lock_);
	}

	virtual int acquire(void) {
		return ::pthread_rwlock_wrlock(&lock_);
	}

	virtual int release(void) {
		return ::pthread_rwlock_unlock(&lock_);
	}

	int acquireRead(void) {
		return ::pthread_rwlock_rdlock(&lock_);
	}

	int acquireWrite(void) {
		return ::pthread_rwlock_wrlock(&lock_);
	}

private:
	void operator=(const RWLock &);
	RWLock(const RWLock &);

private:
	pthread_rwlock_t lock_;
};


/**
 * Spin
 */
class Spin : virtual public ILock {
public:
	Spin(void) {
		::pthread_spin_init(&lock_);
	}

	virtual ~Spin(void) {
		::pthread_spin_destroy(&lock_);
	}

	virtual int acquire(void) {
		return ::pthread_spin_lock(&lock_);
	}

	virtual int release(void) {
		return ::pthread_spin_unlock(&lock_);
	}

private:
	void operator=(const Spin &);
	Spin(const Spin &);

private:
	pthread_spin_t lock_;
};


template<typename Lock>
class Guard {
public:
	Guard(Lock &lock) : lock_(&lock) {
		lock_->acquire();
	}

	virtual ~Guard(void) {
		lock_->release();
		lock_ = NULL;
	}

private:
	Lock *lock_;
};

#define GUARD(type, name, lock)		Guard< type > name(lock)


#endif /* _LOCK_THREADLOCK_H_ */
