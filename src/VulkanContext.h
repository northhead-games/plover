#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include "VulkanAllocator.h"
#pragma clang diagnostic pop

#include "DescriptorAllocator.h"
#include "Mesh.h"
#include "Texture.h"
#include "ttfRenderer.h"
#include "includes.h"

namespace Plover {
	const int MAX_FRAMES_IN_FLIGHT = 2;
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::string TEXTURE_PATH = "../resources/textures/viking_room.png";

	const std::vector<Vertex> quadVertices = {
		{{-0.5, -0.5, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0}},
		{{-0.5,  0.5, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0}},
		{{ 0.5,  0.5, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0}},
		{{ 0.5, -0.5, 0.0}, {1.0, 1.0, 0.0}, {1.0, 0.0}},
	};

	const std::vector<u32> quadIndices = {0, 1, 2, 0, 2, 3};

	struct CameraUniformBufferObject {
		alignas(16) glm::mat4 camera;
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct PipelineCreateInfo {
		bool useDepthBuffer;
		bool doCulling;
		u32 subpass;
		const char *vertexShaderPath;
		const char *fragmentShaderPath;

		u32 descriptorSetLayoutCount;
		VkDescriptorSetLayout *pDescriptorSetLayouts;
	};

	struct VulkanContext {
		const std::vector<const char*> validationLayers = {
				"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
#ifdef __APPLE__
				,"VK_KHR_portability_subset"
#endif
		};

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		VkInstance instance;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		VkDevice device;

		VulkanAllocator allocator;
		DescriptorAllocator descriptorAllocator;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkSurfaceKHR surface;

		VkSwapchainKHR swapChain;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;

		VkRenderPass renderPass;

		VkCommandPool drawCommandPool;
		VkCommandPool transientCommandPool;

		VkDescriptorSetLayout globalDescriptorSetLayout;
		VkDescriptorSetLayout meshDescriptorSetLayout;
		VkDescriptorSetLayout uiDescriptorSetLayout;

		std::vector<VkDescriptorSet> globalDescriptorSets;

		VkPipeline uiPipeline;
		VkPipelineLayout uiPipelineLayout;

		std::unordered_map<size_t, Mesh*> meshes;
		std::unordered_map<size_t, UIQuad> uiQuads;
		std::unordered_map<size_t, Material> materials;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VmaAllocation> uniformBuffersAllocations;
		std::vector<void*> uniformBuffersMapped;

		Texture texture;

		VkImage depthImage;
		VmaAllocation depthImageAllocation;
		VkImageView depthImageView;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		bool framebufferResized = false;

		uint32_t currentFrame = 0;

		VkDebugUtilsMessengerEXT debugMessenger;

		GLFWwindow* window;

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		std::vector<const char*> getRequiredExtensions();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);

		void initWindow();

		void initVulkan();

		bool render();

		void cleanup();

		void createInstance();

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void setupDebugMessenger();

		bool checkValidationLayerSupport();

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice currentDevice);

		bool checkDeviceExtensionSupport(VkPhysicalDevice currentDevice);

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		uint32_t rateDeviceSuitability(VkPhysicalDevice currentDevice);

		void pickPhysicalDevice();

		void createLogicalDevice();

		void initAllocator();

		void createSurface();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void createSwapChain();

		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		void createImageViews();

		void createRenderPass();

		void createGlobalDescriptorSetLayout();
		void createMeshDescriptorSetLayout();
		void createUIDescriptorSetLayout();

		VkShaderModule createShaderModule(const std::vector<char>& code);

		void createGraphicsPipeline(PipelineCreateInfo info, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout);

		void createUIPipeline();

		size_t createMaterial();

		void createFramebuffers();

		void createCommandPool(VkCommandPoolCreateFlagBits, VkCommandPool& commandPool);

		void createCommandPools();

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void createTexture(TextureCreateInfo info, Texture& texture);

		void createTextureImage(Texture& texture, const char *path);

		void createTextureSampler(Texture& texture);

		VkFormat findSupportedFormats(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		VkFormat findDepthFormat();

		void createDepthResources();

		VkCommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		void createVertexBuffer(std::vector<Vertex> vertices, VkBuffer& buffer, VmaAllocation& allocation);

		void createIndexBuffer(std::vector<uint32_t> indices, VkBuffer& buffer, VmaAllocation& allocation);

		void createUniformBuffers();

		void createDescriptorAllocator();

		void createGlobalDescriptorSets();

		void createMeshUniform(Mesh& mesh);
		void createUIUniform(UIQuad& quad);

		size_t addMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, size_t materialId);

		size_t addUIQuad();

		void createCommandBuffer();

		void createSyncObjects();

		void recordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex);

		void updateUniformBuffer(uint32_t currentImage);

		void drawFrame();

		void recreateSwapChain();

		void cleanupSwapChain();
	};
}
