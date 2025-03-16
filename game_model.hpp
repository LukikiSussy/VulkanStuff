#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <string>

struct GameModelCreationInfo {
    VkDevice* device;
    VkPhysicalDevice* physicalDevice;
    VkSurfaceKHR* surface;
    VkQueue* graphicsQueue;
    VkCommandBuffer* setupCommandBuffer;
    VkFence* setupFence;
};

class GameModel {
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec3 normal;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();

        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && normal == other.normal && texCoord == other.texCoord;
        }
    };

    struct Builder {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices;  

        void loadModel(const std::string& filePath);
        void loadTexture(const std::string& filePath);
    };

    GameModel(const GameModelCreationInfo& creationInfo, const GameModel::Builder& builder);
    ~GameModel();

    GameModel(const GameModel&) = delete;
    GameModel& operator=(const GameModel&) = delete;

    static std::unique_ptr<GameModel> createModelFromFile(const GameModelCreationInfo& creationInfo, const std::string modelFilePath, const std::string textureFilePath);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

    void destroy();

private:
    VkDevice* device;
    VkPhysicalDevice* physicalDevice;
    VkSurfaceKHR* surface;
    VkQueue* graphicsQueue;
    VkCommandBuffer* setupCommandBuffer;
    VkFence* setupFence;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
    bool hasIndexBuffer = false;

    void createVertexBuffers(const std::vector<Vertex>& vertices);
    void createIndexBuffers(const std::vector<uint32_t>& indices);
};