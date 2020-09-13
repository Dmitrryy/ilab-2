#pragma once

#include <set>
#include <map>
#include <unordered_map>

#include <sstream>
#include <iomanip>
#include <iostream>


template <typename T>
class LFU_container
{
public:

    LFU_container(size_t _maxSize)
        : m_maxSize(_maxSize)
        , m_hit(0u)
        , m_miss(0u)
    { }

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
    std::pair<T, bool> lookup(const T& _val);

    //remove item(_val) from conteiner
    void erase(const T& _val);

    //true - _val is on the list (else false)
    bool contain(const T& _val) const { return m_keyToFreq.count(_val) == 1; }


    auto begin  () const noexcept { return m_freqToKey.begin(); }
    auto end    () const noexcept { return m_freqToKey.end(); }
    auto cbegin () const noexcept { return m_freqToKey.cbegin(); }
    auto cend   () const noexcept { return m_freqToKey.cend(); }
    
    auto rbegin () const noexcept { return m_freqToKey.rbegin(); }
    auto rend   () const noexcept { return m_freqToKey.rend(); }
    auto rcbegin() const noexcept { return m_freqToKey.crbegin(); }
    auto rcend  () const noexcept { return m_freqToKey.crend(); }


    bool empty   () const noexcept { return m_keyToFreq.empty(); }
    bool full    () const noexcept { return m_maxSize == m_keyToFreq.size(); }
    size_t size  () const noexcept { return m_keyToFreq.size(); }
    size_t mSize () const noexcept { return m_maxSize; }
    //
    size_t hit() const noexcept { return m_hit; }
    size_t miss() const noexcept { return m_miss; }

    void resize(size_t _size);

    void clear() noexcept { m_keyToFreq.clear(); m_freqToKey.clear(); }

    std::string debugString();


private:

    T _remove_old_();

    void _insert_(const T& _key);

private:

    std::multimap< size_t, T > m_freqToKey;
    std::unordered_map< T, typename std::multimap< size_t, T >::iterator > m_keyToFreq;

    std::unordered_map< T, size_t > m_history;

    size_t m_maxSize;

    size_t m_hit;
    size_t m_miss;
};

template <typename T>
std::pair<T, bool> LFU_container<T>::lookup(const T& _val)
{
    auto result = m_keyToFreq.find(_val);
    auto removed = std::make_pair(_val, false);

    if (result == m_keyToFreq.end()) 
    {
        if (m_maxSize != 0u) {
            if (full()) {
                removed.first = _remove_old_();
                removed.second = true;
            }
            m_history[_val] += 1;
            _insert_(_val);
        }
        m_miss++;
    }
    else 
    {
        erase(_val);
        m_history[_val] += 1;
        _insert_(_val);

        m_hit++;
    }

    return removed;
}

template <typename T>
void LFU_container<T>::resize(size_t _size)
{
    m_maxSize = _size;

    while(m_keyToFreq.size() > _size){
        _remove_old_();
    }
}

template <typename T>
void LFU_container<T>::erase(const T& _key)
{
    auto removed = m_keyToFreq.find(_key);

    if (removed != m_keyToFreq.end())
    {
        m_freqToKey.erase(removed->second);
        m_keyToFreq.erase(removed);
    }
}

template <typename T>
T LFU_container<T>::_remove_old_()
{
    T result;
    if (!empty()) 
    {
        auto removed_it = m_freqToKey.begin();
        result = removed_it->second;
        m_history[result] = removed_it->first;

        m_keyToFreq.erase(result);
        m_freqToKey.erase(removed_it);
    }

    return result;
}

template <typename T>
void LFU_container<T>::_insert_(const T& _key)
{
    size_t _freq = m_history[_key];

    auto it = m_freqToKey.insert(std::make_pair(_freq, _key));
    m_keyToFreq[_key] = it;
}

template <typename T>
std::string LFU_container<T>::debugString()
{
    using std::setw;

    std::ostringstream out;
    out << "max size:       " << m_maxSize << '\n'
        << "size freqToKey: " << m_freqToKey.size() << '\n'
        << "size keyToFreq: " << m_keyToFreq.size() << '\n'
        << "size history:   " << m_history.size() << '\n'
        << "full:           " << std::boolalpha << full() << '\n'
        << "empty:          " << empty() << '\n';
    
    if (!empty())
    {
        out << std::left << "content:\n";
        out << setw(10) << "key" << setw(10) << "frequency" << '\n';
        for (const auto& k : m_freqToKey){
            out << setw(10) << k.second << setw(10) << k.first << '\n';
        }
        out << '\n';
    }

    return out.str();
}