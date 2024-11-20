#include<iostream>
#include<pthread.h>
#include<vector>
#include<stdlib.h>
#include<unistd.h>

int arr_size(0);
int threads_num(0);
int index_to_terminate(0);
int counter_startedT(0);
std::vector<int> arr;
std::vector<pthread_t> threads;

pthread_mutex_t mtx1;
pthread_mutex_t mtx2;
pthread_mutex_t mtx_stream;
pthread_cond_t cond_start_all;
pthread_cond_t cond_stop_this;
pthread_cond_t cond_resume;


void* marker(void * p_thr_index)
{
    
    int* thr_index(static_cast<int*>(p_thr_index));
    int value(0);
    int num_of_flagged(0);
    std::vector<int> indexes;

    srand(*thr_index);
    
    pthread_mutex_lock(&mtx1);
    pthread_cond_wait(&cond_start_all,&mtx1);
    ++counter_startedT;

    for(;;)
    {
        value=rand()%arr_size;
        if(!arr[value])
        {
            usleep(5000);
            indexes.push_back(value);
            arr[value]=*thr_index;
            ++num_of_flagged;
            usleep(5000);
            pthread_mutex_unlock(&mtx1);
        }
        else
        {
            usleep(1000000);
            pthread_mutex_lock(&mtx_stream);
            std::cout<<"\nThread Index:"<<*thr_index;
            std::cout<<"\nCount of flagged el: "<<num_of_flagged;
            std::cout<<"\nUnable to flag the index: "<<value<<'\n';
            pthread_mutex_unlock(&mtx_stream);
            pthread_mutex_lock(&mtx2);
            pthread_cond_signal(&cond_stop_this);
            pthread_cond_wait(&cond_resume,&mtx1);
            
            if(*thr_index==index_to_terminate)
            {
                for(int i(0);i<arr_size;++i)
                {
                    arr[indexes[i]]=0;
                    pthread_mutex_unlock(&mtx1);
                }
                pthread_exit(nullptr);
            }
            pthread_mutex_unlock(&mtx1);
            pthread_exit(nullptr);
        }
    }

    
}


int main()
{
    std::cout<<"Enter the size of array: ";
    std::cin>>arr_size;
    
    std::cout<<"Enter the number of the threads: "; 
    std::cin>>threads_num;

    arr.resize(arr_size,0);
    threads.resize(threads_num,0);
    int counter_finishedT(0);

    pthread_mutex_init(&mtx1,nullptr);
    pthread_mutex_init(&mtx2,nullptr);
    pthread_mutex_init(&mtx_stream,nullptr);
    pthread_cond_init(&cond_start_all,nullptr);
    pthread_cond_init(&cond_stop_this,nullptr);
    pthread_cond_init(&cond_resume,nullptr);

    for(int i(1);i<threads_num+1;++i)
    {
        int *index=new int(i);
        void* ptr_index(index);
        pthread_create(&threads[i],nullptr,&marker,ptr_index);
    }

    while(counter_startedT!=threads_num)
        pthread_cond_broadcast(&cond_start_all);
    pthread_mutex_lock(&mtx2);

    for(;counter_finishedT!=threads_num;)
    {
        pthread_mutex_unlock(&mtx2);
        pthread_cond_wait(&cond_stop_this,&mtx1);
        ++counter_finishedT;
    }

    pthread_mutex_lock(&mtx_stream);
    std::cout<<"\nArray output: \n";
    for(int& i:arr)
        std::cout<<i<<' ';
    std::cout<<'\n';
    pthread_mutex_unlock(&mtx_stream);
   
    pthread_mutex_lock(&mtx_stream);
    std::cout<<"Enter index [ from 1 to "<<threads_num<<"] of thread in order to complete thread: ";
    pthread_mutex_unlock(&mtx_stream);
    std::cin>>index_to_terminate;

    pthread_cond_broadcast(&cond_resume);
    pthread_mutex_unlock(&mtx1);

    pthread_mutex_lock(&mtx_stream);
    std::cout<<"Thread "<<index_to_terminate<<" was terminated\n";
    pthread_mutex_unlock(&mtx_stream);

    usleep(1000000);
    pthread_mutex_lock(&mtx_stream);
    std::cout<<"\nArray output after termination: \n";
    for(int& i:arr)
        std::cout<<i<<' ';
    std::cout<<'\n';
    pthread_mutex_unlock(&mtx_stream);

    pthread_mutex_destroy(&mtx1);
    pthread_mutex_destroy(&mtx2);
    pthread_mutex_destroy(&mtx_stream);
    pthread_cond_destroy(&cond_start_all);
    pthread_cond_destroy(&cond_stop_this);
    pthread_cond_destroy(&cond_resume);

}

