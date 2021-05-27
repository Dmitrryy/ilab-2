/*************************************************************************************************
 *
 *   Renderable.hpp
 *
 *   Created by dmitry
 *   28.04.2021
 *
 ***/

#pragma once

#include <Engine/Types.hpp>


namespace ezg::engine
{

    class Renderable
    {
    protected:
        bool m_isUploaded = false;

        RenderMaterial m_renderMaterial = {};

        mutable size_t m_curInstanceId = 0;

        friend Engine;

    public:

        enum class Type
        {
            JustMesh
            , ReflectionMesh
            , Light
        };

    public:

        virtual ~Renderable() = default;

    public:

        bool isUploaded() const noexcept { return m_isUploaded; }

        virtual void draw(VkCommandBuffer cmdBuff
                          , const CameraView& camera
                          , RenderMaterial& last) const = 0;

        virtual glm::mat4 getModelMatrix() const { return glm::mat4{1.f}; }
        virtual glm::vec3 getColor() const { return glm::vec3(1.f); }

        virtual glm::vec3 getPosition() const { return glm::vec3(0.f); }
        virtual Type type() const noexcept = 0;
        virtual RenderMaterial::Type renderMaterialType() const noexcept = 0;
    };// class Renderable


    /// class Mesh
    /// class that defines the required fields and methods required for rendering
    class Mesh : virtual public Renderable
    {
    protected:
        AllocatedBuffer m_vertexBuffer = {};

        // so that the engine can access private fields
        // (these fields are not needed by an external user)
        friend Engine;
    public:

        std::vector< Vertex > vertices;

    public:

        virtual ~Mesh() = default;

    public:

        void draw(VkCommandBuffer cmdBuff
                  , const CameraView& camera
                  , RenderMaterial& last) const override;

        Type type() const noexcept override { return Type::JustMesh; }
        RenderMaterial::Type renderMaterialType() const noexcept override { return RenderMaterial::Type::DEFAULT; }

        bool load_from_obj(const std::string& filename);
    };//class Mesh



    class Mirror : public Mesh
    {
        AllocatedImage m_environmentCubeMap = {};
        AllocatedImage m_depthCubeImage = {};
        VkExtent2D m_extent = {2024, 2024};
        VkDescriptorSet m_descriptorSet = nullptr;
        VkImageView m_cubeImageView = nullptr;
        VkSampler m_cubeSampler = nullptr;

        VkRenderPass  m_environmentRenderPass = nullptr;

        std::array< frameBufferData, 6 > m_cubeFrameBuffers = {};

        friend Engine;
    private:

        void draw(VkCommandBuffer cmdBuff
                  , const CameraView& camera
                  , RenderMaterial& last) const override;

        void updateEnvironmentMap(VkCommandBuffer cmdBuff
                                  , const std::vector< Renderable* >& objects);

    public:

        float m_distanceToEye = 10.f;

    public:

        glm::vec3 getPosition() const override = 0;
        Type type() const noexcept override { return Type::ReflectionMesh; }
        RenderMaterial::Type
        renderMaterialType() const noexcept override { return RenderMaterial::Type::REFLECTION; }

    };//class Mirror




    struct Light : public Renderable
    {
        virtual float getFarPlane() const noexcept { return 50.f; }
        virtual float getEpsilon() const noexcept { return 0.05f; }
        virtual float getShadowOpacity() const noexcept { return 0.5f; }


        void draw(VkCommandBuffer cmdBuff
                          , const CameraView& camera
                          , RenderMaterial& last) const override { /* nop */ }

        Type type() const noexcept override { return Type::Light; }
        RenderMaterial::Type renderMaterialType() const noexcept override { return RenderMaterial::Type::DEFAULT_DEPTH_TEST_ONLY; }
    };//class Light

}//namespace ezg