#include <mutex>
#include <thread>
#include <stack>
#include "thread.h"


template<typename stackType>
threadController<stackType>::~threadController(){
	stop();
	while (!inputStack.empty()) {
		inputStack.pop();
	}
	while (!outputStack.empty()) {
		outputStack.pop();
	}
}

template<typename stackType>
void threadController<stackType>::stop(){
	state_mutex.lock();
	state = 0;
	state_mutex.unlock();
	
	wait_mutex.lock();
	if (thread != 0) {
		thread->join();
		delete thread;
		thread = 0;
	}
	wait_mutex.unlock();
}

template<typename stackType>
bool threadController<stackType>::isRunning(){
	int tmp;
	state_mutex.lock();
	tmp = state;
	state_mutex.unlock();	
	if(tmp == 1){return true;}

	if(tmp == 2){
		wait_mutex.lock();
		wait_mutex.unlock();
		return true;
	}

	return false;
}

template<typename stackType>
void threadController<stackType>::resume(){
	state_mutex.lock();
	state = 1;
	wait_mutex.unlock();
	state_mutex.unlock();	
}

template<typename stackType>
void threadController<stackType>::wait(){
	state_mutex.lock();
	state  = 2;
	wait_mutex.try_lock();
	state_mutex.unlock();
}

template<typename stackType>
void threadController<stackType>::setThread(std::thread* thread_input) {
	state_mutex.lock();
	std::thread * tmp = thread;
	state_mutex.unlock();
	if (tmp != 0) {
		stop();
	}
	state_mutex.lock();
	thread = thread_input;
	state = 1;
	state_mutex.unlock();
}

template<typename stackType>
void threadController<stackType>::send_input(stackType in) {
	io_mutex.lock();
 	inputStack.push(in);
	io_mutex.unlock();
}

template<typename stackType>
void threadController<stackType>::send_output(stackType in) {
	io_mutex.lock();
	outputStack.push(in);
	io_mutex.unlock();
}

template<typename stackType>
stackType threadController<stackType>::recv_input() {
	stackType tmp = 0;
	io_mutex.lock();
	if (!inputStack.empty()) {
		tmp = inputStack.top();
		inputStack.pop();
	}
	io_mutex.unlock();
	return tmp;
}

template<typename stackType>
stackType threadController<stackType>::recv_output() {
	stackType tmp;
	io_mutex.lock();
	if (!outputStack.empty()) {
		tmp = outputStack.top();
		outputStack.pop();
	}
	io_mutex.unlock();
	return tmp;
}
