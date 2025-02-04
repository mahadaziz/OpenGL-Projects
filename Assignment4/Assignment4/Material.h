#ifndef MATERIAL_H
#define MATERIAL_H

class HitResult;

class ReflectResult
{
public:
    Ray m_ray;
    Vector3D m_color;
};

class Material
{
public:
    Vector3D m_color;
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) = 0;
};


class Diffuse : public Material
{
public:
    Diffuse(const Vector3D& color)
    {
        m_color = color;
    };
    
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) override
    {
        ReflectResult res;
        //TODO: 4. generate one scattered ray and assign it to res.m_ray
        Vector3D vector = hit.m_hitNormal + Vector3D::random();
        Ray scattered_ray(hit.m_hitPos, vector);
        res.m_ray = scattered_ray;
        res.m_color = m_color;
        return res;
    }
};


class Specular : public Material
{
public:
    Specular(const Vector3D& color)
    {
        m_color = color;
    }
    
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) override
    {
        ReflectResult res;
        ////TODO: 5. generate one mirrored ray and assign it to res.m_ray
        Vector3D reflection = ray.m_direction - 2 * dot(ray.m_direction, hit.m_hitNormal) * hit.m_hitNormal;
        Ray scattered_ray(hit.m_hitPos, reflection);
        res.m_ray = scattered_ray;
        res.m_color = m_color;
        return res;
    }
};
#endif
