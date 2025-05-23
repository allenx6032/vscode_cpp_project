#pragma once

#ifndef __ctpl_stl_thread_pool_H__
#define __ctpl_std_thread_pool_H__

#include<functional>
#include<thread>
#include<atomic>
#include<vector>
#include<memory>
#include<exception>
#include<future>
#include<mutex>
#include<queue>

namespace ctpl {

	using task_t = std::function<void(int)>;

	namespace detail {
		template<typename T>
		class Queue {
		public:
			// 常量引用 (T cosnt*常量指针)
			bool Push(T const& value) {
				// 可用 lock_guard代替(内存更小 相较不灵活)
				// std::lock_guard<std::mutex>(mMutex);
				std::unique_lock<std::mutex>lock(this->mMutex);
				this->mQueue.push(value);
				return true;
			}

			bool Pop(T& value) {
				std::unique_lock<std::mutex> lock(this->mMutex);
				if (mQueue.empty())return false;
				value = this->mQueue->front();
				this->mQueue.pop();
				return true;
			}

			bool Empty() {
				std::unique_lock<std::mutex>lock(this->mMutex);
				return mQueue.empty();
			}


		private:
			std::queue<T>mQueue;
			std::mutex mMutex;
		};
	}

	class thread_pool {
	public:
		thread_pool() { this->Init(); }
		thread_pool(int nThreads) { this->Init(); this->Resize(nThreads); }

		~thread_pool() {
			this->Stop(true);
		}

		int Size() { return static_cast<int>(this->mThreads.size()); }

		int N_Idle() { return mWaiting; }

		std::thread& GetThread(int i) { return *this->mThreads[i]; }

		void Resize(int nThreads) {
			if (!this->mIsStop && !this->mIsDone) {
				int oldThreadsNum = static_cast<int>(this->mThreads.size());
				if (oldThreadsNum <= nThreads) {
					this->mThreads.resize(nThreads);
					this->mFlags.resize(nThreads);

					for (int i = oldThreadsNum; i < nThreads; ++i) {
						this->mFlags[i] = std::make_shared<std::atomic<bool>>(false);
						this->SetThread(i);
					}
				}
				else {
					for (int i = oldThreadsNum - 1; i >= nThreads; --i) {
						*this->mFlags[i] = true;
						this->mThreads[i]->detach();
					}
					{
						std::unique_lock<std::mutex>lock(this->mMutex);
						this->mCv.notify_all();
					}
					this->mThreads.resize(nThreads);  // safe to delete because the threads are detached
					this->mThreads.resize(nThreads);  // safe to delete because the threads have copies of shared_ptr of the flags, not originals
				}
			}
		}

		void ClearQueue() {
			task_t* _f;
			while (this->mQ.Pop(_f))
				delete _f;
		}

		task_t Pop() {
			task_t* _f = nullptr;
			this->mQ.Pop(_f);
			std::unique_ptr<task_t>func(_f);
			task_t f;
			if (_f) {
				f = *_f;
			}
			return f;
		}

		void Stop(bool isWait = false) {
			if (!isWait) {
				if (this->mIsStop) return;

				this->mIsStop = true;
				for (int i = 0, n = this->Size(); i < n; ++i) {
					*this->mFlags[i] = true;
				}
				this->ClearQueue();
			}
			else {
				if (this->mIsDone || this->mIsStop) return;

				this->mIsDone = true;
			}

			{
				std::unique_lock<std::mutex> lock(this->mMutex);
				this->mCv.notify_all();
			}

			this->ClearQueue();
			this->mThreads.clear();
			this->mFlags.clear();
		}

		// 右值引用 完美转发 走移动构造 
		template<typename F,typename... Rest>
		auto Push(F&& f, Rest&&...rest) -> std::future<decltype(f(0, rest...))> {
			auto pck = std::make_shared<std::packaged_task><decltype(f(0, rest...))(int) >> (
				std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Rest>(rest)...)
				);
			auto _f = new task_t([pck](int id) {
				(*pck)(id);
				});
				
			this->mQ.Push(_f);
			std::unique_lock<std::mutex>lock (this->mMutex);
			this->mCv.notify_one();
			return pck->get_future();
		};

		template<typename F>
		auto Push(F&& f) -> std::future<decltype(f(0))> {
			auto pck = std::make_shared<std::packaged_task<decltype(f(0))(int)>>(std::forward<F>(f));
			auto _f = new task_t([pck](int id) {
				(*pck)(id);
				});
			this->mQ.Push(_f);
			std::unique_lock<std::mutex>lock(this->mMutex);
			this->mCv.notify_one();
			return pck->get_future();
		}

	private:
		thread_pool(const thread_pool&) = delete;
		thread_pool(thread_pool&&) = delete;
		thread_pool& operator=(const thread_pool&) = delete;
		thread_pool& operator=(thread_pool&&) = delete;

		void SetThread(int i) {
			std::shared_ptr<std::atomic<bool>> flag(this->mFlags[i]);

			auto f = [this, i, flag]() {
				std::atomic<bool>& _flag = *flag;
				task_t* _f;
				bool isPop = this->mQ.Pop(_f);

				while (true) {
					while (isPop) {
						std::unique_ptr<task_t>func(_f);
						(*_f)(i);

						if (_flag)return;
						else isPop = this->mQ.Pop(_f);
					}
					//queue is empty
					std::unique_lock<std::mutex> lock(this->mMutex);
					++this->mWaiting;
					this->mCv.wait(lock, [this, &_f, &isPop, &_flag]() {isPop = this->mQ.Pop(_f); return isPop || this->mIsDone || _flag; });
					--this->mWaiting;
					if (!isPop)
						return;
				}
			};
			this->mThreads[i].reset(new std::thread(f));
		}

		void Init() { this->mWaiting = 0; this->mIsStop = false; this->mIsDone = false; };

	private:
		std::vector<std::unique_ptr<std::thread>> mThreads;
		std::vector<std::shared_ptr<std::atomic<bool>>> mFlags;
		detail::Queue<task_t*> mQ;
		std::atomic<bool> mIsDone;
		std::atomic<bool> mIsStop;
		std::atomic<int> mWaiting;

		std::mutex mMutex;
		std::condition_variable mCv;
	};

}
#endif