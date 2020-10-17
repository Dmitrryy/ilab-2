
namespace matrix
{

	template <typename T>
	T& Matrix<T>::at(size_t y_, size_t x_)&
	{
		return const_cast<T&>(static_cast<const Matrix<T>*>(this)->at(y_, x_));
	}


	template <typename T>
	const T& Matrix<T>::at(size_t y_, size_t x_) const&
	{
		if (y_ >= m_nlines || x_ >= m_ncolumns) {
			throw std::out_of_range("Out of range valid value.");
		}

		if (m_order == Order::Row)
		{
			return *(m_data + y_ * m_ncolumns + x_);
		}
		else if (m_order == Order::Column)
		{
			return *(m_data + x_ * m_nlines + y_);
		}
		else { assert(0); }

	}


	template <typename T>
	void Matrix<T>::clear()
	{
		*this = std::move(Matrix<T>());
	}


	template <typename T>
	bool Matrix<T>::equal(const Matrix<T>& that_) const
	{
		bool result = true;

		if (m_ncolumns != that_.getNColumns() || m_nlines != that_.getNLines()) {
			return false;
		}

		if (m_order == Order::Row) {
			for (size_t i = 0; i < m_nlines; i++)
			{
				for (size_t k = 0; k < m_ncolumns; k++)
				{
					result = (at(i, k) == that_.at(i, k));
					if (!result) {
						break;
					}
				}
				if (!result) {
					break;
				}
			}
		}
		else if (m_order == Order::Column) {
			for (size_t i = 0; i < m_ncolumns; i++)
			{
				for (size_t k = 0; k < m_nlines; k++)
				{
					result = (at(k, i) == that_.at(k, i));
					if (!result) {
						break;
					}
				}
				if (!result) {
					break;
				}
			}
		}
		else { assert(0); }

		return result;
	}


	template <typename T>
	void Matrix<T>::setOrder(Order order_)
	{
		if (order_ != m_order)
		{
			Matrix<T> tmp(m_nlines, m_ncolumns, order_);
			copy__(tmp, *this, true);
			*this = std::move(tmp);
		}
	}


	template <typename T>
	void Matrix<T>::resize(size_t y_, size_t x_)
	{
		if (y_ == 0 || x_ == 0) {
			clear();
			return;
		}
		if (m_data == nullptr) {
			*this = std::move(Matrix<T>(y_, x_, m_order));
			return;
		}

		if (m_order == Order::Row && m_ncolumns == x_ && m_nlines >= y_)
		{
			m_nlines = y_;
		}
		else if (m_order == Order::Row && m_ncolumns == x_ && x_ * y_ <= m_capacity)
		{
			for (size_t i = 0; i < m_ncolumns; i++) {
				for (size_t k = m_nlines; k < y_; k++) {
					at(k, i) = T();
				}
			}
			m_nlines = y_;
		}
		else if (m_order == Order::Column && m_nlines == y_ && m_ncolumns >= x_)
		{
			m_ncolumns = x_;
		}
		else if (m_order == Order::Column && m_nlines == y_ && x_ * y_ <= m_capacity)
		{
			for (size_t i = 0; i < m_nlines; i++) {
				for (size_t k = m_ncolumns; k < x_; k++) {
					at(i, k) = T();
				}
			}
			m_ncolumns = x_;
		}
		else
		{
			Matrix<T> tmp(y_, x_, m_order);
			copy__(tmp, *this, true);
			*this = std::move(tmp);
		}
		m_size = x_ * y_;
	}


	template <typename T>
	T Matrix<T>::trace() const noexcept
	{
		if (m_nlines != m_ncolumns || m_nlines == 0) {
			return {};
		}

		T result = at(0, 0);

		for (size_t i = 1; i < m_nlines && result != T{}; i++)
		{
			result *= at(i, i);
		}

		return result;
	}


	template <typename T>
	T Matrix<T>::determinanteSloww() const
	{
		T result = T();

		if (m_nlines == m_ncolumns)
		{
			if (m_nlines == 1) {
				return at(0, 0);
			}
			else if (m_nlines == 2)
			{
				result = at(0, 0) * at(1, 1) - at(1, 0) * at(0, 1);
			}
			else
			{
				for (size_t k = 0; k < m_nlines; k++)
				{
					result += at(0, k) * (submatrix(0, k).determinanteSloww()) * ((k % 2) ? -1 : 1);
				}
			}
		}

		return result;
	}


