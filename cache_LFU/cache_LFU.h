#pragma once

#include <unordered_map>
#include <list>
#include <string>
#include <sstream>
#include <set>
#include <iostream>
#include <functional>
#include <exception>
#include <iomanip>

#include "LFU_conteiner.hpp"


//Val doesn't have to be an array(pointer to array)!!!
template<typename Val, typename Key>
class cache_LFU
{
	class Elem;
	
	using DataType = std::unordered_map< Key, Elem >;
	using CacheType = std::list< Elem* >;

public:

	using ValType = Val;

	//default constructor
	//   _max_size - max loaded elements in cash
	cache_LFU(size_t _max_size)
		: m_cache(_max_size)
		, m_hit(0u)
		, m_miss(0u)
	{
	}

public:

	//commits an item to the database
	//   _id - individual!! key fo item.
	//   _loader - loader function... 
	//             allocates memory for the element and returns a pointer.
	void add(const Key& _id, std::function< Val* () > _loader);

	//checks if an item is loaded with key _id.
	bool check(Key _id);

	bool existence(Key _id) const { return m_data.find(_id) != m_data.end(); }

	//returns a reference to the element with key _id.
	//if add is not called for _id, you get out_of_range exception.
	Val& get(Key _id);

	void reloadAll();

	///////////////////////get////////////////////////////////
	bool empty  () const noexcept { return m_cache.size() == 0u; }
	bool full   () const noexcept { return m_cache.size() == m_cache.mSize(); }
	size_t size () const noexcept { return m_cache.size(); }
	size_t mSize() const noexcept { return m_cache.mSize(); }
	//
	size_t hit() const noexcept { return m_hit; }
	size_t miss() const noexcept { return m_miss; }
	//////////////////////////////////////////////////////////

	std::string debugString();

private:

	//system function.
	//removes an element according to the LFU algorithm.
	//void _remove_();

	//system function.
	//fixes the use of an element with key _id.
	//void _up_    (const Key& _id);

	//system function.
	//loads an element into memory.
	//void _load_  (const Key& _id);

private:
	//CacheType m_cash;
	LFU_container< Key > m_cache;
	std::unordered_map< Key, Elem > m_data;

	size_t m_hit;
	size_t m_miss;
}; //class cash


///////////////////////////////////////////////////////////////////
//Elem class:
//made for the convenience of downloading and uploading data 
//and for calculating the frequency of use.
template<typename Val, typename Key>
class cache_LFU<Val, Key>::Elem
{
public:

	Elem            (const Elem& _that) = delete;   //not saported
	Elem& operator= (const Elem&)       = delete;   //not saported
	Elem            (Elem&& _that)      = delete;   //not saported
	Elem& operator= (Elem&&)            = delete;   //not saported

public:

	Elem() = default;
	Elem(Key _id, std::function< Val* () > _loader)
		: m_key(_id)
		, m_loader(_loader)
	{}

	~Elem() {
		delete m_data;
		m_data = nullptr;
	}

	Key    getKey    () const noexcept { return m_key; }
	size_t getRequest() const noexcept { return m_request; }
	Val*   getData   ()       noexcept { return m_data; }
	bool   isLoad    () const noexcept { return m_data != nullptr; }

	void setLoader(std::function< Val* () > _loader) { m_loader = _loader; }
	void setKey   (Key _id) noexcept                 { m_key = _id; }

	void incRequest() noexcept { m_request++; }
	void decRequest() noexcept { m_request--; }

	void load();
	void reload() noexcept { delete m_data; /**/ m_data = nullptr; }

private:

	Key                      m_key = Key();
	Val*                     m_data = nullptr;
	std::function< Val* () > m_loader;
	size_t                   m_request = 0u;
}; // class Elem

template<typename Val, typename Key>
void cache_LFU<Val, Key>::Elem::load() {
	if (m_data == nullptr)
	{
		m_data = m_loader();
		if (m_data == nullptr) {
			throw std::runtime_error("invalid loader (returned nullptr)");
		}
	}
}
///////////////////////////////////////////////////////////////////



template<typename Val, typename Key>
void cache_LFU<Val, Key>::add(const Key& _id, std::function< Val* () > _loader)
{
	Elem& elem = m_data[_id];
	elem.setLoader(_loader);
	elem.setKey(_id);
}

template<typename Val, typename Key>
bool cache_LFU<Val, Key>::check(Key _id)
{
	const Elem& res = m_data.at(_id);
	return res.isLoad();
}

template<typename Val, typename Key>
Val& cache_LFU<Val, Key>::get(Key _id)
{
	Elem& res = m_data.at(_id);

	if (!res.isLoad())
	{
		std::pair<Key, bool> removed = m_cache.lookup(_id);

		if (removed.second) {
			m_data.at(removed.first).reload();
		}
		res.load();
		m_miss++;
	}
	else {
		std::pair<Key, bool> removed = m_cache.lookup(_id);
		assert(!removed.second);
		m_hit++;
	}

	return *(res.getData());
}

template<typename Val, typename Key>
void cache_LFU<Val, Key>::reloadAll()
{
	m_cache.clear();

	for (auto& elem : m_data) {
		elem.second.reload();
	}
}

template<typename Val, typename Key>
std::string cache_LFU<Val, Key>::debugString()
{
	using std::setw;

	std::ostringstream out;
	out << std::left;
	out << "size data: " << m_data.size() << '\n'
		<< "size cash: " << m_cache.size() << '\n'
		<< "max size:  " << m_cache.mSize() << '\n'
		<< "hit:       " << m_hit << '\n'
		<< "miss:      " << m_miss << '\n';

	if (!m_cache.empty()) {
		out << "loaded:\n";
		out << setw(10) << "id" << setw(10) << "frequency" << setw(10) << "data" << '\n';
		for (const auto& elem : m_cache) {
			out << setw(10) << elem.second << setw(10) << elem.first 
				<< setw(15) << *(m_data[elem.second].getData()) << '\n';
		}
	}

	return out.str();
}