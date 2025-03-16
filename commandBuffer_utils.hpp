#pragma once

#include <vulkan/vulkan_core.h>
#include "utils.hpp"

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandBuffer commandBuffer);

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandBuffer commandBuffer);

void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkDevice device, VkCommandPool CommandPool, VkQueue graphicsQueue);

void flushCommandBufferCommands(VkDevice device, VkQueue graphicsQueue, VkCommandBuffer commandBuffer, VkFence fence);

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer& buffer, VkDeviceMemory& bufferMemory);