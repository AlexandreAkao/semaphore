#include <mutex>
#include <condition_variable>

#pragma once

using namespace std;

class Semaphore
{
public:
	Semaphore(int permits);
	Semaphore(int permits, int permitsInit);
	void down();
	void up();

private:
	int maxPermits;
	int currentPermits;
	mutex mtx;
	condition_variable cv;
};

