#include "renderervk.h"
#include "culibc.h"
#include "error.h"
#include "file.h"
#include "log/log.h"
#include "memory/stackallocator.h"
#include "memory/linearallocator.h"
#include "math/scalar.h"
#include "math/mat.h"
#include <memory.h>
#include <vulkan/vulkan.h>

#define MAX_FRAMES_IN_FLIGHT 2

static struct Context
{
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkImage* swapchain_images;
    uint32_t swapchain_images_num;
    VkFormat swapchain_image_format;
    VkExtent2D swapchain_extent;
    VkImageView* swapchain_image_views;
    VkRenderPass render_pass;
    VkDescriptorSetLayout descriptor_set_layout;
    VkPushConstantRange push_constant_range;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
    VkFramebuffer* swapchain_framebuffers;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSet descriptor_sets[MAX_FRAMES_IN_FLIGHT];
    VkCommandPool command_pool;
    VkCommandBuffer* command_buffers;
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_buffer_memory;

    VkBuffer uniform_buffers[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory uniform_buffers_memory[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore* image_available_semaphores;
    VkSemaphore* render_finished_semaphores;
    VkFence* in_flight_fences;
    VkFence* images_in_flight;
    size_t current_frame;

    const char* validation_layers[1];
    const char* device_extensions[1];

    VkBool32 enable_validation_layers;

    Window window;
} C;

static const int INVALID_QUEUE_FAMILY_INDEX = -1;

#define QUEUES_NUM 2
typedef struct QueueFamilyIndices
{
    union
    {
        struct
        {
            int graphicsFamily;
            int presentFamily;
        };
        int indices[QUEUES_NUM];
    };
} QueueFamilyIndices;

typedef struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilites;
    VkSurfaceFormatKHR* formats;
    int formats_num;
    VkPresentModeKHR* present_modes;
    int present_modes_num;
} SwapChainSupportDetails;

typedef struct UniformBufferObject
{
    _Alignas(16) m4 projection;
} UniformBufferObject;

typedef struct PushConstantsObject
{
    m4 view;
} PushConstantsObject;

typedef struct Vertex
{
    v3 pos;
    v3 colour;
} Vertex;

static PushConstantsObject g_PushConstants;

static Vertex vertices[] = {
    {{-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}}, // A 0
    {{+0.5f, -0.5f, +0.5f}, {1.0f, 0.0f, 1.0f}}, // B 1
    {{+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f, 1.0f}}, // C 2
    {{-0.5f, +0.5f, +0.5f}, {0.0f, 1.0f, 1.0f}}, // D 3
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}, // E 4
    {{+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // F 5
    {{+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}, // G 6
    {{-0.5f, +0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // H 7
};

static u16 indices[]= {
    0, 1, 2,
    2, 3, 0,
    3, 4, 0,
    4, 3, 7,
    7, 3, 2,
    2, 6, 7,
    7, 6, 5,
    5, 4, 7,
    5, 6, 2,
    1, 5, 2,
    0, 4, 5,
    0, 5, 1
};

typedef struct Buffer
{
    void* ptr;
    size_t length;
} Buffer;

static Buffer AllocBuffer(size_t size)
{
    Buffer buffer = {
        .ptr = StackMalloc(size, "ShaderBuffer"),
        .length = size
    };
    return buffer;
}

static Buffer FreeBuffer(Buffer buffer)
{
    StackFree(buffer.ptr);
    buffer.ptr = NULL;
    buffer.length = 0;
    return buffer;
}

static VkVertexInputBindingDescription GetBindingDescription(void)
{
    VkVertexInputBindingDescription binding_description = {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    return binding_description;
}

static void GetAttributeDescriptions(VkVertexInputAttributeDescription* descs, u8* num)
{
    static VkVertexInputAttributeDescription attribute_descriptions[] = {
        [0] = {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, pos)
        },
        [1] = {
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, colour)
        }
    };

    memcpy(descs, attribute_descriptions, sizeof(attribute_descriptions));
    *num = ArrayCount(attribute_descriptions);
}

static VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void InitVkContext(Window window)
{
    C.window = window;
    C.physical_device = VK_NULL_HANDLE;
    C.current_frame = 0;
    C.validation_layers[0] = "VK_LAYER_KHRONOS_validation";
    C.device_extensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    #ifdef NDEBUG
    C.enable_validation_layers = VK_FALSE;
    #else
    C.enable_validation_layers = VK_TRUE;
    #endif
}

static void InitQueueFamilyIndices(QueueFamilyIndices* indices)
{
    indices->graphicsFamily = INVALID_QUEUE_FAMILY_INDEX;
    indices->presentFamily = INVALID_QUEUE_FAMILY_INDEX;
}

static VkBool32 AreQueueFamilyIndicesComplete(QueueFamilyIndices* indices)
{
    return indices->graphicsFamily != INVALID_QUEUE_FAMILY_INDEX &&
           indices->presentFamily != INVALID_QUEUE_FAMILY_INDEX;
}

static void FreeSwapChainSupportDetails(SwapChainSupportDetails* details)
{
    if (details->present_modes != NULL) StackFree(details->present_modes);
    if (details->formats != NULL) StackFree(details->formats);
}

static VkBool32 CheckValidationLayerSupport(void)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    VkLayerProperties* availableLayers = StackMalloc(layerCount * sizeof(VkLayerProperties), "VkLayerProperties");
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (int i = 0; i < ArrayCount(C.validation_layers); i++)
    {
        VkBool32 layerFound = VK_FALSE;
        const char* layerName = C.validation_layers[i];

        for (u32 j = 0; j < layerCount; j++)
        {
            const VkLayerProperties* layerProperties = &availableLayers[j];
            if (cu_strcmp(layerName, layerProperties->layerName) == 0)
            {
                layerFound = VK_TRUE;
                break;
            }
        }

        if (!layerFound)
        {
            StackFree(availableLayers);
            return VK_FALSE;
        }
    }
    StackFree(availableLayers);

    return VK_TRUE;
}

static char** GetRequiredExtensions(uint32_t* extensionCount)
{
    GetVulkanExtensions(C.window, extensionCount, NULL);
    size_t allocSize =
        (*extensionCount + C.enable_validation_layers) * sizeof(char*);
    char** extensions = StackMalloc(allocSize, "Vk Extensions array");
    // Please note that the call to GetVulkanExtensions
    // resets the extensionCount to it's original value.
    GetVulkanExtensions(C.window, extensionCount, extensions);

    if (C.enable_validation_layers)
        extensions[(*extensionCount)++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
{
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
        return VK_FALSE;

    L_DEBUG("Vulkan: %s", pCallbackData->pMessage);

    return VK_FALSE;
 }

static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo)
{
    VkDebugUtilsMessengerCreateInfoEXT info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DebugCallback,
        .pUserData = NULL
    };
    *createInfo = info;
}

static void PrintExtensions(void)
{
    u32 num_extensions;
    vkEnumerateInstanceExtensionProperties(NULL, &num_extensions, NULL);
    VkExtensionProperties* extensions = StackMalloc(num_extensions * sizeof(VkExtensionProperties), "VkExtensionProperites");
    vkEnumerateInstanceExtensionProperties(NULL, &num_extensions, extensions);


    L_DEBUG("Available extensions:");
    for (u32 i = 0; i < num_extensions; i++)
    {
        L_DEBUG("%s in version of %u", extensions[i].extensionName, extensions[i].specVersion);
    }

    StackFree(extensions);
}

static void CreateInstance(void)
{
    if (C.enable_validation_layers && !CheckValidationLayerSupport())
        ERROR("The validation layers couldn't have been enabled");

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = GetWindowTitle(C.window),
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Cubana Rendering Engine",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_2,
    };

    VkInstanceCreateInfo createInfo = {
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        createInfo.pApplicationInfo = &appInfo,
    };

    uint32_t extensionsCount;
    char** extensions = GetRequiredExtensions(&extensionsCount);
    createInfo.enabledExtensionCount = extensionsCount;
    createInfo.ppEnabledExtensionNames = extensions;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (C.enable_validation_layers)
    {
        createInfo.enabledLayerCount = ArrayCount(C.validation_layers);
        createInfo.ppEnabledLayerNames = C.validation_layers;

        PopulateDebugMessengerCreateInfo(&debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, NULL, &C.instance) != VK_SUCCESS)
        ERROR("Vulkan Instance creation failure");

    StackFree(extensions);
}

static void SetupDebugMessenger(void)
{
    if (!C.enable_validation_layers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
    PopulateDebugMessengerCreateInfo(&createInfo);

    if (CreateDebugUtilsMessengerEXT(
        C.instance, &createInfo, NULL, &C.debug_messenger) != VK_SUCCESS)
        ERROR("The debug messenger couldn't have been created.");
}

static void CreateSurface(void)
{
    if (!CreateVulkanSurface(C.window, C.instance, &C.surface))
        ERROR("Vulkan surface creation error.");
}

static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    InitQueueFamilyIndices(&indices);

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = StackMalloc(queueFamilyCount * sizeof(VkQueueFamilyProperties), "VkQueueFamilyProperties");
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);


    for (u32 i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, C.surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (AreQueueFamilyIndicesComplete(&indices)) {
            break;
        }
    }

    StackFree(queueFamilies);

    return indices;
}

static VkBool32 CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    u32 extensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

