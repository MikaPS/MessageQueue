#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

template <typename T> class TSQueue {

	protected:
	
		// the underlying queue	
		std::queue<T> m_queue;
		
		// mutex for thread synchronization
		std::mutex m_mutex;
		
		// Condition variable for signaling
		std::condition_variable m_cond;
		
		// number of items in the queue
		std::atomic<size_t> m_enqued_items = 0;

		// a flag to stop the queue
		mutable std::atomic<bool> abort_flag = false;

	public:
		TSQueue() {}

		void abort() {
			// this can be called from another thread to stop operation
			abort_flag.store(true);
			m_cond.notify_all();
		}

		void push(T item)
		{

			// Acquire lock.  We will wait here if mutex locked
			std::unique_lock<std::mutex> lock(m_mutex);

			// Add item to the non-thread safe queue
			m_queue.push(item);
			m_enqued_items++;

			// Notify one waiting thread that something is  waiting
			m_cond.notify_one();
        	}

   	 	T pop() {

			// acquire the lock
			std::unique_lock<std::mutex> lock(m_mutex);

			// wait until queue is not empty
			m_cond.wait(lock,
			[this]() { return (!m_queue.empty() || is_aborted()); });

			if (abort_flag) {
				// std::cout << "pop() wait is aborted due to queue closure" << std::endl;
				return nullptr;
			}

			// retrieve item
			T item = m_queue.front();
			m_queue.pop();
			m_enqued_items--;
			// return item
			return item;
		}

		size_t size() {
			return m_enqued_items;
		}

		bool is_aborted() {
			return abort_flag.load();
		}

};
