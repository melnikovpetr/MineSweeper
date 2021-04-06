#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <functional>
#include <condition_variable>

class Timer
{
public:
	typedef std::chrono::milliseconds::rep Value;
	enum StopCond : size_t
	{
		ByTime,
		ByCount,
	};

public:
	Timer(StopCond stopCond = ByTime) : _stopCond(stopCond) {}

	void start(Value interval, Value startValue, Value finishValue, const std::function< void(Timer::Value) >& timeout);

	void stop();

	void wait();

	bool isRunning() const { return _isRunning; }

	~Timer() { stop(); }

private:
	std::mutex _mutex;
	std::condition_variable _cv;
	std::thread _thread;
	
	bool _isRunning{ false };
	StopCond _stopCond;

	Value _interval{};
	Value _value{};
	Value _finishValue{};

	std::function< void(Timer::Value) > _timeout;
};

#endif // TIMER_H
