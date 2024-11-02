#include <vector>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


int arr_size(0);
int threads_num(0);
int index_to_complete(0);
std::vector<int> arr_id;
std::vector<int> arr;

pthread_mutex_t mtx;
pthread_cond_t cond_start_all;
pthread_cond_t cond_stop_this;
pthread_cond_t cond_main_answer;

void* marker(void* _p_thr_index)
{

    int thr_index(*(static_cast<int*>(_p_thr_index)));
    srand(thr_index);

    int value(0);
    int numOf_flagged(0);
    std::vector<int> indexes(arr_size,0);

    pthread_cond_wait(&cond_start_all,&mtx);
    pthread_mutex_lock(&mtx);
    for(;;)
    {
        value=rand()%arr_size;
        if(!arr[value])
        {
            usleep(5);
            indexes.push_back(value);
            arr[value]=thr_index;
            ++numOf_flagged;
            usleep(5);
            continue;
            pthread_mutex_unlock(&mtx);
        }
        else
        {
            std::cout<<"Thread Index:"<<thr_index;
            std::cout<<"\nCount of flagged el:"<<numOf_flagged;
            std::cout<<"\nUnflagged index: "<<value;
            pthread_cond_signal(&cond_stop_this);
            pthread_cond_wait(&cond_main_answer,&mtx);
            if(thr_index==index_to_complete)
            {
                for(int i(0),j(0);i<arr_size;++i)
                {
                    j=indexes[i];
                    arr[j]=0;
                    pthread_mutex_unlock(&mtx);
                }
                pthread_exit(nullptr);
            }
            continue;
        }

    }
    
}

int main()
{


    std::cout<<"Enter array size\n";
    std::cin>>arr_size;
    std::cout<<"Enter the number of Threads to run\n";
    std::cin>>threads_num;

    int counter_finishedT(0);

    pthread_mutex_init(&mtx,nullptr);
    pthread_cond_init(&cond_start_all,nullptr);
    pthread_cond_init(&cond_stop_this,nullptr);
    pthread_cond_init(&cond_main_answer,nullptr);

    arr_id.resize(threads_num,0);
    arr.resize(arr_size,0);

    for(int i(0);i<threads_num;++i)
    {
        auto b(arr_id.begin());
        std::advance(b,i);
        arr_id[i]=i;
        pthread_create(nullptr,nullptr,marker,(void*)b);
    }
    
    pthread_cond_broadcast(&cond_start_all);
    
    for(;counter_finishedT<threads_num;)
    {
        pthread_cond_wait(&cond_stop_this,&mtx);
        ++counter_finishedT;
    }

    std::cout<<"Array output: \n";
    for(int& i:arr)
        std::cout<<i<<' ';

    std::cout<<"Enter index[ from 0 to "<<threads_num-1<<"] of thread in order to complete thread: ";
    std::cin>>index_to_complete;

    pthread_cond_broadcast(&cond_main_answer);
        

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond_start_all);
    pthread_cond_destroy(&cond_stop_this);
    pthread_cond_destroy(&cond_main_answer);
}

