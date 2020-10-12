#include "test_gen.h"

la::Vector3f genPoint3f(la::Square _area)
{
	Random rx(_area.getA().x, _area.getB().x);
	Random ry(_area.getA().y, _area.getB().y);
	Random rz(_area.getA().z, _area.getB().z);

	return la::Vector3f( rx(), ry(), rz() );
}

std::vector<int> testGenTriangles(la::Square _area, size_t _num, size_t _rad /*= 5u*/)
{
	std::vector<la::Vector3f> vec1;
	vec1.reserve(_num);

	Random rx(_area.getA().x, _area.getB().x);
	Random ry(_area.getA().y, _area.getB().y);
	Random rz(_area.getA().z, _area.getB().z);

	for (size_t i = 0; i < _num; i++)
	{
		vec1.push_back(genPoint3f(_area));
	}

	std::vector<la::Vector3f> res;
	for (const auto& v : vec1)
	{
		const la::Square loc_area({ v.x + 5, v.y + 5, v.z + 5 }, { v.x - 5, v.y - 5, v.z - 5 });

		res.push_back(genPoint3f(loc_area));
		res.push_back(genPoint3f(loc_area));
		res.push_back(genPoint3f(loc_area));
	}

	std::vector<int> result;
	result.reserve(_num * 3u * 3u);

	for (const auto& v : res)
	{
		result.push_back(v.x);
		result.push_back(v.y);
		result.push_back(v.z);
	}

	return result;
}