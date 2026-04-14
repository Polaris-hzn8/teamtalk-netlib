#ifndef _REF_OBJECT_H_
#define _REF_OBJECT_H_

#include <atomic>
#include <string>

namespace imcore {

// 引用计数对象类
class CRefObject {
 public:
  CRefObject();
  virtual ~CRefObject();

  void AddRef();
  void ReleaseRef();

 private:
  std::atomic<int> m_refCount;
};

}  // namespace imcore

#endif  // _REF_OBJECT_H_