//
// Created by Owner on 30-May-20.
//

#include "ThreadPoolExecutor.h"

template<typename Producer>
void ThreadPoolExecutor<Producer>::worker_function() {
	while(!stopped) {
		auto task = producer.get_task();
		if(!task) break;
		(*task)();
		++num_tasks_finished;
		++total_num_tasks_finished;
	}
#ifdef THREAD_PRINTS

	if(stopped) {
		std::cout << std::this_thread::get_id() << " - stopped gracefully after processing " << num_tasks_finished << " task(s)" << std::endl;
	}
	else {
		std::cout << std::this_thread::get_id() << " - finished after processing " << num_tasks_finished << " task(s)" << std::endl;
	}
#endif
}

template<typename Producer>
bool ThreadPoolExecutor<Producer>::start() {
	bool running_status = false;
	// see: https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange
	if(!running.compare_exchange_strong(running_status, true)) { // updates running to true if it's different from running_status
		return false;                                            // weak is cheaper but could be wrong
	}
	for(int i=0; i < num_threads; ++i) {
		workers.push_back(std::thread([this]{
			worker_function();
		}));
	}
	return true;
}

template<typename Producer>
ThreadPoolExecutor<Producer>::ThreadPoolExecutor(Producer& producer,
												 int num_threads)
												 : producer(producer),
												   num_threads(num_threads)
{
	workers.reserve(num_threads);
}

template<typename Producer>
void ThreadPoolExecutor<Producer>::stop_gracefully()
{
	stopped = true;
	wait_till_finish();
}

template<typename Producer>
void ThreadPoolExecutor<Producer>::wait_till_finish()
{
	for(auto& t : workers)
	{
		t.join();
	}
#ifdef THREAD_PRINTS
	std::cout << "thread pool finished/stopped after processing " << total_num_tasks_finished << " task(s)" << std::endl;
#endif
}

template class ThreadPoolExecutor<Simulation>;
