#pragma once
//#include<pthread.h> LINUX
#include<iostream>
#include<thread>
#include<List>
#include<mutex>
#include<vector>
class ThreadPool;

struct S_Worker {
	std::thread::id threadId; //线程id
	bool terminate; //是否结束该worker的标志
	int isWorking;  //该worker是否在工作
	ThreadPool* pool; //隶属于的线程池
};

struct S_Job {
	void(*func)(void* arg); // 任务函数
	void* userData;         // 函数参数
};

class ThreadPool {
public:
	ThreadPool();
	ThreadPool(int workNums,int maxJobNums = 10);
	~ThreadPool();

	// 添加任务api
	int PushJob(void(*func)(void* data), void* arg, int len);

private:
	// 向线程池中添加任务
	bool _addJob(S_Job* job);

	// 回调函数
	static void* _run(void* arg);
	void _threadLoop(void* arg);

private:
	std::list<S_Job*>m_JobList;
	int m_MaxJobs; // 最大任务数
	int m_ThreadNums; // 工作线程数
	int m_FreeThreadNums; // 空闲工作数
	std::vector<std::thread*>m_v;
	S_Worker* m_workers;

	std::condition_variable m_cv; // 线程条件等待
	std::mutex m_mutex;  // 任务锁
};