#include <vector>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


int arr_size(0);
int threads_num(0);
std::vector<int> arr_id;

pthread_mutex_t mtx;
pthread_cond_t cond;

void* marker(void* _p_thr_index)
{
    pthread_cond_broadcast(&cond);

    int* p_thr_index(static_cast<int*>(_p_thr_index));
    srand(*p_thr_index);

    int value(0);
    int numOf_flagged(0);

    for(;;)
    {
        value=rand()%arr_size;
        if(!arr[value])
        {
            sleep(0.005);
            arr[value]=*p_thr_index;
            ++numOf_flagged;
            sleep(0.005);
        }
        else
        {
            std::cout<<"Thread Index:"<<*p_thr_index;
            std::cout<<"\nCount of flagged el:"<<numOf_flagged;
            std::cout<<"\nUnflagged index: "<<value;
        }
    }
    
}

int main()
{


    std::cout<<"Enter array size\n";
    std::cin>>arr_size;
    std::cout<<"Enter the number of Threads to run\n";
    std::cin>>threads_num;

    std::vector<int> arr(arr_size,0);

    pthread_mutex_init(&mtx,nullptr);
    pthread_cond_init(&cond,nullptr);

    arr_id.resize(threads_num,0);

    for(int i(0);i<threads_num;++i)
    {
        arr_id[i]=i;
        pthread_create(nullptr,nullptr,marker,arr_id+i);
    }
    
    pthread_cond_signal(&cond);
    
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);
}

