#ifndef TASK_ACTIONTASK_H_
#define TASK_ACTIONTASK_H_

#include "SLO_Task.h"
#include "SLO_TaskPool.h"
#include <functional>

class ActionTask final : public Task, public std::enable_shared_from_this<ActionTask>
{
public:
	ActionTask();
	virtual ~ActionTask() override;

	virtual bool compute(uint32_t) override;
	virtual void stop(bool) override;
	virtual bool isFinished() override;
	virtual void reset(uint32_t) override;
	virtual void collect() override;

	static std::shared_ptr<ActionTask> get() {
		return pool_.get();
	}

	int getDuration() const {
		return duration_;
	}

	double getCurrValue() const {
		return currValue_;
	}

	double getStartValue() const {
		return startValue_;
	}

	uint32_t getStartTime() const {
		return startTime_;
	}

	void setAction(std::function<void(double)>);
	void setup(double, double, uint32_t, uint32_t);
	void setInterpolator(std::function<double(double)>);

private:
	static TaskPool<ActionTask> pool_;
	std::function<double(double)> interpolator_;
	bool finished_;
	uint32_t duration_;
	double currValue_;
	double startValue_;
	double stopValue_;
	double deltaValue_;
	double durationReciprocal_;
	uint32_t startTime_;
	std::function<void(double)> action_;
};

#endif // TASK_ACTIONTASK_H_
