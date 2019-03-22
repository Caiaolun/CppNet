#ifndef _CELLTimestamp_HPP__
#define _CELLTimestamp_HPP__
#include <chrono>
using namespace std::chrono;

class CELLTimestamp
{
public:
	CELLTimestamp();
	~CELLTimestamp();
	double GetElapsedSecond();
	double GetElapsedTimeInMilliSec();
	long long GetElapseTimeInMicroSec();
	void Update();
	
private:
	time_point<high_resolution_clock> _begin;
};

CELLTimestamp::CELLTimestamp()
{
	Update();
}

CELLTimestamp::~CELLTimestamp()
{
}

/*******************************************

* function introduced: Update time

* input parameter: NULL

* output parameter: NULL

* return: NULl

*******************************************/
void CELLTimestamp::Update()
{
	_begin = high_resolution_clock::now();
}

/*******************************************

* function introduced: Get Second

* input parameter: NULL

* output parameter: NULL

* return: Second

*******************************************/
double CELLTimestamp::GetElapsedSecond()
{
	return this->GetElapseTimeInMicroSec() * 0.000001;
}

/*******************************************

* function introduced: Get millisecond

* input parameter: NULL

* output parameter: NULL

* return: millisecond

*******************************************/
double CELLTimestamp::GetElapsedTimeInMilliSec()
{
	return this->GetElapseTimeInMicroSec() * 0.001;
}

/*******************************************

* function introduced: Get microsecond

* input parameter: NULL

* output parameter: NULL

* return: microsecond

*******************************************/
long long CELLTimestamp::GetElapseTimeInMicroSec()
{
	return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
}

#endif // !_CELLTimestamp_HPP__