    VkExtensionProperties* availableExtensions = StackMalloc(extensionCount * sizeof(VkExtensionProperties), "VkExtensionProperties");
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

    int matchedExtensionsNum = 0;

    for (u32 i = 0; i < extensionCount; i++)
    {
        for (int j = 0; j < ArrayCount(C.device_extensions); j++)
        {
            if (!cu_strcmp(availableExtensions[i].extensionName, C.device_extensions[j]))
                matchedExtensionsNum++;
        }
    }

    StackFree(availableExtensions);

    return matchedExtensionsNum == ArrayCount(C.device_extensions);
}

// It's callers responsible to free memory with FreeSwapChainSupportDetails
static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, C.surface, &details.capabilites);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, C.surface, &formatCount, NULL);
    if (formatCount > 0) {
        details.formats = StackMalloc(formatCount * sizeof(VkSurfaceFormatKHR), "Swapchain Formats");
        details.formats_num = formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, C.surface, &formatCount, details.formats);
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, C.surface, &presentModeCount, NULL);
    if (presentModeCount > 0) {
        details.present_modes = StackMalloc(presentModeCount * sizeof(VkPresentModeKHR), "Vk Present modes");
        details.present_modes_num = presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, C.surface, &presentModeCount, details.present_modes);
    }

    return details;
}

