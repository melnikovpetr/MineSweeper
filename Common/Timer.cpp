#include "Timer.h"

#include <cassert>

void Timer::start(Value interval, Value startValue, Value finishValue, const std::function<void(Timer::Value)>& timeout)
{
	assert(interval > 0);

	stop();

	_interval = interval; _value = startValue; _finishValue = finishValue; _timeout = timeout;
	_thread = std::thread([this]() {
		std::unique_lock<std::mutex> lock(_mutex);
		auto desired = _value;
		auto startValue = _value;
		auto start = std::chrono::high_resolution_clock::now();
		while ((_finishValue > (_stopCond == ByTime ? _value : desired))
			&& (_cv.wait_for(lock, std::chrono::milliseconds(_interval + desired - _value)) == std::cv_status::timeout))
		{
			auto curr = std::chrono::high_resolution_clock::now();
			_value = startValue + std::chrono::duration_cast<std::chrono::milliseconds>(curr - start).count();
			desired += _interval;
			_timeout(_value);
		}
		_isRunning = false;
	});
	_isRunning = true;
}

void Timer::stop()
{
	if (_thread.joinable())
	{
		_cv.notify_all();
		_thread.join();
	}
}

void Timer::wait()
{
	if (_thread.joinable()) _thread.join();
}
