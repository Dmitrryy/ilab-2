#pragma once

#include <list>
#include <unordered_map>


template <typename TVal>
class LRU_container
{
public:

    //default constructor
    //   _size - max size of container
    LRU_container(size_t _size = 0u) noexcept
        : m_max_size(_size)
    {}

    //lookup:
    //Updates the position of _val.
    //If contain(_val) == true, it places _val in the head of the container.
    //else puts _val on the head and remove the object at the end of the list (if the list is full).
    //
    //parametrs:
    //    _val - object of interest.
    //return:
    //    returns a pair.
    //    bool second - has the object been deleted?
    //    TVal first  - deleted object (otherwise empty).
    std::pair<TVal, bool> lookup(const TVal& _val);

    //remove item(_val) from list
    void erase(const TVal& _val) { m_list.erase(m_map.at(_val)); /**/ m_map.erase(_val); }

    //true - _val is on the list (else false)
    bool contain(const TVal& _val) const { return m_map.count(_val) == 1; }


    const TVal& front () const noexcept { return m_list.front(); }
    const TVal& back  () const noexcept { return m_list.back(); }


    auto begin  () const noexcept { return m_list.begin(); }
    auto end    () const noexcept { return m_list.end(); }
    auto cbegin () const noexcept { return m_list.cbegin(); }
    auto cend   () const noexcept { return m_list.cend(); }
    
    auto rbegin () const noexcept { return m_list.rbegin(); }
    auto rend   () const noexcept { return m_list.rend(); }
    auto rcbegin() const noexcept { return m_list.crbegin(); }
    auto rcend  () const noexcept { return m_list.crend(); }


    bool empty   () const noexcept { return m_map.empty(); }
    bool full    () const noexcept { return m_max_size == m_map.size(); }
    size_t size  () const noexcept { return m_map.size(); }
    size_t mSize () const noexcept { return m_max_size; }

    void resize(size_t _size) noexcept;

    void clear() noexcept { m_map.clear(); m_list.clear(); }


    void push_front(const TVal& _val);
    void push_back(const TVal& _val);

    void pop_front() noexcept;
    void pop_back() noexcept;

private:

    std::unordered_map<TVal, typename std::list<TVal>::iterator> m_map;
    std::list<TVal> m_list;

    size_t m_max_size;
};


template <typename TVal>
std::pair<TVal, bool> LRU_container<TVal>::lookup(const TVal& _val)
{
    std::pair <TVal, bool> result = std::make_pair(_val, false);
    auto res = m_map.find(_val);

    if (res != m_map.end()) {
        m_list.push_front(_val);

        m_list.erase(res->second);
        res->second = m_list.begin();
    }
    else if (m_max_size != 0u) {
        if (m_map.size() == m_max_size) {
            result = std::make_pair(back(), true);
            pop_back();
        }

        push_front(_val);
    }

    return result;
}

template <typename TVal>
void LRU_container<TVal>::resize(size_t _size) noexcept 
{
    m_max_size = _size;

    while (m_map.size() > _size) {
        pop_back();
    }
}

template <typename TVal>
void LRU_container<TVal>::push_front(const TVal& _val) 
{
    m_list.push_front(_val);
    m_map[_val] = m_list.begin();
}
template <typename TVal>
void LRU_container<TVal>::push_back(const TVal& _val) 
{
    m_list.push_front(_val);
    m_map[_val] = std::prev(m_list.end());
}

template <typename TVal>
void LRU_container<TVal>::pop_back() noexcept 
{
    m_map.erase(m_list.back());
    m_list.pop_back();
}
template <typename TVal>
void LRU_container<TVal>::pop_front() noexcept 
{
    m_map.erase(m_list.front());
    m_list.pop_front();
}