#include "game_model.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "utils.hpp"
#include "commandBuffer_utils.hpp"
#include <array>
#include <iostream>


namespace std {
    template <>
    struct hash<GameModel::Vertex> {
        size_t operator()(GameModel::Vertex const& vertex) const {
            size_t seed = 0;
            hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.texCoord);
            return seed;
        }
    };
}

void GameModel::destroy() {
    this->~GameModel();
}

GameModel::GameModel(const GameModelCreationInfo& creationInfo, const GameModel::Builder& builder) : 
    device{ creationInfo.device }, physicalDevice{ creationInfo.physicalDevice }, surface{ creationInfo.surface }, graphicsQueue{ creationInfo.graphicsQueue },
    setupCommandBuffer{ creationInfo.setupCommandBuffer }, setupFence{ creationInfo.setupFence } {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

GameModel::~GameModel() {
    vkDestroyBuffer(*device, vertexBuffer, nullptr);
    vkFreeMemory(*device, vertexBufferMemory, nullptr);

    vkDestroyBuffer(*device, indexBuffer, nullptr);
    vkFreeMemory(*device, indexBufferMemory, nullptr);
}

VkVertexInputBindingDescription GameModel::Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 4> GameModel::Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, normal);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}

void GameModel::bind(VkCommandBuffer commandBuffer) {
    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    if (hasIndexBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
};
void GameModel::draw(VkCommandBuffer commandBuffer) {
    if (hasIndexBuffer) {
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    }
    else {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
};

std::unique_ptr<GameModel> GameModel::createModelFromFile(const GameModelCreationInfo& creationInfo, const std::string modelFilePath, const std::string textureFilePath) {
    GameModel::Builder builder{};
    builder.loadModel(modelFilePath);
    if (textureFilePath.length() > 0) {
        builder.loadTexture(textureFilePath);
    }
    return std::make_unique<GameModel>(creationInfo, builder);
}

void GameModel::Builder::loadModel(const std::string& filePath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }

            if (index.texcoord_index >= 0) {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void GameModel::Builder::loadTexture(const std::string& filePath) {
    std::cout << "\n" << "texture loader called" << "\n";
}

void GameModel::createVertexBuffers(const std::vector<GameModel::Vertex>&vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();

    QueueFamilyIndices indices = findQueueFamilies(*physicalDevice, *surface);

    uint32_t queueFamilyIndicesArray[] = { indices.graphicsFamily.value() };

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(*device, *physicalDevice, *surface, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(*device, stagingBufferMemory, 0, vertexBufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)vertexBufferSize);
    vkUnmapMemory(*device, stagingBufferMemory);


    createBuffer(*device, *physicalDevice, *surface, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, vertexBufferSize, *setupCommandBuffer);

    flushCommandBufferCommands(*device, *graphicsQueue, *setupCommandBuffer, *setupFence);

    vkDestroyBuffer(*device, stagingBuffer, nullptr);
    vkFreeMemory(*device, stagingBufferMemory, nullptr);
};

void GameModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
    indexCount = static_cast<uint32_t>(indices.size());
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

    hasIndexBuffer = indexCount > 0;

    if (!hasIndexBuffer) {
        return;
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(*device, *physicalDevice, *surface, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(*device, stagingBufferMemory, 0, indexBufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)indexBufferSize);
    vkUnmapMemory(*device, stagingBufferMemory);

    createBuffer(*device, *physicalDevice, *surface, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, indexBuffer, indexBufferSize, *setupCommandBuffer);

    flushCommandBufferCommands(*device, *graphicsQueue, *setupCommandBuffer, *setupFence);

    vkDestroyBuffer(*device, stagingBuffer, nullptr);
    vkFreeMemory(*device, stagingBufferMemory, nullptr);
};