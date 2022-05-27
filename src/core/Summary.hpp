// Copyright (c) 2020 by Chrono

#ifndef _SUMMARY_HPP
#define _SUMMARY_HPP

#include "cpplang.hpp"
#include "SalesData.hpp"
#include "SpinLock.hpp"

BEGIN_NAMESPACE(cpp_study)

class Summary final{
  public:
    using this_type         = Summary;
    using sales_type        = SalesData;
    using lock_type         = SpinLock;
    using lock_guard_type   = SpinLockGuard;
    using string_type       = std::string;
    using map_type          = std::map<string_type, sales_type>;
                            //std::unordered_map<string_type, sales_type>;
    using minmax_sales_type = std::pair<string_type, string_type>;

  public:
    Summary() = default;
   ~Summary() = default;
    Summary(const this_type&) = delete;
    Summary& operator=(const this_type&) = delete;

  private:
    // 锁只是一个工具，不影响类实例对象的含义，所以可以配置成 mutable
    mutable lock_type   m_lock;
            map_type    m_sales;
  public:
    void add_sales(const sales_type& sale) {
        lock_guard_type guard(m_lock);
        const auto& id = sale.id();

        // not found
        if (m_sales.find(id) == m_sales.end()) {
            m_sales[id] = sale;
            return;
        }

        // found
        // you could use iter to optimize it
        m_sales[id].inc_sold(sale.sold());
        m_sales[id].inc_revenue(sale.revenue());
    }

    minmax_sales_type minmax_sales() const {
        // 直到锁可用，才执行后续方法
        // 出栈时，自动析构、解锁
        lock_guard_type guard(m_lock);

        // == return minmax_sales_type();
        if (m_sales.empty()) { return {};  }

        // algorithm
        auto ret = std::minmax_element(
            std::begin(m_sales), std::end(m_sales),
            [](const auto& a, const auto& b){ return a.second.sold() < b.second.sold(); }
            );

        // min max
        auto min_pos = ret.first;
        auto max_pos = ret.second;

        return {min_pos->second.id(), max_pos->second.id()};
    }
};

END_NAMESPACE(cpp_study)

#endif  //_SUMMARY_HPP