	template <typename T>
	T Matrix<T>::determinante() const
	{
		if (m_nlines != m_ncolumns) {
			return {};
		}

		T one{};
		one++;
		T epsilon = one * 0.000001;

		Matrix<T> tr_mtr(m_nlines, m_ncolumns, Order::Row);
		copy__(tr_mtr, *this, true);

		const size_t N = m_nlines;
		bool minus = false;
		for (size_t cl = 0; cl < N; cl++)
		{
			//find a*0 != 0
			for (size_t k = cl; k < N; k++)
			{
				if (std::abs(tr_mtr.at(k, cl)) > epsilon) {
					if (k != cl) {
						tr_mtr.swopLines(cl, k);
						minus = !minus;
						//std::cout << tr_mtr.dumpStr() << std::endl;
					}
					break;
				}
			}
			//det = 0;
			//std::cout << tr_mtr.dumpStr() << std::endl;
			if (std::abs(tr_mtr.at(cl, cl)) < epsilon) {
				return T{};
			}

			const T& cur_elem = tr_mtr.at(cl, cl);
			for (size_t ln = cl + 1; ln < N; ln++)
			{
				T multiplier = tr_mtr.at(ln, cl) / cur_elem;
				for (size_t c = 0; c < N; c++)
				{
					tr_mtr.at(ln, c) -= tr_mtr.at(cl, c) * multiplier;
					//std::cout << tr_mtr.dumpStr() << std::endl;
				}
			}
		}

		if (minus) {
			return -tr_mtr.trace();
		}
		return tr_mtr.trace();
	}


	template <typename T>
	void Matrix<T>::swopLines(size_t l1_, size_t l2_)
	{
		for (size_t k = 0; k < m_ncolumns; k++) {
			std::swap(at(l1_, k), at(l2_, k));
		}
	}


	template <typename T>
	void Matrix<T>::swopColumns(size_t cl1_, size_t cl2_)
	{
		for (size_t k = 0; k < m_nlines; k++) {
			std::swap(at(k, cl1_), at(k, cl2_));
		}
	}

	template <typename T>
	Matrix<T> Matrix<T>::submatrix(size_t deleted_line, size_t deleted_column) const
	{
		if (m_ncolumns == 0 || m_nlines == 0) {
			return {};
		}

		Matrix<T> result(m_ncolumns - 1, m_nlines - 1, m_order);

		for (size_t i = 0, j = 0; i < m_nlines; i++)
		{
			if (i == deleted_line) {
				continue;
			}
			for (size_t k = 0, h = 0; k < m_ncolumns; k++)
			{
				if (k == deleted_column) {
					continue;
				}
				result.at(j, h) = at(i, k);
				h++;
			}
			j++;
		}

		return result;
	}


	template <typename T>
	void Matrix<T>::
		copy__(Matrix<T>& dest_, const Matrix<T>& source_, bool save_order_/* = false*/)
	{
		const size_t min_y = std::min(source_.getNLines(), dest_.getNLines());
		const size_t min_x = std::min(source_.getNColumns(), dest_.getNColumns());

		if (save_order_) {
			if (source_.getOrder() == Order::Row) {
				for (size_t i = 0; i < min_y; i++)
				{
					for (size_t k = 0; k < min_x; k++)
					{
						dest_.at(i, k) = source_.at(i, k);
					}
				}
			}
			else if (source_.getOrder() == Order::Column) {
				for (size_t i = 0; i < min_x; i++)
				{
					for (size_t k = 0; k < min_y; k++)
					{
						dest_.at(k, i) = source_.at(k, i);
					}
				}
			}
			else {
				assert(0);
			}
		}
		else
		{
			dest_.setOrder(source_.getOrder());
			copy__(dest_, source_, true);
		}
	}


	template <typename T>
	Matrix<T>& Matrix<T>::transpose()&
	{
		std::swap(m_ncolumns, m_nlines);
		if (m_order == Order::Column)
		{
			m_order = Order::Row;
		}
		else if (m_order == Order::Row)
		{
			m_order = Order::Column;
		}
		else { assert(0); }

		return *this;
	}


	template <typename T>
	Matrix<T>& Matrix<T>::negate()&
	{
		for (size_t k = 0; k < m_size; k++)
		{
			m_data[k] = -m_data[k];
		}
		return *this;
	}


	template <typename T>
	Matrix<T>& Matrix<T>::add(const Matrix& rhs_)&
	{
		if (m_order == Order::Row) {
			for (size_t i = 0; i < m_nlines; i++)
			{
				for (size_t k = 0; k < m_ncolumns; k++)
				{
					at(i, k) += rhs_.at(i, k);
				}
			}
		}
		else if (m_order == Order::Column) {
			for (size_t i = 0; i < m_ncolumns; i++)
			{
				for (size_t k = 0; k < m_nlines; k++)
				{
					at(k, i) += rhs_.at(k, i);
				}
			}
		}

		return *this;
	}


	template <typename T>
	Matrix<T>& Matrix<T>::sub(const Matrix& rhs_)&
	{
		if (m_order == Order::Row) {
			for (size_t i = 0; i < m_nlines; i++)
			{
				for (size_t k = 0; k < m_ncolumns; k++)
				{
					at(i, k) -= rhs_.at(i, k);
				}
			}
		}
		else if (m_order == Order::Column) {
			for (size_t i = 0; i < m_ncolumns; i++)
			{
				for (size_t k = 0; k < m_nlines; k++)
				{
					at(k, i) -= rhs_.at(k, i);
				}
			}
		}

		return *this;
	}


