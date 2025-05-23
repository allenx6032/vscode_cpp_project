#include"SelfTP.h"

ThreadPool::ThreadPool()
{

}

ThreadPool::ThreadPool(int workNums, int maxJobNums):
	m_ThreadNums(workNums),m_FreeThreadNums(workNums), m_MaxJobs(maxJobNums)
{
	if (workNums < 1) {
		std::cout << "err worker num err" << std::endl;
	}

	m_workers = new S_Worker[workNums];

	for (int i = 0; i < workNums; i++) {
		m_workers[i].terminate = false;
		m_workers[i].pool = this;
		m_workers[i].isWorking = false;
		m_v.push_back(std::move(new std::thread([=]() {
			_run((void*)&(m_workers[i]));
			})));
	}
}

ThreadPool::~ThreadPool()
{
	for (int i = 0; i < m_ThreadNums; ++i) {
		m_workers[i].terminate = true;
	}
	m_cv.notify_all();
	for (auto i : m_v) {
		if (i->joinable()) {
			i->join();
		}
	}
	delete[] m_workers;
	for (auto i : m_v) {
		delete i;
	}
	m_v.clear();
}

int ThreadPool::PushJob(void(*func)(void* data), void* arg, int len)
{
	S_Job* job = new S_Job();
	if (job == nullptr)
		return -2;

	memset(job, 0, sizeof(S_Job));
	job->userData = new char[len];
	memcpy(job->userData, arg, len);
	job->func = func;

	_addJob(job);
	return 1;
}

bool ThreadPool::_addJob(S_Job* job)
{
	std::lock_guard<std::mutex>lock(m_mutex);
	if (m_JobList.size() >= m_MaxJobs) {
		return false;
	}

	m_JobList.push_back(job);
	m_cv.notify_one();
	return true;
}

void* ThreadPool::_run(void* arg)
{
	S_Worker* worker = (S_Worker*)arg;
	worker->pool->_threadLoop(arg);
	return nullptr;
}

void ThreadPool::_threadLoop(void* arg)
{
	S_Worker* worker = (S_Worker*)arg;
	while (true) {
		while (m_JobList.size() == 0) {
			if (worker->terminate)break;
			//条件变量只能用uniquelock
			std::unique_lock<std::mutex>lk(this->m_mutex);
			m_cv.wait(lk);
		}

		if (worker->terminate)break;

		if (m_JobList.empty())continue;
		// 任务不为空 走下来

		std::unique_lock<std::mutex>lk(this->m_mutex);
		S_Job* job = m_JobList.front();
		m_JobList.pop_front();
		lk.unlock();

		m_FreeThreadNums--;
		worker->isWorking = true;

		job->func(job->userData);
		worker->isWorking = false;

		delete job->userData;
		delete job;
	}
	delete worker;
	// 线程退出
}