#ifndef thread_Controller_h
# define thread_Controller_h
#include <mutex>
#include <thread>
#include <queue>

# include <iostream>

template <typename queueType>
class threadController{
	private:
		std::mutex state_mutex;
		std::mutex wait_mutex;
		std::mutex io_mutex;
		
		std::queue<queueType> inputQueue;
		std::queue<queueType> outputQueue;
		
		int state;
		std::thread * thread;
	public:
		threadController(std::thread* thread_input = 0) : thread(thread_input){
			if (thread == 0) {
				state = 0;
			}else{
				state = 1;
			}
		}
		~threadController(){
			stop();
			while (!inputQueue.empty()) {
				inputQueue.pop();
			}
			while (!outputQueue.empty()) {
				outputQueue.pop();
			}
		}
		void stop(){
			terminate();
			
			wait_mutex.lock();
			if (thread != 0) {
				thread->join();
				delete thread;
				thread = 0;
			}
			wait_mutex.unlock();
		}
		
		void terminate() {
			state_mutex.lock();
			state = 0;
			state_mutex.unlock();	
		}
		
		bool isRunning(){
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
		void resume(){
			state_mutex.lock();
			state = 1;
			wait_mutex.unlock();
			state_mutex.unlock();	
		}
		void wait(){
			state_mutex.lock();
			state  = 2;
			wait_mutex.try_lock();
			state_mutex.unlock();
		}
		
		void setThread(std::thread* thread_input){
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
		
		void send_input(queueType in, bool useLock = true){
			if (useLock) io_mutex.lock();
			inputQueue.push(in);
			if (useLock) io_mutex.unlock();
		}
		void send_output(queueType in, bool useLock = true){
			if (useLock) io_mutex.lock();
			outputQueue.push(in);
			if (useLock) io_mutex.unlock();
		}
		queueType recv_input(bool useLock = true){
			queueType tmp = 0;
			if (useLock) io_mutex.lock();
			if (!inputQueue.empty()) {
				tmp = inputQueue.front();
				inputQueue.pop();
			}
			if (useLock) io_mutex.unlock();
			return tmp;
		}
		queueType recv_output(bool useLock = true){
			queueType tmp;
			if (useLock) io_mutex.lock();
			if (!outputQueue.empty()) {
				tmp = outputQueue.front();
				outputQueue.pop();
			}
			if (useLock) io_mutex.unlock();
			return tmp;
		}
		
		bool try_lock_IO() {
			return io_mutex.try_lock();
		}
		
		void lock_IO() {
			io_mutex.lock();
		}
		void unlock_IO() {
			io_mutex.unlock();
		}
};
template <typename queueType>
class threadControllerCloser{
	threadController<queueType> * controller;
public:
	threadControllerCloser< queueType >(threadController<queueType> * controller) : controller(controller) {
	}
	~threadControllerCloser< queueType >(){
		controller->terminate();
	}
};

# define useThreadController(int, controller) threadControllerCloser<int> threadControllerCloser_tmp_obj(controller)

# endif