static VkBool32 IsDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    VkBool32 suitable = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
    VkBool32 tex_compression = deviceFeatures.textureCompressionETC2;

    QueueFamilyIndices indices = FindQueueFamilies(device);

    L_INFO("Selecting device %s", deviceProperties.deviceName);
    L_INFO("Having API version of %u", deviceProperties.apiVersion);
    L_INFO("And type of %d", deviceProperties.deviceType);

    VkBool32 extensionsSupported = CheckDeviceExtensionSupport(device);

    VkBool32 swapChainAdequate = VK_FALSE;
    if (extensionsSupported) {
        SwapChainSupportDetails details = QuerySwapChainSupport(device);
        swapChainAdequate = details.formats_num && details.present_modes_num;
        FreeSwapChainSupportDetails(&details);
    }

    return AreQueueFamilyIndicesComplete(&indices) && extensionsSupported && swapChainAdequate;
}

static void PickPhysicalDevice(void)
{
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(C.instance, &deviceCount, NULL);

    if (deviceCount == 0)
        ERROR("No GPUs found on the machine.");

    VkPhysicalDevice* devices = StackMalloc(deviceCount * sizeof(VkPhysicalDevice), "VkPhysicalDevices");
    vkEnumeratePhysicalDevices(C.instance, &deviceCount, devices);


    for (u32 i = 0; i < deviceCount; i++)
    {
        if (IsDeviceSuitable(devices[i]))
        {
            C.physical_device = devices[i];
            break;
        }
    }

    if (C.physical_device == VK_NULL_HANDLE)
        ERROR("No suitable GPU found.");

    StackFree(devices);
}

