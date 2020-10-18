#include "test_gen.h"

la::Vector3f genPoint3f(la::Rectangle3 _area)
{
	Random rx(_area.getA().x, _area.getB().x);
	Random ry(_area.getA().y, _area.getB().y);
	Random rz(_area.getA().z, _area.getB().z);

	return la::Vector3f( rx(), ry(), rz() );
}

la::Triangle genTriangle(la::Rectangle3 _area)
{
	return { genPoint3f(_area), genPoint3f(_area), genPoint3f(_area) };
}

std::vector<int> testGenTriangles(la::Rectangle3 _area, size_t _num, size_t _rad /*= 5u*/)
{
	std::vector<int> result;

	for (size_t k = 0; k < _num; )
	{
		const la::Vector3f center = genPoint3f(_area);
		const la::Rectangle3 loc_area({ center.x + _rad, center.y + _rad, center.z + _rad }, 
			{ center.x - _rad, center.y - _rad, center.z - _rad });
	
		const la::Triangle tr = genTriangle(loc_area);

		if (tr.valid()) {
			result.push_back(tr.getA().x);
			result.push_back(tr.getA().y);
			result.push_back(tr.getA().z);

			result.push_back(tr.getB().x);
			result.push_back(tr.getB().y);
			result.push_back(tr.getB().z);

			result.push_back(tr.getC().x);
			result.push_back(tr.getC().y);
			result.push_back(tr.getC().z);

			k++;
		}
	}

	return result;
}