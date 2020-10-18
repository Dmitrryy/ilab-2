#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <sstream>

#include "../LAL/include/LALmath.h"

namespace la
{

	template < typename T >
	class Octree
	{
		std::vector<T> m_data;

		Rectangle3 m_area;

		mutable size_t m_deep;
		mutable bool m_deep_actual;

		mutable size_t m_size;
		mutable bool m_size_actual;

		std::array< Octree< T >*, 8 > m_children;
		Octree<T>* m_parent;

	public:

		using IntersecC = std::vector< std::pair< T, T > >;

		Octree            (const Octree& _that) = delete;   //not saported
		Octree& operator= (const Octree&)       = delete;   //not saported
		Octree            (Octree&& _that)      = delete;   //not saported
		Octree& operator= (Octree&&)            = delete;   //not saported


		Octree(Rectangle3 _area, Octree* _parent = nullptr) noexcept
			: m_data()
			, m_area(_area)
			, m_deep(0)
			, m_children{nullptr}
			, m_parent(_parent)
		{}

		~Octree() {
			for (int i = 0; i < 8; i++)
				delete m_children.at(i);
		}

		size_t split  ();
		//size_t split  (size_t _deep);
		size_t msplit ();

		size_t size() const;
		size_t deep() const noexcept;

		bool   haveChildren() const { return m_children.at(0) != nullptr; }

		Rectangle3 getArea() const noexcept { return m_area; }

		void   add(const T& _elem);

		IntersecC getIntersections() const;

		std::string dumpStr  () const;
		void        dumpLaTex(std::ostream& _out) const; //todo

	private:

		bool _tryAdd_(const T& _elem, const Rectangle3& elem_area);
		bool _tryAndNotAdd_(const T& _elem, const Rectangle3& elem_area) const;
		std::vector< Rectangle3 > _splitArea_() const;

		//O(N^2)
		IntersecC _intersecv_(const std::vector< T >& _vec) const;
		IntersecC _intersecvv_(const std::vector< T >& _lhs, const std::vector< T >& _rhs) const;
		IntersecC _intersecNv_(const std::vector< std::vector< T > >& _lhs, const std::vector< T >& _rhs)const ;

		IntersecC _getIntersecWith_(std::vector< std::vector< T > > _vec) const;

	};



	template <typename T>
	void Octree<T>::add(const T& _elem)
	{
		const auto elem_area = _elem.getArea();

		if (!_tryAdd_(_elem, elem_area))
		{
			throw std::logic_error(std::string("elem is outside: \n"));
		}
	}


	template <typename T>
	bool Octree<T>::_tryAdd_(const T& _elem, const Rectangle3& elem_area)
	{
		bool res = false;

		if (la::contein(m_area, elem_area))
		{
			if (haveChildren())
			{
				uint8_t counter = 0;
				Octree* tmp = nullptr;
				for (int i = 0; i < 8; i++)
				{
					assert(m_children.at(i) != nullptr);

					if (intersec(m_children.at(i)->getArea(), elem_area))
					{
						counter++;
						tmp = m_children.at(i);
					}
					if (counter > 1)
					{
						tmp = nullptr;
						break;
					}
				}

				if (tmp != nullptr)
				{
					const bool tr = tmp->_tryAdd_(_elem, elem_area);
					assert(tr);
				}
				else
				{
					m_data.push_back(_elem);
				}
			}
			else
			{
				m_data.push_back(_elem);
			}
			res = true;
			m_size_actual = false;
		}

		return res;
	}


	template <typename T>
	bool Octree<T>::_tryAndNotAdd_(const T& _elem, const Rectangle3& elem_area) const
	{
		bool res = false;

		if (la::contein(m_area, elem_area))
		{
			res = true;
		}

		return res;
	}


	template <typename T>
	size_t Octree<T>::size() const
	{
		if (!m_size_actual)
		{
			m_size = m_data.size();
			if (haveChildren()) {
				for (int i = 0; i < 8; i++) {
					m_size += m_children.at(i)->size();
				}
			}
			m_size_actual = true;
		}
		return m_size;
	}


	template <typename T>
	size_t Octree<T>::deep() const noexcept
	{
		if (!m_deep_actual)
		{
			m_deep = 0;
			if (haveChildren())
			{
				for (const auto& ch : m_children)
				{
					m_deep = std::max(ch->deep(), m_deep);
				}
			}
			m_deep++;
		} 
		return m_deep;
	}


	template <typename T>
	size_t Octree<T>::split()
	{
		m_deep_actual = false;

		if (haveChildren()) 
		{
			size_t res = 0;
			for (Octree* ch : m_children) {
				res = std::max(ch->split(), res);
			}
			return res;
		}
		if (m_data.size() < 2) {
			return 0;
		}

		std::vector<Rectangle3> squares(_splitArea_());

		std::vector< std::vector< int > > new_chdata(8);
		std::vector< int > new_data;
		for (size_t i = 0, n = m_data.size(); i < n; i++)
		{
			T& tmp = m_data.at(i);
			const Rectangle3 esq = tmp.getArea();
			uint16_t counter = 0u;
			int num_target = 0;
			for (int k = 0; k < 8; k++)
			{
				if (la::contein(squares.at(k), esq)) {
					num_target = k;
					counter++;
				}
				if (counter > 1) {
					break;
				}
			}

			if (counter == 1) {
				new_chdata.at(num_target).push_back(i);
			}
			else {
				new_data.push_back(i);
			}
			//assert(counter > 0);
		}
		
		if (new_data.size() <= m_data.size() / 2)
		{
			std::vector< T > tmp_data(std::move(m_data));
			m_data = std::vector< T >();
			m_data.reserve(new_data.size());

			for (const int& id_elem : new_data)
			{
				m_data.push_back(std::move(tmp_data.at(id_elem)));
			}

			for (int i = 0; i < 8; i++)
			{
				m_children.at(i) = new Octree(squares.at(i), this);
			}

			for (int i = 0; i < 8; i++)
			{
				for (const int& id_elem : new_chdata.at(i))
				{
					const bool rest = m_children.at(i)->_tryAdd_(tmp_data.at(id_elem), tmp_data.at(id_elem).getArea());
					assert(rest);
				}
			}

			return 1;
		}
		
		return 0;
	}

