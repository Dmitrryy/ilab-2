#pragma once

#include <unordered_map>
#include <list>
#include <set>
#include <iostream>
#include <functional>
#include <exception>


template<typename Val, typename Key>
class cash_LFU
{
	struct Elem
	{
		~Elem() {
			delete m_data;
		}
		Key                      m_key;
		Val* m_data = nullptr;
		std::function< Val* () > m_loader = []() { return nullptr; };
		size_t                   m_request = 0u;

		Key getKey() const { return m_key; }
	};

	using DataType = std::unordered_map< Key, Elem >;

	std::function<bool(const Elem* lhs, const Elem* rhs)> cmp =
		[](const Elem* lhs, const Elem* rhs) { return lhs->m_request < rhs->m_request; };
	using CashType = std::multiset< Elem*, decltype(cmp) >;


public:
	cash_LFU(size_t _max_size)
		: m_size(0u)
		, m_maxSize(_max_size)
		, m_cash(cmp)
	{}

public:

	void add(const Key& _id, std::function< Val* () > _loader);
	bool check(Key _id);
	Val& get(Key _id);

	bool full() { return m_size == m_maxSize; }
	size_t size() { return m_size; }
	size_t mSize() { return m_maxSize; }

private:

	void _remove_();
	void _up_(const Key& _id);
	void _load_(const Key& _id);

private:

	size_t m_size;
	size_t m_maxSize;

	CashType m_cash;
	std::unordered_map< Key, Elem > m_data;
};


template<typename Val, typename Key>
void cash_LFU<Val, Key>::add(const Key& _id, std::function< Val* () > _loader)
{
	m_data[_id].m_loader = _loader;
	m_data[_id].m_key = _id;
}

template<typename Val, typename Key>
bool cash_LFU<Val, Key>::check(Key _id)
{
	auto res = m_data.find(_id);
	return res != m_data.end();
}

template<typename Val, typename Key>
Val& cash_LFU<Val, Key>::get(Key _id)
{
	auto& res = m_data.at(_id);

	if (res.m_data == nullptr)
	{
		_load_(_id);
	}

	_up_(_id);
	return *(res.m_data);
}

template<typename Val, typename Key>
void cash_LFU<Val, Key>::_load_(const Key& _id)
{
	Elem& elem = m_data.at(_id);

	if (full())
	{
		_remove_();
	}

	if (elem.m_data == nullptr)
	{
		elem.m_data = elem.m_loader();
		if (elem.m_data == nullptr) {
			throw std::runtime_error("invalid loader (returned nullptr)");
		}
	}
	m_cash.insert(&elem);
	m_size++;
}

template<typename Val, typename Key>
void cash_LFU<Val, Key>::_up_(const Key& _id)
{
	Elem* elem = &m_data[_id];

	m_cash.erase(elem);
	elem->m_request++;
	m_cash.insert(elem);
}

template<typename Val, typename Key>
void cash_LFU<Val, Key>::_remove_()
{
	if (m_size == 0u) {
		throw std::logic_error("remove for empty data");
	}
	Key key = (*m_cash.begin())->getKey();
	Elem& elem = m_data[key];

	delete elem.m_data;
	elem.m_data = nullptr;

	m_cash.erase(m_cash.begin());

	m_size--;
}