static void CreateLogicalDevice(void)
{
    QueueFamilyIndices indices = FindQueueFamilies(C.physical_device);

    VkDeviceQueueCreateInfo queueCreateInfos[QUEUES_NUM];

    int uniqueQueuesNum = indices.graphicsFamily != indices.presentFamily ?
                          QUEUES_NUM : 1;
    float priority = 1.0f;
    for (int i = 0; i < uniqueQueuesNum; i++)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.indices[i],
            .queueCount = 1,
            .pQueuePriorities = &priority,
        };
        queueCreateInfos[i] = queueCreateInfo;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = (uint32_t)uniqueQueuesNum,
        .pQueueCreateInfos = queueCreateInfos,
        .pEnabledFeatures = &deviceFeatures,
        .enabledExtensionCount = ArrayCount(C.device_extensions),
        .ppEnabledExtensionNames = C.device_extensions
    };

    if (C.enable_validation_layers)
    {
        createInfo.enabledLayerCount = ArrayCount(C.validation_layers);
        createInfo.ppEnabledLayerNames = C.validation_layers;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(C.physical_device, &createInfo, NULL, &C.device) != VK_SUCCESS)
        ERROR("Logical device creation error.");

    vkGetDeviceQueue(C.device, indices.graphicsFamily, 0, &C.graphics_queue);
    vkGetDeviceQueue(C.device, indices.presentFamily, 0, &C.present_queue);
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const VkSurfaceFormatKHR* availableFormats, u32 formatsNum)
{
    for (u32 i = 0; i < formatsNum; i++)
    {
        if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormats[i];
    }

    return availableFormats[0]; // Fallback to any surface format
}

static VkPresentModeKHR ChooseSwapPresentMode(const VkPresentModeKHR* modes, u32 presentModesNum)
{
    for (u32 i = 0; i < presentModesNum; i++)
    {
        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            return modes[i];
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities)
{
    if (capabilities->currentExtent.width != UINT32_MAX)
    {
        return capabilities->currentExtent;
    }
    else
    {
        VkExtent2D actualExtent;
        GetVulkanDrawableSize(C.window, &actualExtent.width, &actualExtent.height);
        VkExtent2D minExtent = capabilities->minImageExtent;
        VkExtent2D maxExtent = capabilities->maxImageExtent;
        actualExtent.width = max(minExtent.width, min(maxExtent.width, actualExtent.width));
        actualExtent.height = max(minExtent.height, min(maxExtent.height, actualExtent.width));

        return actualExtent;
    }
}

static void CreateSwapChain(void)
{
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(C.physical_device);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.formats_num);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.present_modes, swapChainSupport.present_modes_num);
    VkExtent2D extent = ChooseSwapExtent(&swapChainSupport.capabilites);

    uint32_t imageCount = swapChainSupport.capabilites.minImageCount + 1;

    if (swapChainSupport.capabilites.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilites.maxImageCount)
        imageCount = swapChainSupport.capabilites.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = C.surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    };

    QueueFamilyIndices indices = FindQueueFamilies(C.physical_device);

    if(indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = indices.indices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = NULL;
    }

    createInfo.preTransform = swapChainSupport.capabilites.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(C.device, &createInfo, NULL, &C.swapchain) != VK_SUCCESS)
    {
        ERROR("SwapChain creation failure.");
    }
    FreeSwapChainSupportDetails(&swapChainSupport);

    L_INFO("Swapchain created.");

    vkGetSwapchainImagesKHR(C.device, C.swapchain, &imageCount, NULL);
    C.swapchain_images = LinearMalloc(imageCount * sizeof(VkImage));
    C.swapchain_images_num = imageCount;
    vkGetSwapchainImagesKHR(C.device, C.swapchain, &imageCount, C.swapchain_images);

    C.swapchain_image_format = surfaceFormat.format;
    C.swapchain_extent = extent;
}

