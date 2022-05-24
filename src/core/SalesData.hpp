// Copyright (c) 2020 by Chrono

#ifndef _SALES_DATA_HPP
#define _SALES_DATA_HPP

#include <msgpack.hpp>
#include "cpplang.hpp"

#if MSGPACK_VERSION_MAJOR < 2
#   error "msgpack  is too old"
#endif

BEGIN_NAMESPACE(cpp_study)

class SalesData final {
  public:
    using this_type         = SalesData;
    using string_type       = std::string;
    using string_view_type  = const std::string&;
    using uint_type         = unsigned int;
    using currency_type     = double;
    STATIC_ASSERT(sizeof(uint_type) >= 4);
    STATIC_ASSERT(sizeof(currency_type) >= 4);

  public:
    SalesData(string_view_type id, uint_type s, currency_type r) noexcept
        : m_id(id), m_sold(s), m_revenue(r)
    {}

    SalesData(string_view_type id) noexcept
        : SalesData(id, 0, 0)  // 委托构造
    {}

  public:
#if 0
    SalesData(SalesData&& s) noexcept
        : m_id(std::move(s.m_id)),
          m_sold(std::move(s.m_sold)),
          m_revenue(std::move(s.m_revenue))
    {}

    SalesData& operator=(SalesData&& s) noexcept
    {
        m_id = std::move(s.m_id);
        m_sold = std::move(s.m_sold);
        m_revenue = std::move(s.m_revenue);

        return *this;
    }
#endif

    SalesData() = default;
   ~SalesData() = default;

    SalesData(const this_type&) = default;
    SalesData& operator=(const this_type&) = default;

    /*
     * 代码里显式声明了转移构造和转移赋值函数，
     * 这样，在放入容器的时候就避免了拷贝，能提高运行效率。
     */
    SalesData(this_type&& s) = default;
    SalesData& operator=(this_type&& s) = default;

  private:
    string_type m_id        = "";
    uint_type   m_sold      = 0;
    uint_type   m_revenue   = 0;

  public:
    /* 把成员属性依序放到 MSGPACK_DEFINE, 可以方便调用 msgpack:
     *   Book book1 = {1, "1984", {"a","b"}};
     *   Book book2;
     *
     *   msgpack::sbuffer sbuf;
     *   msgpack::pack(sbuf, book1);
     *
     *   auto obj = msgpack::unpack( sbuf.data(), sbuf.size()).get();
     *   obj.convert(book2);
     */
    MSGPACK_DEFINE(m_id, m_sold, m_revenue);
    // 通过 msgpack 来构造 实例
    explicit SalesData(const msgpack::sbuffer& sbuf) {
      auto obj = msgpack::unpack(
          sbuf.data(), sbuf.size()).get();
      obj.convert(*this);
    }

    [[nodiscard]] msgpack::sbuffer pack() const {
        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, *this);
        return sbuf;
    }


public:
    void inc_sold(uint_type s)        noexcept { m_sold += s; }
    void inc_revenue(currency_type r) noexcept { m_revenue += r; }
    /*
     * [[nodiscard]]: 提醒调用方，该需要记得接收该函数的返回值。
     * 比如下面的函数，明显调用就是为了获得 函数返回值 的，所以可以加一个这个标记。
     * 也常用与会返回重要错误码的函数，提醒使用方注意检查错误码。
     * 如果调用时，非要抛弃返回值，可以加 (void):
     *     (void)obj.id();
     */
    [[nodiscard]] string_view_type id()      const noexcept { return m_id; }
    [[nodiscard]] uint_type        sold()    const noexcept { return m_sold; }
    [[nodiscard]] currency_type    revenue() const noexcept { return m_revenue; }
    CPP_DEPRECATED
    [[nodiscard]] currency_type    average() const { return m_revenue / m_sold; }
};

END_NAMESPACE(cpp_study)

#endif  //_SALES_DATA_HPP
