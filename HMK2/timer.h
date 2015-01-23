#ifndef TIMER_H
#define TIMER_H

#include <chrono>

template<typename F>
std::chrono::duration<double> timeFunc(F const & f)
{
	auto start = std::chrono::system_clock::now();
	f();
	auto stop = std::chrono::system_clock::now();
	return stop - start;
}

#endif