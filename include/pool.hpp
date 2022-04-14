/*
    pool.hpp - Used for multithreading
    Part of mandelbrot viewer made in C++ using SFML library
    LICENSE: GNU Lesser General Public License v3.0
*/

#ifndef POOL_LOCK
#define POOL_LOCK

#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <condition_variable>

std::mutex queue_mutex;

template <typename F, typename T>
struct queue_data{
  F callee;
  T arg;
};

template <typename F, typename T>
class thread_pool{
private:
  std::condition_variable condition;
  std::queue<queue_data<F, T>> Queue;
  bool terminate_flag;
  bool terminate_job_flag;
  std::vector<std::thread> pool;
  bool destroyed;

  void init(){
    terminate_flag = false;
    terminate_job_flag = false;
    destroyed = false;
    int num = std::thread::hardware_concurrency();
    int i;
    for(i=0; i<num; i++){
      pool.emplace_back(std::thread(std::bind(&thread_pool::loop_function, &(*this) )));
    }
  }

public:
  thread_pool(){
    init();
  }

  ~thread_pool(){
    if(!destroyed){
      shutdown();
    }
  }

  void loop_function(){
    while(true){
      F Job;
      T arg;
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        condition.wait(lock, [this]() -> bool{
          return (!Queue.empty() || terminate_flag || terminate_job_flag);
        });
        if(Queue.empty() && terminate_flag){
          lock.unlock();
          //condition.notify_one();
          //std::cout << "Terminating...\n";
          return;
        }else if(terminate_job_flag){
          continue;
        }
        Job = Queue.front().callee;
        arg = Queue.front().arg;
        Queue.pop();
      }
      //condition.notify_one();
      Job(arg);
    }
  }

  void add_job(F new_job, const T & arg){
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      queue_data<F, T> temp;
      temp.callee = new_job;
      temp.arg = arg;
      Queue.push(temp);
    }
    condition.notify_one();
  }

  void shutdown(){
    std::unique_lock<std::mutex> lock(queue_mutex);
    terminate_flag = true;
    lock.unlock();
    condition.notify_all();

    for(std::thread & th : pool){
        th.join();
    }
    pool.clear();
    destroyed = true;
  }

  void kill_jobs(){
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      terminate_job_flag = true;
    }
    condition.notify_all();
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      terminate_job_flag = false;
    }

    while(Queue.size()){
      Queue.pop();
    }
  }
};

#endif