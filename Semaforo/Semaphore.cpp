    #include "Semaphore.h"

Semaphore::Semaphore(int permits)
{
	this->maxPermits = permits;
	this->currentPermits = permits;
}

Semaphore::Semaphore(int permits, int permitsInit)
{
    this->maxPermits = permits;
    this->currentPermits = permitsInit;
}

void Semaphore::down()
{
    unique_lock<mutex> down(mtx);

    if (this->currentPermits == 0) {
        cv.wait(down);
    }

    this->currentPermits--;
}

void Semaphore::up()
{
    unique_lock<mutex> down(mtx);

    if (currentPermits < maxPermits) {
        this->currentPermits++;
        cv.notify_one();
    }
}

