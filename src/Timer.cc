#include "Timer.h"
#include <cstdio>

void Timer::start_clock() {
	m_clock = std::chrono::steady_clock::now();
}

void Timer::set_time(float second) {
	m_second = second;
}

bool Timer::stop_search() const {
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	const auto during =  (double)std::chrono::duration_cast<std::chrono::milliseconds>(now - m_clock).count() / 1000.f;
	return buffer_time > (m_second - during);
}

float Timer::get_during() const {
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	const auto during =  (double)std::chrono::duration_cast<std::chrono::milliseconds>(now - m_clock).count() / 1000.f;
	return during;
}
