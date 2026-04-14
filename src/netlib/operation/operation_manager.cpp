
/*
 Reviser: Polaris_hzn8
 Email: lch2022fox@163.com
 Github: https://github.com/Polaris-hzn8
 brief:
*/

#include <algorithm>
#include <imcore/operation/operation.h>
#include <imcore/operation/operation_manager.h>

namespace imcore {

namespace {
class LambdaOperation : public Operation {
 public:
  LambdaOperation(std::function<void()> operationRun) : m_operationRun(operationRun) {}
  virtual void processOpertion() { m_operationRun(); }
  virtual void release() { delete this; }

 private:
  std::function<void()> m_operationRun;
};
}  // namespace

OperationManager::~OperationManager() {
  try {
    shutdown();
  } catch (...) {
    LOG__(ERR, _T("OperationManager: shutdown throw unknown exception"));
    assert(FALSE);
  }
}

// 启动线程
IMCoreErrorCode OperationManager::startup() {
  if (m_operationThread.joinable())
    return IMCORE_OK;

  m_bContinue.store(true);
  m_operationThread = std::thread([this] {
    std::unique_lock<std::mutex> lck(m_cvMutex);
    while (m_bContinue.load()) {
      // if (m_vecRealtimeOperations.empty())
      //	m_cv.wait(lck);
      m_cv.wait(lck, [this]() { return !m_bContinue.load() || !m_vecRealtimeOperations.empty(); });

      if (!m_bContinue.load())
        break;

      Operation* pOperation = nullptr;
      {
        std::lock_guard<std::mutex> lock(m_mutexOperation);
        if (!m_vecRealtimeOperations.empty()) {
          pOperation = m_vecRealtimeOperations.front();
          m_vecRealtimeOperations.pop_front();
        }
      }
      if (pOperation) {
        try {
          pOperation->process();
        } catch (...) {
          LOG__(ERR, _T("OperationManager: operation threw exception"));
        }
        pOperation->release();
      }
    }
  });

  return IMCORE_OK;
}

// 安全停止
void OperationManager::shutdown(IN int seconds /*= 2000*/) {
  m_bContinue.store(false);
  m_cv.notify_all();

  // bug: join may be infinite waiting
  // if (m_operationThread.joinable())
  //	m_operationThread.join();

  auto start = std::chrono::steady_clock::now();
  while (m_operationThread.joinable()) {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    if (elapsed.count() > seconds * 1000) {
      LOG__(ERR, _T("OperationManager::shutdown timeout waiting for thread join"));
      break;
    }
    try {
      m_operationThread.join();
    } catch (...) {
      LOG__(ERR, _T("OperationManager::shutdown join failed"));
      break;
    }
  }

  std::lock_guard<std::mutex> locker(m_mutexOperation);
  // m_vecRealtimeOperations
  for (auto* pOperation : m_vecRealtimeOperations) {
    try {
      pOperation->release();
    } catch (...) {
      LOG__(ERR, _T("OperationManager: operation release threw exception"));
    }
  }
  m_vecRealtimeOperations.clear();

  // m_vecDelayOperations
  for (auto* pOperation : m_vecDelayOperations) {
    delete pOperation;
    pOperation = nullptr;
  }
  m_vecDelayOperations.clear();
}

IMCoreErrorCode OperationManager::startOperation(IN Operation* pOperation, Int32 delay) {
  if (!pOperation) {
    LOG__(ERR, _T("startOperation pOperation nullptr"));
    return IMCORE_ARGUMENT_ERROR;
  }

  if (delay > 0) {
    std::thread([this, pOperation, delay]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
      {
        std::lock_guard<std::mutex> locker(m_mutexOperation);
        m_vecRealtimeOperations.push_back(pOperation);
      }
      m_cv.notify_one();
    })
      .detach();
  } else {
    std::lock_guard<std::mutex> locker(m_mutexOperation);
    m_vecRealtimeOperations.push_back(pOperation);
    m_cv.notify_one();
  }

  return IMCORE_OK;
}

IMCoreErrorCode OperationManager::startOperationWithLambda(std::function<void()> operationRun,
                                                           Int32 delay,
                                                           std::string oper_name) {
  LambdaOperation* pLambdaOper = new LambdaOperation(operationRun);
  pLambdaOper->setName(oper_name);
  return startOperation(pLambdaOper, delay);
}

IMCoreErrorCode OperationManager::clearOperationByName(std::string oper_name) {
  std::lock_guard<std::mutex> locker(m_mutexOperation);
  auto iter = std::remove_if(m_vecRealtimeOperations.begin(), m_vecRealtimeOperations.end(), [&](Operation* pOper) {
    if (pOper->getName() == oper_name) {
      LOG__(APP, _T("clearOperationByName - %S"), oper_name.c_str());
      pOper->release();
      return true;
    }
    return false;
  });

  if (iter != m_vecRealtimeOperations.end()) {
    m_vecRealtimeOperations.erase(iter, m_vecRealtimeOperations.end());
  }

  return IMCORE_OK;
}

OperationManager* getOperationManager() {
  static OperationManager manager;
  return &manager;
}

}  // namespace imcore