static void CreateImageViews(void)
{
    C.swapchain_image_views = LinearMalloc(C.swapchain_images_num * sizeof(VkImageView));
    VkImageViewCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .format = C.swapchain_image_format,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1
    };

    for (size_t i = 0; i < C.swapchain_images_num; i++) {
        createInfo.image = C.swapchain_images[i];
        if (vkCreateImageView(C.device, &createInfo, NULL, &C.swapchain_image_views[i])
            != VK_SUCCESS) {
                ERROR("Image view creation failure");
            }
    }

    L_INFO("SwapChain image views have been created.");
}

static void CreateRenderPass(void)
{
    VkAttachmentDescription colorAttachment = {
        .format = C.swapchain_image_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    if (vkCreateRenderPass(C.device, &renderPassInfo, NULL, &C.render_pass)
        != VK_SUCCESS)
        ERROR("Render pass creation failure.");

    L_INFO("Render pass was created.");
}

static Buffer File2Buffer(const char* filename)
{
    Buffer result = {0};
    File file = FileOpen(filename, "rb");
    if (!file.valid)
    {
        ERROR("%s", file.error_msg);
        return result;
    }

    s64 fileSize =  FileSize(&file);
    result = AllocBuffer(fileSize);
    char* fileContents = (char*)result.ptr;

    s64 totalObjectsRead = 0, objectsRead = 1;
    char* buffer = fileContents;
    while (totalObjectsRead < fileSize && objectsRead != 0)
    {
        objectsRead = FileRead(&file, buffer, 1, (fileSize - totalObjectsRead));
        totalObjectsRead += objectsRead;
        buffer += objectsRead;
    }
    FileClose(&file);
    if (totalObjectsRead != fileSize)
    {
        result = FreeBuffer(result);
        return result;
    }

    return result;
}

static VkShaderModule CreateShaderModule(Buffer code)
{
    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.length,
        .pCode = (uint32_t*)code.ptr
    };

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(C.device, &createInfo, NULL, &shaderModule)
        != VK_SUCCESS)
            ERROR("Shader module creation failure.");

    return shaderModule;
}

static void CreateDescriptorSetLayout(void)
{
    VkDescriptorSetLayoutBinding ubo_layout_binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorSetLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &ubo_layout_binding
    };

    if (vkCreateDescriptorSetLayout(C.device, &layout_info, NULL, &C.descriptor_set_layout)
        != VK_SUCCESS)
    {
        ERROR("Descriptor Set Layout creation failure!");
    }
}

static void CreatePushConstantsRange(void)
{
    C.push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    C.push_constant_range.offset = 0;
    C.push_constant_range.size = sizeof(PushConstantsObject);
}

