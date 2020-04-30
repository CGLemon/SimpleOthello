#ifndef TIMER_H_INCLUDE
#define TIMER_H_INCLUDE
#include <chrono>

class Timer {

public:
	Timer() : buffer_time(0.1f) {};
	void start_clock();
	void set_time(float second);
	bool stop_search() const;
	float get_during() const;
private:
	float m_second;
	float buffer_time;
	std::chrono::steady_clock::time_point m_clock;
	
};
#endif
