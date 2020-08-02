//
// Created by Owner on 30-May-20.
//

#ifndef EX3_ADV_CPP_THREADPOOLEXECUTOR_H
#define EX3_ADV_CPP_THREADPOOLEXECUTOR_H

#include "../common/common.h"
#include <thread>
#include <atomic>
#include "Simulation.h"


template<typename Producer>
class ThreadPoolExecutor
{
	Producer& producer;
	const int num_threads = -1;
	std::vector<std::thread> workers;
	std::atomic_bool running = false;   // Prevents second call to start()
	std::atomic_bool stopped = false;   // When asked to stopped gracefully the threads use this to know to stop taking tasks
	static thread_local int num_tasks_finished;     // thread_local is always static anyway. thread_local means there is 1 instance per thread.
	std::atomic_int total_num_tasks_finished { 0 };

	void worker_function();
public:
	ThreadPoolExecutor(Producer& producer, int num_threads);

	bool start();

	void stop_gracefully();
	void wait_till_finish();
};

// ThreadPoolExecuter.h - being a template - but outside of the class:
template<typename Producer>
thread_local int ThreadPoolExecutor<Producer>::num_tasks_finished { 0 };
// Have to define static variables

#endif //EX3_ADV_CPP_THREADPOOLEXECUTOR_H