	template <typename T>
	Matrix<T>& Matrix<T>::mul(const T& num_)&
	{
		if (m_order == Order::Row) {
			for (size_t i = 0; i < m_nlines; i++)
			{
				for (size_t k = 0; k < m_ncolumns; k++)
				{
					at(i, k) *= num_;
				}
			}
		}
		else if (m_order == Order::Column) {
			for (size_t i = 0; i < m_ncolumns; i++)
			{
				for (size_t k = 0; k < m_nlines; k++)
				{
					at(k, i) *= num_;
				}
			}
		}

		return *this;
	}


	template <typename T>
	std::string Matrix<T>::dumpStr() const
	{
		std::ostringstream out;

		out << "size: " << m_nlines << " x " << m_ncolumns << '\n'
			<< "order " << toString(m_order) << '\n'
			<< "Data:" << '\n';

		for (size_t y = 0; y < m_nlines; y++)
		{
			out << "| ";
			for (size_t x = 0; x < m_ncolumns; x++)
			{
				out << std::setw(6) << at(y, x) << ' ';
			}
			out << "|\n";
		}

		return out.str();
	}

	template <typename T>
	bool Matrix<T>::operator == (const Matrix<T>& that_) const {
		return equal(that_);
	}

	template <typename T>
	bool Matrix<T>::operator != (const Matrix<T>& that_) const {
		return !equal(that_);
	}

	template <typename T>
	T& Matrix<T>::operator () (size_t _y, size_t _x)&
	{
		return const_cast<T&>(static_cast<const Matrix<T>*>(this)->operator()(_y, _x));
	}
	template <typename T>
	const T& Matrix<T>::operator () (size_t _y, size_t _x) const&
	{
		return at(_y, _x);
	}

	template <typename T>
	Matrix<T>& Matrix<T>::operator += (const Matrix& that_)& {
		return add(that_);
	}

	template <typename T>
	Matrix<T>& Matrix<T>::operator -= (const Matrix<T>& that_)& {
		return sub(that_);
	}

	template <typename T>
	Matrix<T>& Matrix<T>::operator *= (const T& num_)& {
		return mul(num_);
	}


	template <typename T>
	Matrix<T> operator + (const Matrix<T>& lhs_, const Matrix<T>& rhs_)
	{
		Matrix<T> res(lhs_);
		res += rhs_;
		return res;
	}
	template <typename T>
	Matrix<T> operator + (const Matrix<T>& lhs_, Matrix<T>&& rhs_)
	{
		Matrix<T> res(std::move(rhs_));
		res += lhs_;
		return res;
	}
	template <typename T>
	Matrix<T> operator + (Matrix<T>&& lhs_, const Matrix<T>& rhs_)
	{
		Matrix<T> res(std::move(lhs_));
		res += rhs_;
		return res;
	}
	template <typename T>
	Matrix<T> operator + (Matrix<T>&& lhs_, Matrix<T>&& rhs_)
	{
		Matrix<T> res(std::move(lhs_));
		res += rhs_;
		return res;
	}



	template <typename T>
	Matrix<T> operator - (const Matrix<T>& lhs_, const Matrix<T>& rhs_)
	{
		Matrix<T> res(lhs_);
		res -= rhs_;
		return res;
	}
	template <typename T>
	Matrix<T> operator - (const Matrix<T>& lhs_, Matrix<T>&& rhs_)
	{
		Matrix<T> res(std::move(rhs_));
		res -= lhs_;
		return res;
	}
	template <typename T>
	Matrix<T> operator - (Matrix<T>&& lhs_, const Matrix<T>& rhs_)
	{
		Matrix<T> res(std::move(lhs_));
		res -= rhs_;
		return res;
	}
	template <typename T>
	Matrix<T> operator - (Matrix<T>&& lhs_, Matrix<T>&& rhs_)
	{
		Matrix<T> res(std::move(lhs_));
		res -= rhs_;
		return res;
	}


	template <typename T>
	Matrix<T> operator * (const Matrix<T>& mtr_, const T& num_)
	{
		Matrix<T> res(mtr_);
		res.mul(num_);
		return res;
	}
	template <typename T>
	Matrix<T> operator * (Matrix<T>&& mtr_, const T& num_)
	{
		Matrix<T> res(std::move(mtr_));
		res.mul(num_);
		return res;
	}
	template <typename T>
	Matrix<T> operator * (const T& num_, const Matrix<T>& mtr_)
	{
		Matrix<T> res(mtr_);
		res.mul(num_);
		return res;
	}
	template <typename T>
	Matrix<T> operator * (const T& num_, Matrix<T>&& mtr_)
	{
		Matrix<T> res(std::move(mtr_));
		res.mul(num_);
		return res;
	}
}//namespace matrix