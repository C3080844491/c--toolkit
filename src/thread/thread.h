/*
 * thread.h
 *
 *  Created on: Aug 15, 2018
 *      Author: c
 */

#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <signal.h>

class Thread {
public:
	Thread(void);
	Thread(const std::string &name);
	virtual ~Thread(void);

	void start(void);
	virtual void* run(void *args);

	pthread_t getTid(void);
	const std::string &getName(void);

private:
	static void *runWrapper(void *args);

protected:
	pthread_t tid;
	std::string name;
};




Thread::Thread(void) { }

Thread::Thread(const std::string &name) :
		name(name) { }

Thread::~Thread(void) {
	pthread_kill(tid, 0);
}

void *Thread::runWrapper(void *args) {
	Thread *t = (Thread *) args;
	return (t->run)(NULL);
}

void Thread::start(void) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, Thread::runWrapper, (void *)this);
	pthread_join(tid, NULL);
}

void* Thread::run(void *args) {
	return NULL;
}

pthread_t Thread::getTid(void) {
	return tid;
}

const std::string &Thread::getName(void) {
	return name;
}


#endif /* _THREAD_H_ */
