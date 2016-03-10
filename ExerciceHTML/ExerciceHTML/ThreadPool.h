#include <iostream>
#include <vector>
#include <thread>
#include <deque>
#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable>
using namespace std;

class ThreadPool
{
	struct Task
	{
		virtual void execute() = 0;
		virtual ~Task() = default;
	};

	template<class F>
	class TaskMgr : public Task
	{
		F f;
	public:
		TaskMgr(F && f)
			: f{move(f)}
		{}

		void execute() { f(); }
	};

	std::mutex new_task_m;
	std::condition_variable new_task;
	mutable std::vector<std::thread> thr;
	std::deque<std::unique_ptr<Task>> taches;
	mutable std::mutex m;
	std::atomic<bool> closing_;

	bool stop_working() const
	{
		return closing() && empty();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> verrou{ m };
		return taches.empty();
	}

	std::unique_ptr<Task> next_task()
	{
		if (empty())
		{
			std::unique_lock<std::mutex> verrou{ new_task_m };
			new_task.wait(verrou, [&]() { return closing() || !empty(); });
		}
		std::lock_guard<std::mutex> verrou{ m };
		if (taches.empty())
			return nullptr;
		auto p = move(taches.front());
		taches.pop_front();
		return p;
	}

	bool closing() const
	{
		return closing_;
	}

public:
	ThreadPool(unsigned int n = std::thread::hardware_concurrency())
		: closing_{}
	{
		for (decltype(n) i = 0; i < n; ++i)
			thr.emplace_back([&]() {
			while (!stop_working())
			{
				auto quoi = next_task();
				if (quoi)
					quoi->execute();
			}
		});
	}

private:
	template <class F>
	static std::unique_ptr<Task> make_tache(F && f)
	{
		return std::unique_ptr<Task>(new TaskMgr<F>(forward<F>(f)));
	}

public:
	template <class F>
	void add_task(F && f)
	{
		std::lock(m, new_task_m);
		std::unique_lock<std::mutex> verrou_ntm{ new_task_m, std::adopt_lock };
		std::lock_guard<std::mutex> verrou_m{ m, std::adopt_lock };
		taches.emplace_back(make_tache(forward<F>(f)));
		new_task.notify_one();
	}

	void meurs()
	{
		closing_ = true;
		std::unique_lock<std::mutex> verrou{ new_task_m };
		new_task.notify_all();
	}

	void wait_end() const
	{
		for (auto & th : thr)
			if (th.joinable())
				th.join();
	}

	~ThreadPool()
	{
		meurs();
		wait_end();
	}
};