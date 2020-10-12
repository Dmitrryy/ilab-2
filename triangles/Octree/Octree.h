#pragma once

#include <vector>
#include <array>
#include <algorithm>

#include "../LAL/include/LALmath.h"

namespace la
{

	template < typename T >
	class Octree
	{
		std::vector<T> m_data;
		std::vector<T> m_outside_data;

		Square m_area;

		size_t m_deep;

		std::array< Octree< T >*, 8 > m_children;
		Octree<T>* m_parent;

		using IntersecC = std::vector< std::pair< T, T > >;

	public:

		Octree(Square _area, Octree* _parent = nullptr)
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
		size_t split  (size_t _deep);
		size_t msplit ();

		size_t size() const;

		bool haveChildren() const noexcept { return m_children.at(0) != nullptr; }

		size_t getDeep() const noexcept { return m_deep; }
		Square getArea() const noexcept { return m_area; }

		void add(const T& _elem);

		IntersecC getIntersections() const;


	private:

		bool _tryAdd_(const T& _elem, const Square& elem_area);
		bool _tryAndNotAdd_(const T& _elem, const Square& elem_area) const;
		std::vector< Square > _splitArea_() const;

		//O(N^2)
		IntersecC _intersecv_(const std::vector< T >& _vec) const;
		IntersecC _intersecvv_(const std::vector< T >& _lhs, const std::vector< T >& _rhs) const;
		IntersecC _intersecNv_(std::vector< std::vector< T > > _lhs, const std::vector< T >& _rhs)const ;

		IntersecC _getIntersecWith_(std::vector< std::vector< T > > _vec) const;

	};



	template <typename T>
	void Octree<T>::add(const T& _elem)
	{
		const auto elem_area = _elem.getArea();

		if (!_tryAdd_(_elem, elem_area))
		{
			//if (la::intersec(m_area, elem_area))
			//{
			//	m_data.push_back(_elem);
			//}
			//else
			//{
			//	m_outside_data.push_back(_elem);
			//	assert(m_parent == nullptr);
			//}
			throw std::logic_error(std::string("elem is outside: \n"));
		}
	}


	template <typename T>
	bool Octree<T>::_tryAdd_(const T& _elem, const Square& elem_area)
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
		}

		return res;
	}


	template <typename T>
	bool Octree<T>::_tryAndNotAdd_(const T& _elem, const Square& elem_area) const
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
		size_t res = m_data.size() + m_outside_data.size();
		for (int i = 0; i < 8; i++) {
			res += m_children.at(i).size();
		}
		return res;
	}


	template <typename T>
	size_t Octree<T>::split()
	{
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


		std::vector<Square> squares(_splitArea_());

		std::vector< std::vector< int > > new_chdata(8);
		std::vector< int > new_data;
		for (size_t i = 0, n = m_data.size(); i < n; i++)
		{
			T& tmp = m_data[i];
			const Square esq = tmp.getArea();
			uint16_t counter = 0u;
			int num_target = 0;
			for (int k = 0; k < 8; k++)
			{
				if (la::contein(squares[k], esq)) {
					num_target = k;
					counter++;
				}
				if (counter > 1) {
					break;
				}
			}

			if (counter == 1) {
				new_chdata[num_target].push_back(i);
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
				m_data.push_back(std::move(tmp_data[id_elem]));
			}

			for (int i = 0; i < 8; i++)
			{
				m_children[i] = new Octree(squares[i], this);
			}

			for (int i = 0; i < 8; i++)
			{
				for (const int& id_elem : new_chdata[i])
				{
					const bool rest = m_children[i]->_tryAdd_(tmp_data[id_elem], tmp_data[id_elem].getArea());
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
				IntersecC tmp = ch->_getIntersecWith_(tmp_data);
				res.insert(res.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
			}
		}

		return res;
	}


	template <typename T>
	std::vector< Square > Octree<T>::_splitArea_() const
	{
		const la::Vector3f center = (m_area.getA() + m_area.getB()) / 2.0;

		const double dx = m_area.getDX();
		const double dy = m_area.getDY();
		const double dz = m_area.getDZ();

		std::vector<Square> squares(8);
		squares[0] = Square(m_area.getA(), center);
		squares[1] = Square(m_area.getA() + Vector3f(dx, 0, 0), center);
		squares[2] = Square(m_area.getA() + Vector3f(dx, dy, 0), center);
		squares[3] = Square(m_area.getA() + Vector3f(0, dy, 0), center);
		squares[4] = Square(m_area.getB(), center);
		squares[5] = Square(m_area.getB() - Vector3f(dx, 0, 0), center);
		squares[6] = Square(m_area.getB() - Vector3f(dx, dy, 0), center);
		squares[7] = Square(m_area.getB() - Vector3f(0, dy, 0), center);

		return squares;
	}


	template <typename T>
	std::vector< std::pair< T, T > > Octree<T>::_intersecv_(const std::vector< T >& _vec) const
	{
		IntersecC res;

		for (size_t i = 0; i < _vec.size(); i++) {
			for (size_t k = i + 1; k < _vec.size(); k++)
			{
				if (intersec(_vec[i], _vec[k]))
				{
					res.push_back(std::make_pair(_vec[i], _vec[k]));
				}
			}
		}

		return res;
	}


	template <typename T>
	std::vector< std::pair< T, T > > Octree<T>::
		_intersecNv_(std::vector< std::vector< T > > _lhs, const std::vector< T >& _rhs) const
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
				if (intersec(_lhs[i], _rhs[k]))
				{
					res.push_back(std::make_pair(_lhs[i], _rhs[k]));
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
				IntersecC tmp = ch->_getIntersecWith_(_vec);
				res.insert(res.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
			}
		}

		return res;
	}

}//namespace la