static void CreateGraphicsPipeline(void)
{
    Buffer vertShaderCode = File2Buffer("shaders/shader.vert.spv");
    Buffer fragShaderCode = File2Buffer("shaders/shader.frag.spv");

    VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

    FreeBuffer(fragShaderCode);
    FreeBuffer(vertShaderCode);

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main",
    };

    // Note: pSpecializationInfo can be used to set constants in shaders
    // which can be used to optimize out if statements and unroll for loops
    // pretty neat for optimization

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertexShaderStageInfo, fragShaderStageInfo
    };

    VkVertexInputBindingDescription binding_desc = GetBindingDescription();
    u8 attribute_num = 0;
    VkVertexInputAttributeDescription attribute_descs[2];
    GetAttributeDescriptions(attribute_descs, &attribute_num);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding_desc,
        .vertexAttributeDescriptionCount = ArrayCount(attribute_descs),
        .pVertexAttributeDescriptions = attribute_descs,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) C.swapchain_extent.width,
        .height = (float) C.swapchain_extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    VkRect2D scissor = {
        .offset  = {0, 0},
        .extent = C.swapchain_extent,
    };

    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
    };

    // Requires enabling GPU Feature.
    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f,
        .pSampleMask = NULL,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachement = {
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
    };

    VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachement,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f,
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &C.descriptor_set_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &C.push_constant_range,
    };

    if (vkCreatePipelineLayout(C.device, &pipelineLayoutInfo, NULL,
        &C.pipeline_layout) != VK_SUCCESS)
        ERROR("PipelineLayout creation failure.");

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = NULL,
        .pColorBlendState = &colorBlending,
        .pDynamicState = NULL,
        .layout = C.pipeline_layout,
        .renderPass = C.render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    if (vkCreateGraphicsPipelines(C.device, VK_NULL_HANDLE, 1, &pipelineInfo,
        NULL, &C.graphics_pipeline) != VK_SUCCESS)
        ERROR("Graphics Pipelines creation failure");

    L_INFO("Graphics Pipeline has been created.");

    vkDestroyShaderModule(C.device, fragShaderModule, NULL);
    vkDestroyShaderModule(C.device, vertShaderModule, NULL);
}

static void CreateFramebuffers(void)
{
    C.swapchain_framebuffers =
        LinearMalloc(C.swapchain_images_num * sizeof(VkFramebuffer));

    for (size_t i = 0; i < C.swapchain_images_num; i++)
    {
        VkImageView attachments[] = {
            C.swapchain_image_views[i]
        };

        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = C.render_pass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = C.swapchain_extent.width,
            .height = C.swapchain_extent.height,
            .layers = 1,
        };

        if (vkCreateFramebuffer(C.device, &framebufferInfo, NULL,
            &C.swapchain_framebuffers[i]) != VK_SUCCESS)
            ERROR("Framebuffer creation failure");
    }

    L_INFO("Framebuffers created");
}

static void CreateCommandPool(void)
{
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(C.physical_device);

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    };

    if (vkCreateCommandPool(C.device, &poolInfo, NULL, &C.command_pool)
        != VK_SUCCESS)
        ERROR("Command pool creation failure");
}

static u32 FindMemoryType(u32 type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(C.physical_device, &mem_properties);

    for (u32 i = 0; i < mem_properties.memoryTypeCount; i++)
    {
        if (type_filter & (1 << i) &&
           (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    ERROR("Failed to find a suitable memory type");
    return 0;
}

static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* buffer_memory)
{
    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    if (vkCreateBuffer(C.device, &buffer_info, NULL, buffer)
        != VK_SUCCESS)
    {
        ERROR("Vertex buffer creation failure");
    }

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(C.device, *buffer, &memory_requirements);

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits,
            properties)
    };

    if (vkAllocateMemory(C.device, &alloc_info, NULL, buffer_memory)
        != VK_SUCCESS)
    {
        ERROR("Vertex buffer memory allocation failed");
    }

    // Associate buffer with a memory region
    vkBindBufferMemory(C.device, *buffer, *buffer_memory, 0);
}

static void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = C.command_pool,
        .commandBufferCount = 1
    };

    VkCommandBuffer cmd_buffer;
    vkAllocateCommandBuffers(C.device, &alloc_info, &cmd_buffer);

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    VkBufferCopy copy_region = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size
    };
    vkCmdCopyBuffer(cmd_buffer, src_buffer, dst_buffer, 1, &copy_region);

    vkEndCommandBuffer(cmd_buffer);

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd_buffer
    };

    vkQueueSubmit(C.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(C.graphics_queue);

    vkFreeCommandBuffers(C.device, C.command_pool, 1, &cmd_buffer);
}

static void CreateVertexBuffer(void)
{
    VkDeviceSize buffer_size = sizeof(vertices);

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &staging_buffer, &staging_buffer_memory);

    void* data;
    vkMapMemory(C.device, staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, vertices, buffer_size);
    vkUnmapMemory(C.device, staging_buffer_memory);

    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &C.vertex_buffer, &C.vertex_buffer_memory);

    CopyBuffer(staging_buffer, C.vertex_buffer, buffer_size);

    vkDestroyBuffer(C.device, staging_buffer, NULL);
    vkFreeMemory(C.device, staging_buffer_memory, NULL);
}

