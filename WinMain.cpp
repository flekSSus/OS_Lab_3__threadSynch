#include <iostream>
#include <Windows.h>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>
#include <condition_variable>


std::mutex mtx;
std::condition_variable cv;
std::vector<bool> threadExited;
std::vector<bool> threadSleep;


void marker(size_t id, std::vector<int>& arr)
{
	srand(id);

	int size = arr.size();
	int flaggedEl = 0;

	while (true)
	{
	    std::unique_lock<std::mutex> ul(mtx);
	    if (threadExited[id] == 1)
	    {
	    	ul.unlock();
	    	break;
	    }

	cv.wait(ul, [&]{return threadSleep[id] == 0;});

		if (threadExited[id] == 1)
		{
			ul.unlock();
			cv.notify_all();
			break;
		}

		int rng = rand() % size;
		if (arr[rng] == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			arr[rng] = id + 1;
			
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			flaggedEl++;
		}
		else
		{
			std::cout << "\nThread id: " << id + 1 << "\n"<< "Count of colored: " << flaggedEl << "\n"<< "Impossible to color index: " << rng << "\n\n";
			threadSleep[id] = 1;
			flaggedEl = 0;
		}

		ul.unlock();
		cv.notify_all();
	}

	cv.notify_all();
}


int main()
{
	int size(0);
	int numOfThreads(0);

    std::cout<<"Enter array size: ";
    std::cin>>size;
    std::cout<<"Enter array size: ";
    std::cin>>numOfThreads;

	std::vector<int> arr(size, 0);

	std::vector<std::thread> threads;

	threadExited.resize(numOfThreads, 0);
	threadSleep.resize(numOfThreads, 0);

	for(int i(0);i<numOfThreads;++i)
		threads.push_back(std::thread(marker, i, std::ref(arr)));
	
	cv.notify_all();

	int numOfExitedT= 0;
	while (numOfExitedT < numOfThreads)
	{
		std::unique_lock<std::mutex> ul(mtx);

		cv.wait(ul, [&]	{return std::find(threadSleep.begin(), threadSleep.end(), 0) == threadSleep.end(); });

		if (numOfExitedT == numOfThreads - 1)
		{
			ul.unlock();
			break;
		}

		for (auto& i : arr)
			std::cout << i << " ";

		int stopId;
        std::cout<<"Enter id of thread in order to stop it from ( 1 to "<<numOfThreads<<") : "; 
        std::cin>>stopId;
        if(stopId <= 0 || stopId > max)
        {
            std::cout<<"Incorrect index\n";
        }

		for (size_t i = 0; i < size; i++)
		{
			if (arr[i] == stopId) 
				arr[i] = 0;
		}

		threadExited[stopId - 1] = true;
		threadSleep = threadExited;

		numOfExitedT++;
		ul.unlock();
		cv.notify_all();
	}

	for (size_t i = 0; i < numOfThreads; i++)
	{
		threads[i].detach();
	}

	std::cout << "\n\nFinal Array is:\n";
	for (auto i : arr)
		std::cout << i << " ";

}
