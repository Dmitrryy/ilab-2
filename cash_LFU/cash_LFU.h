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


//Val doesn't have to be an array(pointer to array)!!!
template<typename Val, typename Key>
class cash_LFU
{
	class Elem;
	
	using DataType = std::unordered_map< Key, Elem >;

	//comparator for multiset
	std::function<bool(const Elem* lhs, const Elem* rhs)> cmp =
		[](const Elem* lhs, const Elem* rhs) { return lhs->getRequest() < rhs->getRequest(); };
	using CashType = std::multiset< Elem*, decltype(cmp) >;

public:
	//defoult constructor
	//   _max_size - max loaded elements in cash
	cash_LFU(size_t _max_size)
		: m_size(0u)
		, m_maxSize(_max_size)
		, m_cash(cmp)
	{
		if (_max_size == 0u) {
			throw std::invalid_argument("max size of cash can't be zero");
		}
	}

public:

	//commits an item to the database
	//   _id - individual!! key fo item.
	//   _loader - loader function... 
	//             allocates memory for the element and returns a pointer.
	void add(const Key& _id, std::function< Val* () > _loader);

	//checks if an item is loaded with key _id.
	bool check(Key _id);

	//returns a reference to the element with key _id.
	//if add is not called for _id, you get out_of_range.
	Val& get(Key _id);

	void reloadAll();

	///////////////////////get////////////////////////////////
	bool empty  () const noexcept { return m_size == 0u; }
	bool full   () const noexcept { return m_size == m_maxSize; }
	size_t size () const noexcept { return m_size; }
	size_t mSize() const noexcept { return m_maxSize; }
	//////////////////////////////////////////////////////////

	std::string debugString();

private:

	//system function.
	//removes an element according to the LFU algorithm.
	void _remove_();

	//system function.
	//fixes the use of an element with key _id.
	void _up_    (const Key& _id);

	//system function.
	//loads an element into memory.
	void _load_  (const Key& _id);

private:
	CashType m_cash;
	DataType m_data;

	size_t m_size;
	size_t m_maxSize;
}; //class cash

///////////////////////////////////////////////////////////////////
//Elem class:
//made for the convenience of downloading and uploading data 
//and for calculating the frequency of use.
template<typename Val, typename Key>
class cash_LFU<Val, Key>::Elem
{
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

	Key    getKey    () const { return m_key; }
	size_t getRequest() const { return m_request; }
	Val*   getData   ()       { return m_data; }
	bool   isLoad    () const { return m_data != nullptr; }

	void setLoader(std::function< Val* () > _loader) { m_loader = _loader; }
	void setKey   (Key _id)                          { m_key = _id; }

	void incRequest() { m_request++; }
	void decRequest() { m_request--; }

	void load();
	void reload() {	delete m_data; /**/ m_data = nullptr; }

private:

	Key                      m_key = Key();
	Val*                     m_data = nullptr;
	std::function< Val* () > m_loader = []() { return nullptr; };
	size_t                   m_request = 0u;
}; // class Elem

template<typename Val, typename Key>
void cash_LFU<Val, Key>::Elem::load() {
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
void cash_LFU<Val, Key>::add(const Key& _id, std::function< Val* () > _loader)
{
	Elem& elem = m_data[_id];
	elem.setLoader(_loader);
	elem.setKey(_id);
}

template<typename Val, typename Key>
bool cash_LFU<Val, Key>::check(Key _id)
{
	Elem& res = m_data.at(_id);
	return res.isLoad();
}

template<typename Val, typename Key>
Val& cash_LFU<Val, Key>::get(Key _id)
{
	Elem& res = m_data.at(_id);

	if (!res.isLoad())
	{
		_load_(_id);
	}

	_up_(_id);
	return *(res.getData());
}

template<typename Val, typename Key>
void cash_LFU<Val, Key>::reloadAll()
{
	while (!empty()) {
		_remove_();
	}
}

template<typename Val, typename Key>
void cash_LFU<Val, Key>::_load_(const Key& _id)
{
	Elem& elem = m_data.at(_id);
	
	if (full())
	{
		_remove_();
	}

	elem.load();
	m_cash.insert(&elem);
	m_size++;
}

template<typename Val, typename Key>
void cash_LFU<Val, Key>::_up_(const Key& _id)
{
	Elem* elem = &m_data[_id];

	m_cash.erase(elem);
	elem->incRequest();
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

	elem.reload();

	m_cash.erase(m_cash.begin());

	m_size--;
}

template<typename Val, typename Key>
std::string cash_LFU<Val, Key>::debugString()
{
	using std::setw;

	std::ostringstream out;
	out << std::left;
	out << "size:      " << m_size << '\n'
		<< "size data: " << m_data.size() << '\n'
		<< "size cash: " << m_cash.size() << '\n'
		<< "max size:  " << m_maxSize << '\n';
	if (!m_cash.empty()) {
		out << "loaded: \n";
		out << setw(10) << "id" << setw(10) << "request" << setw(10) << "data" << '\n';
		for (const auto& elem : m_cash) {
			out << setw(10) << elem->getKey() << setw(10) << elem->getRequest() 
				<< setw(15) << *(elem->getData()) << '\n';
		}
	}

	return out.str();
}