	template <typename T>
	size_t Octree<T>::msplit()
	{
		size_t res = 0, res2 = 0;
		while ((res2 = split()) != 0) {
			res += res2;
		}
		return res;
	}


	template <typename T>
	std::vector< std::pair< T, T > > Octree<T>::getIntersections() const 
	{
		IntersecC res = _intersecv_(m_data);

		if (haveChildren())
		{
			std::vector< std::vector< T > > tmp_data;
			tmp_data.emplace_back(m_data);
			for (const Octree<T>* ch : m_children)
			{
				assert(ch != nullptr);
				IntersecC tmp = ch->_getIntersecWith_(tmp_data);
				res.insert(res.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
			}
		}

		return res;
	}


	template <typename T>
	std::vector< Rectangle3 > Octree<T>::_splitArea_() const
	{
		const la::Vector3f center = (m_area.getA() + m_area.getB()) / 2.0;

		const double dx = m_area.getDX();
		const double dy = m_area.getDY();
		const double dz = m_area.getDZ();

		std::vector<Rectangle3> squares(8);
		squares.at(0) = Rectangle3(m_area.getA(), center);
		squares.at(1) = Rectangle3(m_area.getA() + Vector3f(dx, 0, 0), center);
		squares.at(2) = Rectangle3(m_area.getA() + Vector3f(dx, dy, 0), center);
		squares.at(3) = Rectangle3(m_area.getA() + Vector3f(0, dy, 0), center);
		squares.at(4) = Rectangle3(m_area.getB(), center);
		squares.at(5) = Rectangle3(m_area.getB() - Vector3f(dx, 0, 0), center);
		squares.at(6) = Rectangle3(m_area.getB() - Vector3f(dx, dy, 0), center);
		squares.at(7) = Rectangle3(m_area.getB() - Vector3f(0, dy, 0), center);

		return squares;
	}


	template <typename T>
	std::vector< std::pair< T, T > > Octree<T>::_intersecv_(const std::vector< T >& _vec) const
	{
		IntersecC res;

		for (size_t i = 0; i < _vec.size(); i++) {
			for (size_t k = i + 1; k < _vec.size(); k++)
			{
				//std::cout << _vec.at(i).id << ' ' << _vec.at(k).id << std::endl;
				if (intersec(_vec.at(i), _vec.at(k)))
				{
					res.push_back(std::make_pair(_vec.at(i), _vec.at(k)));
				}
			}
		}

		return res;
	}


	template <typename T>
	std::vector< std::pair< T, T > > Octree<T>::
		_intersecNv_(const std::vector< std::vector< T > >& _lhs, const std::vector< T >& _rhs) const
	{
		IntersecC res;

		for (const auto& vec : _lhs)
		{
			IntersecC tmp = _intersecvv_(vec, _rhs);
			res.insert(res.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
		}

		return res;
	}

	template <typename T>
	std::vector< std::pair< T, T > > Octree<T>::
		_intersecvv_(const std::vector< T >& _lhs, const std::vector< T >& _rhs) const
	{
		IntersecC res;

		for (size_t i = 0; i < _lhs.size(); i++) {
			for (size_t k = 0; k < _rhs.size(); k++) {
				//std::cout << _lhs.at(i).id << ' ' << _rhs.at(k).id << std::endl;
				if (intersec(_lhs.at(i), _rhs.at(k)))
				{
					res.push_back(std::make_pair(_lhs.at(i), _rhs.at(k)));
				}
			}
		}

		return res;
	}


	template <typename T>
	std::vector< std::pair< T, T > > Octree<T>::_getIntersecWith_(std::vector< std::vector< T > > _vec) const
	{
		IntersecC res = _intersecv_(m_data);

		IntersecC tmpr = _intersecNv_(_vec, m_data);
		res.insert(res.end(), std::make_move_iterator(tmpr.begin()), std::make_move_iterator(tmpr.end()));
		
		if (haveChildren())
		{
			_vec.emplace_back(m_data);
			for (const Octree* ch : m_children)
			{
				assert(ch != nullptr);
				IntersecC tmp = ch->_getIntersecWith_(_vec);
				res.insert(res.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
			}
		}

		return res;
	}


	template <typename T>
	std::string Octree<T>::dumpStr() const
	{
		std::ostringstream out;

		out << "Octree:" << '\n'
			<< "m_data.size() = " << m_data.size() << '\n'
			<< "all size      = " << size() << '\n'
			<< "max deep      = " << deep() << '\n'
			<< "have children ? " << haveChildren() << '\n'
			<< "parent ptr    = " << m_parent;

		return out.str();
	}

}//namespace la