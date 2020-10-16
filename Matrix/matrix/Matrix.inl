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
T Matrix<T>::determinanteSloww() const
{
	T result = T();

	if (m_nlines == m_ncolumns)
	{
		if (m_nlines == 2)
		{
			result = at(0, 0) * at(1, 1) - at(1, 0) * at(0, 1);
		}
		else
		{
			for (size_t k = 0; k < m_nlines; k++)
			{
				result += (submatrix(k, 0).determinanteSloww()) * ((k % 2) ? -1 : 1);
			}
		}
	}

	return result;
}


template <typename T>
Matrix<T> Matrix<T>::submatrix(size_t deleted_column, size_t deleted_line) const
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


#define orCase(a) \
case matrix::Order::a: \
res = #a; \
break;


std::string toString(typename Order order_)
{
	std::string res;

	switch (order_)
	{
		orCase(Row);
		orCase(Column);
	}

	return res;
}


std::ostream& operator << (std::ostream& stream_, matrix::Order order_)
{
	return stream_ << matrix::toString(order_);
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