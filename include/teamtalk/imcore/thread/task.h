/*
 Reviser: Polaris_hzn8
 Email: 3453851623@qq.com
 filename: Task.h
 Update Time: Tue 13 Jun 2023 09:01:22 CST
 brief:
    CTask抽象基类
    派生类必须实现run函数来定义具体任务逻辑
*/

#ifndef TEAMTALK_IMCORE_THREAD_TASK_H_
#define TEAMTALK_IMCORE_THREAD_TASK_H_

namespace teamtalk::imcore::thread {

class CTask {
 public:
  CTask() {}
  virtual ~CTask() {}

  virtual void run() = 0;

 private:
};

}  // namespace teamtalk::imcore::thread

#endif  // TEAMTALK_IMCORE_THREAD_TASK_H_
