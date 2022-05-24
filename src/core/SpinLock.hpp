// Copyright (c) 2020 by Chrono
#ifndef _SPIN_LOCK_HPP
#define _SPIN_LOCK_HPP

#include "cpplang.hpp"

BEGIN_NAMESPACE(cpp_study)

// atomic spinlock with TAS
// 互斥锁、自旋锁、读写锁，都是属于悲观锁。多线程同时修改共享资源的概率比较高，于是很容易出现冲突，所以访问共享资源前，先要上锁。
// 乐观锁全程并没有加锁，所以它也叫无锁编程。版本号。
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
        // 直到成功获取 m_lock 的控制权，即 m_lock 为 false。
        for(;;) {
            // 若 原先未上锁, 则 加锁后直接返回
            if (!m_lock.test_and_set()) { return; }
            // 比较gentle的主动出让cpu, 让其他线程先执行。yield 时间不确定。部分操作系统对 yield 的实现可能是 sleep_for(0) 。
            std::this_thread::yield();
        }
    }
    // 将 m_lock 置为 true, 并返回 原始值取反
    bool try_lock() noexcept { return !m_lock.test_and_set(); }
    // 将 m_lock 置为 false
    void unlock()   noexcept { m_lock.clear(); }
};

// RAII for lock
// you can change it to a template class
class SpinLockGuard final {
  public:
    using this_type      = SpinLockGuard;
    using spin_lock_type = SpinLock;
  private:
    // 这里是引用类型?
    spin_lock_type& m_lock;

  public:
    explicit SpinLockGuard(spin_lock_type& lock) noexcept : m_lock(lock){
        m_lock.lock();
    }
   ~SpinLockGuard() noexcept {
       m_lock.unlock();
   }
    SpinLockGuard(const this_type&)            = delete;
    SpinLockGuard& operator=(const this_type&) = delete;
};

END_NAMESPACE(cpp_study)

#endif  //_SPIN_LOCK_HPP