static void CreateIndexBuffer(void)
{
    VkDeviceSize buffer_size = sizeof(indices);

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &staging_buffer, &staging_buffer_memory);

    void* data;
    vkMapMemory(C.device, staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, indices, buffer_size);
    vkUnmapMemory(C.device, staging_buffer_memory);

    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &C.index_buffer, &C.index_buffer_memory);

    CopyBuffer(staging_buffer, C.index_buffer, buffer_size);

    vkDestroyBuffer(C.device, staging_buffer, NULL);
    vkFreeMemory(C.device, staging_buffer_memory, NULL);
}

static void CreateUniformBuffer(void)
{
    VkDeviceSize buffer_size = sizeof(UniformBufferObject);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &C.uniform_buffers[i], &C.uniform_buffers_memory[i]);
    }

    UniformBufferObject ubo = {
        Perspective(90.0f, C.swapchain_extent.width / C.swapchain_extent.height,
                    0.1f, 50.0f)
    };

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        void* data;
        vkMapMemory(C.device, C.uniform_buffers_memory[i], 0, sizeof(ubo),
            0, &data);
            memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(C.device, C.uniform_buffers_memory[i]);
    }
}

static void CreateDescriptorPool(void)
{
    VkDescriptorPoolSize pool_size = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = MAX_FRAMES_IN_FLIGHT
    };

    VkDescriptorPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .pPoolSizes = &pool_size,
        .maxSets = MAX_FRAMES_IN_FLIGHT
    };

    if (vkCreateDescriptorPool(C.device, &pool_info, NULL, &C.descriptor_pool)
        != VK_SUCCESS)
        ERROR("Descriptor Pool creation failure");
}

static void CreateDescriptorSets(void)
{
    VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        layouts[i] = C.descriptor_set_layout;

    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = C.descriptor_pool,
        .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = layouts
    };

    if (vkAllocateDescriptorSets(C.device, &alloc_info, C.descriptor_sets)
        != VK_SUCCESS)
        ERROR("Descriptor sets allocation failure");

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo buffer_info = {
            .buffer = C.uniform_buffers[i],
            .offset = 0,
            .range = sizeof(UniformBufferObject)
        };

        VkWriteDescriptorSet descriptor_write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = C.descriptor_sets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .pBufferInfo = &buffer_info,
        };

        vkUpdateDescriptorSets(C.device, 1, &descriptor_write, 0, NULL);
    }
}

static void CreateCommandBuffers(void)
{
    C.command_buffers = LinearMalloc(C.swapchain_images_num * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = C.command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t)C.swapchain_images_num,
    };

    if (vkAllocateCommandBuffers(C.device, &alloc_info, C.command_buffers)
        != VK_SUCCESS)
        ERROR("Command buffer allocation failure");
}

