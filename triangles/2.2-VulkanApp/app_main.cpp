
#include "Application/VulkanDriver.h"

#include <cmath>

#include "../LAL/include/LALmath.h"
#include "../Octree/Octree.h"

#define IN_FROM_TESTS1 "../2.1-Intersec/tests/010rr.txt"

struct TRiangleColor
{
    TRiangleColor(const la::Triangle& tr)
        : triangle(tr)
    {}

    la::Triangle triangle;
    mutable la::Vector3f color = {0.f, 0.f, 0.5f};

    la::Rectangle3 getArea() const noexcept { return triangle.getArea(); }
    bool intersec(const TRiangleColor& _lhs) const 
    { 
        bool res = triangle.intersec(_lhs.triangle); 
        if (res)
        {
            color = _lhs.color = { 0.5f, 0.f, 0.f };
        }
        return res;
    }
};

//it looks like a Castile....
struct PointToRef
{
    PointToRef(TRiangleColor& data)
        : m_data(&data)
    {}

    TRiangleColor* m_data = nullptr;
    operator const TRiangleColor& () { return *m_data; }
    la::Rectangle3 getArea() const noexcept { return m_data->getArea(); }
    bool intersec(const PointToRef& lhs) const { return m_data->intersec(*lhs.m_data); }
};

glm::vec3 toGLM(const la::Vector3f& vec) { return { vec.x, vec.y, vec.z }; }
template <typename T>
std::vector< TRiangleColor > getData(T& _source);

int main()
{
#ifdef IN_FROM_TESTS1

    std::ifstream fin(IN_FROM_TESTS1);
    if (!fin.is_open()) {
        std::cerr << "cant opent test file: " << IN_FROM_TESTS1 << std::endl;
        exit(1);
    }
    auto triangles = getData(fin);

#else

    auto triangles = getData(std::cin);

#endif // IN_FROM_TESTS

    
    using std::min;
    using std::max;

    if (!triangles.empty())
    {
        la::Vector3f a = triangles.at(0).triangle.getA(), b = triangles.at(0).triangle.getB();

        for (size_t i = 0, n = triangles.size(); i < n; ++i)
        {
            const la::Triangle tr = triangles.at(i).triangle;
            a.x = min(a.x, min(tr.getA().x, min(tr.getB().x, tr.getC().x)));
            a.y = min(a.y, min(tr.getA().y, min(tr.getB().y, tr.getC().y)));
            a.z = min(a.z, min(tr.getA().z, min(tr.getB().z, tr.getC().z)));
            b.x = max(b.x, max(tr.getA().x, max(tr.getB().x, tr.getC().x)));
            b.y = max(b.y, max(tr.getA().y, max(tr.getB().y, tr.getC().y)));
            b.z = max(b.z, max(tr.getA().z, max(tr.getB().z, tr.getC().z)));
        }

        la::Octree< PointToRef > tree(la::Rectangle3(a, b));

        for (size_t i = 0, n = triangles.size(); i < n; ++i)
        {
            tree.add(PointToRef(triangles.at(i)));
        }

        tree.msplit();
        auto res = tree.getIntersections();
        //


        std::vector< vks::Vertex > vkData;
        vkData.resize(triangles.size() * 3);

        for (const auto& tr : triangles)
        {
            vks::Vertex vert = {};
            vert.color = toGLM(tr.color);
            vert.normal = glm::normalize(toGLM(tr.triangle.getPlane().getN()));
            vert.pos = toGLM(tr.triangle.getA());
            vkData.push_back(vert);

            vert.pos = toGLM(tr.triangle.getB());
            vkData.push_back(vert);

            vert.pos = toGLM(tr.triangle.getC());
            vkData.push_back(vert);
        }

        vks::VulkanDriver app("vulkan");

        app.setCameraView(vks::CameraView(toGLM(b) * 1.3f, toGLM((a + b) / 2.f) - toGLM(b) * 1.3f));
        float speed = (a - b).modul() / 8.f;
        app.setCameraSpeed((speed > 1.f) ? speed : 1.f);

        app.Init();

        app.updataVertexBuf(vkData);


        app.Run();
    }

    return 0;
}


template <typename T>
std::vector< TRiangleColor > getData(T& _source)
{
    int n = 0;
    _source >> n;
    std::vector< TRiangleColor > data;
    data.reserve(n);
    for (int i = 0; i < n; i++)
    {
        la::Vector3f a, b, c;
        _source >> a.x >> a.y >> a.z
            >> b.x >> b.y >> b.z
            >> c.x >> c.y >> c.z;
        data.emplace_back(la::Triangle(a, b, c));
    }

    return data;
}