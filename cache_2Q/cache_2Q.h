#pragma once

#include <sstream>
#include <functional>
#include <iomanip>
#include <cassert>

#include "LRU_conteiner.h"


template<typename Val, typename Key>
class cache_2Q
{
	class Elem;

public:

	using ValType = Val;

	//default constructor
	//   _max_size - max loaded elements in cash (min - 5)
	cache_2Q(size_t _max_size)
		: m_hit_count(0u)
		, m_miss_count(0u)
	{
		_distribution_memory_(_max_size);
	}

public:

	//commits an item to the database
	//   _id - individual!! key fo item.
	//   _loader - loader function... 
	//             allocates memory for the element and returns a pointer.
	void add(const Key& _id, std::function< Val* () > _loader);

	//checks if an item is loaded with key _id.
	bool check(Key _id) const;

	bool existence(Key _id) const { return m_data.find(_id) != m_data.end(); }

	//returns a reference to the element with key _id.
	//if add is not called for _id, you get out_of_range exception.
	Val& get(Key _id);

	void clear() noexcept;

	///////////////////////get////////////////////////////////
	bool empty  () const noexcept { return m_cacheIn.empty(); }
	bool full   () const noexcept { return m_cacheIn.full() && m_cacheOut.full() && m_cacheHot.full(); }
	size_t size () const noexcept { return m_cacheIn.size() + m_cacheOut.size() + m_cacheHot.size(); }
	size_t mSize() const noexcept { return m_cacheIn.mSize() + m_cacheOut.mSize() + m_cacheHot.mSize(); }
	//
	size_t hit  () const noexcept { return m_hit_count; }
	size_t miss () const noexcept { return m_miss_count; }
	//////////////////////////////////////////////////////////

	std::string debugString();

private:

	void _distribution_memory_(size_t _size) noexcept;

	//system function.
	//loads an element into memory.
	void _load_(const Key& _id);

private:

	LRU_container<Key> m_cacheIn;
	LRU_container<Key> m_cacheOut;
	LRU_container<Key> m_cacheHot;

	std::unordered_map< Key, Elem > m_data;

	size_t m_hit_count;
	size_t m_miss_count;
}; //class cash


///////////////////////////////////////////////////////////////////
//Elem class:
//made for the convenience of downloading and uploading data.
template<typename Val, typename Key>
class cache_2Q<Val, Key>::Elem
{
public:

	Elem(const Elem& _that) = delete;   //not saported
	Elem& operator= (const Elem&) = delete;   //not saported
	Elem(Elem&& _that) = delete;   //not saported
	Elem& operator= (Elem&&) = delete;   //not saported

public:

	Elem() noexcept
		: m_key()
		, m_data(nullptr)
		, m_loader()
	{}

	Elem(Key _id, std::function< Val* () > _loader)
		: m_key(_id)
		, m_loader(_loader)
	{}

	~Elem() {
		delete m_data;
		m_data = nullptr;
	}

	Key    getKey() const noexcept { return m_key; }
	Val*  getData()       noexcept { return m_data; }
	bool   isLoad() const noexcept { return m_data != nullptr; }

	void setLoader(std::function< Val* () > _loader) { m_loader = _loader; }
	void setKey(Key _id) noexcept { m_key = _id; }

	void load();
	void reload() noexcept { delete m_data; /**/ m_data = nullptr; }

private:

	Key                      m_key = Key();
	Val*                     m_data = nullptr;
	std::function< Val* () > m_loader;
}; // class Elem

template<typename Val, typename Key>
void cache_2Q<Val, Key>::Elem::load() {
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
void cache_2Q<Val, Key>::add(const Key& _id, std::function< Val* () > _loader)
{
	Elem& elem = m_data[_id];
	elem.setLoader(_loader);
	elem.setKey(_id);
}

template<typename Val, typename Key>
bool cache_2Q<Val, Key>::check(Key _id) const
{
	const Elem& res = m_data.at(_id);
	return res.isLoad();
}

template<typename Val, typename Key>
Val& cache_2Q<Val, Key>::get(Key _id)
{
	Elem& res = m_data.at(_id);

	if (!res.isLoad())
	{
		_load_(_id);
		m_miss_count++;
	}
	else 
	{
		if (m_cacheOut.contain(_id)) 
		{
			m_cacheOut.erase(_id);

			const auto removed = m_cacheHot.lookup(_id);
			if (removed.second) {
				m_data[removed.first].reload();
			}
		}
		else if (m_cacheIn.contain(_id)) {
			m_cacheIn.lookup(_id);
		}
		else if (m_cacheHot.contain(_id)) {
			m_cacheHot.lookup(_id);
		}
		else { assert(0); }

		m_hit_count++;
	}

	return *(res.getData());
}

template<typename Val, typename Key>
void cache_2Q<Val, Key>::clear() noexcept {
	m_cacheIn.clear();
	m_cacheOut.clear();
	m_cacheHot.clear();
	m_data.clear();
}

template<typename Val, typename Key>
void cache_2Q<Val, Key>::_load_(const Key& _id)
{
	Elem& elem = m_data.at(_id);
	elem.load();

	const auto removed = m_cacheIn.lookup(_id);
	
	if (removed.second) {
		const auto removed2 = m_cacheOut.lookup(removed.first);
		if (removed2.second) {
			m_data.at(removed2.first).reload();
		}
	}
}

template<typename Val, typename Key>
void cache_2Q<Val, Key>::_distribution_memory_(size_t _size) noexcept
{
	size_t nsi = 1u, nso = 2u, nsh = 1u;
	if (_size >= 5) {
		//20% for in box
		nsi = _size / 5;
		//20% for hot box
		nsh = _size / 5;
		//60% for out box
		nso = _size - nsi - nsh;
	}

	clear();

	m_cacheIn.resize(nsi);
	m_cacheHot.resize(nsh);
	m_cacheOut.resize(nso);
}

template<typename Val, typename Key>
std::string cache_2Q<Val, Key>::debugString()
{
	using std::setw;

	std::ostringstream out;
	out << std::left;
	out << "size:       " << size() << '\n'
		<< "hit count:  " << m_hit_count << '\n'
		<< "miss count: " << m_miss_count << '\n'
		<< "size data:  " << m_data.size() << '\n'
		<< "In size:    " << m_cacheIn.size() << " max: " << m_cacheIn.mSize() << '\n'
		<< "Out size:   " << m_cacheOut.size() << " max: " << m_cacheOut.mSize() << '\n'
		<< "Hot size:   " << m_cacheHot.size() << " max: " << m_cacheHot.mSize() << '\n';
		
	if (!m_cacheIn.empty()) {
		out << "loaded In:\n";
		out << setw(10) << "id" << setw(10) << "data" << '\n';
		for (const Key& k : m_cacheIn) {
			out << setw(10) << k << setw(10) << *(m_data[k].getData()) << '\n';
		}
	}
	if (!m_cacheOut.empty()) {
		out << "loaded Out:\n";
		out << setw(10) << "id" << setw(10) << "data" << '\n';
		for (const Key& k : m_cacheOut) {
			out << setw(10) << k << setw(10) << *(m_data[k].getData()) << '\n';
		}
	}
	if (!m_cacheHot.empty()) {
		out << "loaded Hot:\n";
		out << setw(10) << "id" << setw(10) << "data" << '\n';
		for (const Key& k : m_cacheHot) {
			out << setw(10) << k << setw(10) << *(m_data[k].getData()) << '\n';
		}
	}

	return out.str();
}