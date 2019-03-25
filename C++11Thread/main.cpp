#include <thread>
#include <iostream>

#include "CELLTimestamp.hpp"
/*********************************************************************
* Part 1 thread
**********************************************************************/
void loop1(int T)
{
	for (int n = 0; n < 10; n++)
	{
		std::cout << "loop " << T << ": " << n << std::endl;
	}
}
void main1()
{

	
	std::thread t[4];
	for (int n = 0; n < 4; n++)
	{
		t[n] = std::thread(loop1, n);
	}
	for (int n = 0; n < 4; n++)
	{
		/**********************************
		* function introduced: detach()
		
		* 1. BranchThread and MainThread go run at the same time
		***********************************/
		//t[n].detach();


		/**********************************
		* function introduced: join()

		* 1. BranchThread and MainThread separate execution
		* 2. Each BranchThread go run at the same time
		***********************************/
		//t[n].join();
	}


	for (int n = 0; n < 10; n++)
	{
		std::cout << "loop Main: " << n << std::endl;
	}
	getchar();
}
/**********************************************************************
**********************************************************************/

/*********************************************************************
* Part 2 lock
**********************************************************************/
#include <mutex>

std::mutex m;
int sum = 0;
void loop2(int T)
{
	for (int n = 0; n < 8000000; n++)
	{
		/******************************************
		* function introduced: lock();unlock()

		* 1.  Each thread edit global variable, if it not lock, value is false
		*******************************************/
		//临界区域-开始
		m.lock();
		sum++;
		m.unlock();
		//临界区域-结束
	}
}
void main2()
{
	std::thread t[4];
	for (int n = 0; n < 4; n++)
	{
		t[n] = std::thread(loop2, n);
	}
	CELLTimestamp tTime;
	for (int n = 0; n < 4; n++)
	{
		t[n].join();
	}
	std::cout << "Time: " << tTime.GetElapsedTimeInMilliSec() << std::endl;
	std::cout << "BranchThread Sum: " << sum << std::endl;
	sum = 0;
	tTime.Update();
	for (int n = 0; n < 8000000; n++)
	{
		sum++;
	}
	std::cout << "Time: " << tTime.GetElapsedTimeInMilliSec() << std::endl;
	std::cout << "loop Main: " << sum << std::endl;
	getchar();
}
/**********************************************************************
**********************************************************************/


/*********************************************************************
* Part 3 auto lock
**********************************************************************/

void loop3(int T)
{
	for (int n = 0; n < 8000000; n++)
	{
		/******************************************
		* function introduced: std::lock_guard<std::mutex> lg(m);

		* 1. "std::lock_guard<std::mutex> lg(m)" begin lock
		* 2. Out " } " auto unlock
		*******************************************/
		std::lock_guard<std::mutex> lg(m);
		sum++;
	}
}
void main3()
{
	std::thread t[4];
	for (int n = 0; n < 4; n++)
	{
		t[n] = std::thread(loop3, n);
	}
	CELLTimestamp tTime;
	for (int n = 0; n < 4; n++)
	{
		t[n].join();
	}
	std::cout << "Time: " << tTime.GetElapsedTimeInMilliSec() << std::endl;
	std::cout << "BranchThread Sum: " << sum << std::endl;
	tTime.Update();
	sum = 0;
	for (int n = 0; n < 8000000; n++)
	{
		sum++;
	}
	std::cout << "Time: " << tTime.GetElapsedTimeInMilliSec() << std::endl;
	std::cout << "loop Main: " << sum << std::endl;
	getchar();
}
/**********************************************************************
**********************************************************************/

/*********************************************************************
* Part 4 atomic lock(atomic operation)
**********************************************************************/
#include <atomic>
/******************************************
* function introduced: atomic

* 1. atomic support only part type and easy operation
* For example: ++operation; --operation
*******************************************/
std::atomic<int> sumA = 0;
void loop4(int T)
{
	for (int n = 0; n < 8000000; n++)
	{
		sumA++;
	}
}
void main()
{
	std::thread t[4];
	for (int n = 0; n < 4; n++)
	{
		t[n] = std::thread(loop4, n);
	}
	CELLTimestamp tTime;
	for (int n = 0; n < 4; n++)
	{
		t[n].join();
	}
	std::cout << "Time: " << tTime.GetElapsedTimeInMilliSec() << std::endl;
	std::cout << "BranchThread Sum: " << sumA << std::endl;
	sumA = 0;
	tTime.Update();
	for (int n = 0; n < 8000000; n++)
	{
		sumA++;
	}
	std::cout << "Time: " << tTime.GetElapsedTimeInMilliSec() << std::endl;
	std::cout << "loop Main: " << sumA << std::endl;
	getchar();
}
/**********************************************************************
**********************************************************************/