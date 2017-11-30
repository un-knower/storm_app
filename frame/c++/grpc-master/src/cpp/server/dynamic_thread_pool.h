/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef GRPC_INTERNAL_CPP_DYNAMIC_THREAD_POOL_H
#define GRPC_INTERNAL_CPP_DYNAMIC_THREAD_POOL_H

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include <grpc++/support/config.h>

#include "src/cpp/server/thread_pool_interface.h"

namespace grpc {

class DynamicThreadPool final : public ThreadPoolInterface {
 public:
 	/**
   	构造函数接收一个参数reserve_threads，这个参数与线程池的缩容策略有关：它表示线程池最多只能有reserve_threads个空闲线程。
   	也就是说，如果线程池的空闲线程数量多于这个值，那么多出来的那些线程就会被系统回收。
   	**/
  explicit DynamicThreadPool(int reserve_threads);
  ~DynamicThreadPool();

  void Add(const std::function<void()>& callback) override; // 提交任务
 };

 private:
  class DynamicThread {
   public:
   	
    DynamicThread(DynamicThreadPool* pool);
    ~DynamicThread();

   private:
    DynamicThreadPool* pool_;
    std::unique_ptr<std::thread> thd_;//无法进行复制构造与赋值操作
    void ThreadFunc();
  };
  std::mutex mu_; // 互斥锁，保护数据成员
  std::condition_variable cv_;  // 条件变量
  std::condition_variable shutdown_cv_;//条件变量，与线程池析构相关
  bool shutdown_;// 线程池是否即将析构
  std::queue<std::function<void()>> callbacks_; // 任务队列 
  int reserve_threads_;// // 最大空闲线程数
  int nthreads_;// 当前线程数
  int threads_waiting_;// 空闲线程数
  std::list<DynamicThread*> dead_threads_; // 保存已经终止的线程 

  void ThreadFunc();
  static void ReapThreads(std::list<DynamicThread*>* tlist);
};

}  // namespace grpc

#endif  // GRPC_INTERNAL_CPP_DYNAMIC_THREAD_POOL_H
