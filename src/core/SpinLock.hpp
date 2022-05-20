// Copyright (c) 2020 by Chrono
#ifndef _SPIN_LOCK_HPP
#define _SPIN_LOCK_HPP

#include "cpplang.hpp"

BEGIN_NAMESPACE(cpp_study)

// atomic spinlock with TAS
class SpinLock final{
  public:
    using this_type   = SpinLock;
    using atomic_type = std::atomic_flag;
  private:
    atomic_type m_lock {false};

  public:
    SpinLock() = default;
   ~SpinLock() = default;
    // 禁止拷贝构造
    SpinLock(const this_type&) = delete;
    SpinLock& operator=(const this_type&) = delete;

    void lock() noexcept {
        for(;;) {
            if (!m_lock.test_and_set()) { return; }
            std::this_thread::yield();
        }
    }
    bool try_lock() noexcept { return !m_lock.test_and_set(); }
    void unlock()   noexcept { m_lock.clear(); }
};

// RAII for lock
// you can change it to a template class
class SpinLockGuard final {
  public:
    using this_type      = SpinLockGuard;
    using spin_lock_type = SpinLock;
  private:
    spin_lock_type& m_lock;

  public:
    explicit SpinLockGuard(spin_lock_type& lock) noexcept : m_lock(lock){
        m_lock.lock();
    }
   ~SpinLockGuard() noexcept {
       m_lock.unlock();
   }

public:
    SpinLockGuard(const this_type&)            = delete;
    SpinLockGuard& operator=(const this_type&) = delete;
};

END_NAMESPACE(cpp_study)

#endif  //_SPIN_LOCK_HPP