static void RecordCommandBuffers(u32 current_image)
{
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
    VkRenderPassBeginInfo render_pass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = C.render_pass,
        .renderArea.offset = {0, 0},
        .renderArea.extent = C.swapchain_extent,
        .clearValueCount = 1,
        .pClearValues = &clear_color,
        .framebuffer = C.swapchain_framebuffers[current_image]
    };

    VkCommandBuffer cmd_buffer = C.command_buffers[current_image];
    if (vkBeginCommandBuffer(cmd_buffer, &begin_info)
        != VK_SUCCESS)
        ERROR("BeginCommandBuffer failure");

    vkCmdBeginRenderPass(
        cmd_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(
        cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, C.graphics_pipeline);

    VkBuffer vertex_buffers[] = { C.vertex_buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(cmd_buffer, C.index_buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        C.pipeline_layout, 0, 1, &C.descriptor_sets[C.current_frame], 0, NULL);
    vkCmdPushConstants(cmd_buffer, C.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT,
        0, sizeof(PushConstantsObject), &g_PushConstants);
    vkCmdDrawIndexed(cmd_buffer, ArrayCount(indices), 64, 0, 0, 0);

    vkCmdEndRenderPass(cmd_buffer);

    if (vkEndCommandBuffer(cmd_buffer) != VK_SUCCESS)
        ERROR("EndCommandBuffer failure");
}

static void CreateSyncObjects(void)
{
    C.image_available_semaphores = LinearMalloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    C.render_finished_semaphores = LinearMalloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    C.in_flight_fences = LinearMalloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);
    C.images_in_flight = LinearMalloc(sizeof(VkFence) * C.swapchain_images_num);
    cu_memset(C.images_in_flight, 0, sizeof(VkFence) * C.swapchain_images_num);

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(C.device, &semaphoreInfo, NULL, &C.image_available_semaphores[i])
            != VK_SUCCESS ||
            vkCreateSemaphore(C.device, &semaphoreInfo, NULL, &C.render_finished_semaphores[i])
            != VK_SUCCESS ||
            vkCreateFence(C.device, &fenceInfo, NULL, &C.in_flight_fences[i])
            != VK_SUCCESS)
            ERROR("Semaphore/Fence creation failure");
    }

    L_INFO("Synchronisation objects created.");
}

int VkRendererInit(Window window)
{
    InitVkContext(window);
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreatePushConstantsRange();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandPool();
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffer();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateCommandBuffers();
    CreateSyncObjects();

    return CU_SUCCESS;
}

void VkRendererDraw(void)
{
    vkWaitForFences(C.device, 1, &C.in_flight_fences[C.current_frame], VK_TRUE, UINT64_MAX);

    uint32_t image_index;
    vkAcquireNextImageKHR(C.device, C.swapchain, UINT64_MAX,
        C.image_available_semaphores[C.current_frame], VK_NULL_HANDLE, &image_index);

    if (C.images_in_flight[image_index] != VK_NULL_HANDLE)
        vkWaitForFences(C.device, 1, &C.images_in_flight[image_index], VK_TRUE, UINT64_MAX);

    RecordCommandBuffers(image_index);

    VkSemaphore wait_semaphores[] =
        { C.image_available_semaphores[C.current_frame] };
    VkPipelineStageFlags wait_stages[] =
        { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &C.command_buffers[image_index],
    };

    VkSemaphore signal_semaphores[] = { C.render_finished_semaphores[C.current_frame] };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(C.device, 1, &C.in_flight_fences[C.current_frame]);
    if (vkQueueSubmit(C.graphics_queue, 1, &submit_info, C.in_flight_fences[C.current_frame])
        != VK_SUCCESS)
        ERROR("Queue submit failure");

    VkSwapchainKHR swap_chains[] = {C.swapchain};
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signal_semaphores,
        .swapchainCount = 1,
        .pSwapchains = swap_chains,
        .pImageIndices = &image_index,
        .pResults = NULL,
    };

    vkQueuePresentKHR(C.present_queue, &present_info);

    C.current_frame = (C.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VkRendererShutdown(void)
{
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyBuffer(C.device, C.uniform_buffers[i], NULL);
        vkFreeMemory(C.device, C.uniform_buffers_memory[i], NULL);
    }
    vkDestroyDescriptorPool(C.device, C.descriptor_pool, NULL);
    vkDestroyDescriptorSetLayout(C.device, C.descriptor_set_layout, NULL);
    vkDestroyBuffer(C.device, C.index_buffer, NULL);
    vkFreeMemory(C.device, C.index_buffer_memory, NULL);
    vkDestroyBuffer(C.device, C.vertex_buffer, NULL);
    vkFreeMemory(C.device, C.vertex_buffer_memory, NULL);
}

void VkRendererSetCamera(v3 position, v3 direction)
{
    v3 up = v3(0.0f, 1.0f, 0.0f);
    g_PushConstants.view = LookAt(position, V3Add(position, direction), up);
}
