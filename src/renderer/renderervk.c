#include "renderervk.h"
#include "buffer.h"
#include "culibc.h"
#include "error.h"
#include "file.h"
#include "igintegration.h"
#include "i3integration.h"
#include "log/log.h"
#include "math/scalar.h"
#include "math/mat.h"
#include <memory.h>
#include <vulkan/vulkan.h>
#include <cimgui.h>
#include <cimgui_impl.h>
#include <cim3d.h>

#define MAX_FRAMES_IN_FLIGHT 2

static struct Context
{
    VkInstance instance;
    VkDebugReportCallbackEXT debug_callback;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkImage depth_image;
    VkDeviceMemory depth_image_memory;
    VkImageView depth_image_view;
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

static struct PhysicalDeviceCapabilities
{
    u32 push_constants_size;
    v2  line_width_range;
    v2  point_size_range;
    u32 min_image_count;
} Capabilities;

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

static struct RendererData
{
    Vertex               cube_vertices[8];
    u16                  cube_indices[36];
    PushConstantsObject  push_constants;
    m4                   projection_matrix;
} D = {
    .cube_vertices = {
        {{-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}}, // A 0
        {{+0.5f, -0.5f, +0.5f}, {1.0f, 0.0f, 1.0f}}, // B 1
        {{+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f, 1.0f}}, // C 2
        {{-0.5f, +0.5f, +0.5f}, {0.0f, 1.0f, 1.0f}}, // D 3
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}, // E 4
        {{+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // F 5
        {{+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}, // G 6
        {{-0.5f, +0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // H 7
    },
    .cube_indices = {
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
    },
};

static void ImGuiCheckVulkanResult(VkResult error)
{
    if (error == 0)
        return;
    ERROR("Vulkan Error: VkResult = %d", error);
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

static VkResult CreateDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugReportCallbackEXT* pDebugMessenger)
{
    PFN_vkCreateDebugReportCallbackEXT func = (PFN_vkCreateDebugReportCallbackEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
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

static VkBool32 CheckValidationLayerSupport(Arena* arena)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    ArenaMarker marker = ArenaMarkerCreate(arena);
    VkLayerProperties* availableLayers = PushArray(arena, VkLayerProperties, layerCount);
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
            ArenaMarkerRollback(marker);
            return VK_FALSE;
        }
    }

    ArenaMarkerRollback(marker);
    return VK_TRUE;
}

static char** GetRequiredExtensions(Arena* arena, uint32_t* extensionCount)
{
    GetVulkanExtensions(C.window, extensionCount, NULL);
    char** extensions = PushArray(arena, char*,
        *extensionCount + C.enable_validation_layers);
    // Please note that the call to GetVulkanExtensions
    // resets the extensionCount to it's original value.
    GetVulkanExtensions(C.window, extensionCount, extensions);

    if (C.enable_validation_layers)
        extensions[(*extensionCount)++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
    size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage,
    void* pUserData)
{
    L_DEBUG("Vulkan: %s", pMessage);

    return VK_FALSE;
 }

static void PopulateDebugMessengerCreateInfo(VkDebugReportCallbackCreateInfoEXT* createInfo)
{
    VkDebugReportCallbackCreateInfoEXT info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        .flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                 VK_DEBUG_REPORT_WARNING_BIT_EXT |
                 VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
        .pfnCallback = DebugCallback,
        .pUserData = NULL
    };
    *createInfo = info;
}

static void PrintExtensions(Arena* arena)
{
    ArenaMarker marker = ArenaMarkerCreate(arena);
    u32 num_extensions;
    vkEnumerateInstanceExtensionProperties(NULL, &num_extensions, NULL);
    VkExtensionProperties* extensions = PushArray(arena, VkExtensionProperties, num_extensions);
    vkEnumerateInstanceExtensionProperties(NULL, &num_extensions, extensions);


    L_DEBUG("Available extensions:");
    for (u32 i = 0; i < num_extensions; i++)
    {
        L_DEBUG("%s in version of %u", extensions[i].extensionName, extensions[i].specVersion);
    }

    ArenaMarkerRollback(marker);
}

static void CreateInstance(Arena* arena)
{
    if (C.enable_validation_layers && !CheckValidationLayerSupport(arena))
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

    ArenaMarker marker = ArenaMarkerCreate(arena);
    uint32_t extensionsCount;
    char** extensions = GetRequiredExtensions(arena, &extensionsCount);
    createInfo.enabledExtensionCount = extensionsCount;
    createInfo.ppEnabledExtensionNames = extensions;
    VkDebugReportCallbackCreateInfoEXT debugCreateInfo;
    if (C.enable_validation_layers)
    {
        createInfo.enabledLayerCount = ArrayCount(C.validation_layers);
        createInfo.ppEnabledLayerNames = C.validation_layers;

        PopulateDebugMessengerCreateInfo(&debugCreateInfo);
        createInfo.pNext = (VkDebugReportCallbackCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, NULL, &C.instance) != VK_SUCCESS)
        ERROR("Vulkan Instance creation failure");

    ArenaMarkerRollback(marker);
}

static void SetupDebugMessenger(void)
{
    if (!C.enable_validation_layers) return;

    VkDebugReportCallbackCreateInfoEXT createInfo = {0};
    PopulateDebugMessengerCreateInfo(&createInfo);

    if (CreateDebugReportCallbackEXT(
        C.instance, &createInfo, NULL, &C.debug_callback) != VK_SUCCESS)
        ERROR("The debug callback couldn't have been created.");
}

static void CreateSurface(void)
{
    if (!CreateVulkanSurface(C.window, C.instance, &C.surface))
        ERROR("Vulkan surface creation error.");
}

static QueueFamilyIndices FindQueueFamilies(Arena* arena, VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    InitQueueFamilyIndices(&indices);

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    ArenaMarker marker = ArenaMarkerCreate(arena);
    VkQueueFamilyProperties* queueFamilies = PushArray(arena, VkQueueFamilyProperties, queueFamilyCount);
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

    ArenaMarkerRollback(marker);

    return indices;
}

static VkBool32 CheckDeviceExtensionSupport(Arena* arena, VkPhysicalDevice device)
{
    ArenaMarker marker = ArenaMarkerCreate(arena);
    u32 extensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

    VkExtensionProperties* availableExtensions = PushArray(arena, VkExtensionProperties, extensionCount);
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

    ArenaMarkerRollback(marker);
    return matchedExtensionsNum == ArrayCount(C.device_extensions);
}

// It's caller's responsibility to free memory allocated with provided arena
static SwapChainSupportDetails QuerySwapChainSupport(Arena* arena, VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, C.surface, &details.capabilites);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, C.surface, &formatCount, NULL);
    if (formatCount > 0) {
        details.formats = PushArray(arena, VkSurfaceFormatKHR, formatCount);
        details.formats_num = formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, C.surface, &formatCount, details.formats);
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, C.surface, &presentModeCount, NULL);
    if (presentModeCount > 0) {
        details.present_modes = PushArray(arena, VkPresentModeKHR, presentModeCount);
        details.present_modes_num = presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, C.surface, &presentModeCount, details.present_modes);
    }

    return details;
}

static VkBool32 IsDeviceSuitable(Arena* arena, VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    QueueFamilyIndices indices = FindQueueFamilies(arena, device);

    const char* device_type_to_string[] = {
        "OTHER", "INTEGRATED_GPU", "DISCRETE_GPU", "VIRTUAL_GPU", "CPU",
    };

    L_INFO("Selecting device %s", deviceProperties.deviceName);
    L_INFO("Having API version of %u", deviceProperties.apiVersion);
    L_INFO("And type of %s", device_type_to_string[deviceProperties.deviceType]);
    L_INFO("Max push constant size: %u", deviceProperties.limits.maxPushConstantsSize);
    L_INFO("Point size range: %f - %f, Granularity: %f",
        deviceProperties.limits.pointSizeRange[0],
        deviceProperties.limits.pointSizeRange[1],
        deviceProperties.limits.pointSizeGranularity);
    L_INFO("Line width range: %f - %f, Granularity: %f, Strict lines?: %s",
        deviceProperties.limits.lineWidthRange[0],
        deviceProperties.limits.lineWidthRange[1],
        deviceProperties.limits.lineWidthGranularity,
        deviceProperties.limits.strictLines ? "true" : "false");

    f32* line_width_range = deviceProperties.limits.lineWidthRange;
    f32* point_size_range = deviceProperties.limits.pointSizeRange;
    Capabilities.line_width_range = v2(line_width_range[0], line_width_range[1]);
    Capabilities.point_size_range = v2(point_size_range[0], point_size_range[1]);
    Capabilities.push_constants_size = deviceProperties.limits.maxPushConstantsSize;

    VkBool32 extensionsSupported = CheckDeviceExtensionSupport(arena, device);

    VkBool32 swapChainAdequate = VK_FALSE;
    if (extensionsSupported) {
        with_arena(arena)
        {
            SwapChainSupportDetails details = QuerySwapChainSupport(arena, device);
            swapChainAdequate = details.formats_num && details.present_modes_num;
        }
    }

    return AreQueueFamilyIndicesComplete(&indices) && extensionsSupported && swapChainAdequate;
}

static void PickPhysicalDevice(Arena* arena)
{
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(C.instance, &deviceCount, NULL);

    if (deviceCount == 0)
        ERROR("No GPUs found on the machine.");

    ArenaMarker marker = ArenaMarkerCreate(arena);
    VkPhysicalDevice* devices = PushArray(arena, VkPhysicalDevice, deviceCount);
    vkEnumeratePhysicalDevices(C.instance, &deviceCount, devices);


    for (u32 i = 0; i < deviceCount; i++)
    {
        if (IsDeviceSuitable(arena, devices[i]))
        {
            C.physical_device = devices[i];
            break;
        }
    }

    if (C.physical_device == VK_NULL_HANDLE)
        ERROR("No suitable GPU found.");

    ArenaMarkerRollback(marker);
}

static void CreateLogicalDevice(Arena* arena)
{
    QueueFamilyIndices indices = FindQueueFamilies(arena, C.physical_device);

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

    VkPhysicalDeviceFeatures deviceFeatures = {
#ifdef CUBANA_DEVELOPMENT
        .geometryShader = VK_TRUE, // Development purposes only
        .largePoints = VK_TRUE
#else
        0
#endif
    };

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

static void CreateSwapChain(Arena* arena)
{
    ArenaMarker marker = ArenaMarkerCreate(arena);
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(arena, C.physical_device);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.formats_num);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.present_modes, swapChainSupport.present_modes_num);
    VkExtent2D extent = ChooseSwapExtent(&swapChainSupport.capabilites);

    Capabilities.min_image_count = swapChainSupport.capabilites.minImageCount;
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

    QueueFamilyIndices indices = FindQueueFamilies(arena, C.physical_device);

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
    ArenaMarkerRollback(marker);

    L_INFO("Swapchain created.");

    vkGetSwapchainImagesKHR(C.device, C.swapchain, &imageCount, NULL);
    C.swapchain_images = PushArray(arena, VkImage, imageCount);
    C.swapchain_images_num = imageCount;
    vkGetSwapchainImagesKHR(C.device, C.swapchain, &imageCount, C.swapchain_images);

    C.swapchain_image_format = surfaceFormat.format;
    C.swapchain_extent = extent;
}

static void CreateImageViews(Arena* arena)
{
    C.swapchain_image_views = PushArray(arena, VkImageView, C.swapchain_images_num);
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
    VkAttachmentDescription color_attachment = {
        .format = C.swapchain_image_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference color_attachment_ref = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentDescription depth_attachment = {
        .format = FindDepthFormat(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference depth_attachment_ref = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_ref,
        .pDepthStencilAttachment = &depth_attachment_ref
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                         VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };

    VkAttachmentDescription attachments[] = {
        color_attachment, depth_attachment
    };

    VkRenderPassCreateInfo render_pass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = ArrayCount(attachments),
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    if (vkCreateRenderPass(C.device, &render_pass_info, NULL, &C.render_pass)
        != VK_SUCCESS)
        ERROR("Render pass creation failure.");

    L_INFO("Render pass was created.");
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

static void CreateGraphicsPipeline(Arena* arena)
{
    VkShaderModule vertShaderModule, fragShaderModule;
    with_arena(arena)
    {
        Buffer vertShaderCode = BufferFromFile(arena, "shaders/shader.vert.spv");
        Buffer fragShaderCode = BufferFromFile(arena, "shaders/shader.frag.spv");

        vertShaderModule = CreateShaderModule(vertShaderCode);
        fragShaderModule = CreateShaderModule(fragShaderCode);
    }

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

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
        .depthBoundsTestEnable = VK_FALSE,
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
        .stencilTestEnable = VK_FALSE,
    };

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depth_stencil,
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

static void CreateFramebuffers(Arena* arena)
{
    C.swapchain_framebuffers =
        PushArray(arena, VkFramebuffer, C.swapchain_images_num);

    for (size_t i = 0; i < C.swapchain_images_num; i++)
    {
        VkImageView attachments[] = {
            C.swapchain_image_views[i],
            C.depth_image_view
        };

        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = C.render_pass,
            .attachmentCount = ArrayCount(attachments),
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

static void CreateCommandPool(Arena* arena)
{
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(arena, C.physical_device);

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    };

    if (vkCreateCommandPool(C.device, &poolInfo, NULL, &C.command_pool)
        != VK_SUCCESS)
        ERROR("Command pool creation failure");
}

VkFormat FindSupportedFormat(VkFormat* candidates, u16 candidates_num,
    VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (u16 i = 0; i < candidates_num; i++)
    {
        VkFormat format = candidates[i];
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(C.physical_device, format, &props);
        b32 is_linear_tiling_satisfied = tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures) & features == features;
        b32 is_optimal_tiling_satisifed = tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures) & features == features;

        if (is_linear_tiling_satisfied || is_optimal_tiling_satisifed)
            return format;
    }

    ERROR("Couldn't find supported format");
    return VK_FORMAT_UNDEFINED;
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

static VkFormat FindDepthFormat(void)
{
    VkFormat desired_depth_formats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };
    return FindSupportedFormat(desired_depth_formats, ArrayCount(desired_depth_formats),
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

static b8 HasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
}

static void CreateDepthResources(void)
{
    VkFormat depth_format = FindDepthFormat();

    // Create Image
    VkImageCreateInfo image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent.width = C.swapchain_extent.width,
        .extent.height = C.swapchain_extent.height,
        .extent.depth = 1,
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = depth_format,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    if (vkCreateImage(C.device, &image_info, NULL, &C.depth_image) != VK_SUCCESS)
        ERROR("Failed to Create Depth Image");

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(C.device, C.depth_image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_requirements.size,
        .memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };

    if (vkAllocateMemory(C.device, &alloc_info, NULL, &C.depth_image_memory) != VK_SUCCESS)
        ERROR("Failed to allocate depth image memory");

    vkBindImageMemory(C.device, C.depth_image, C.depth_image_memory, 0);

    // Create ImageView
    VkImageViewCreateInfo view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = C.depth_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = depth_format,
        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1,
    };

    if (vkCreateImageView(C.device, &view_info, NULL, &C.depth_image_view) != VK_SUCCESS)
        ERROR("Failed creating depth image view");
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
        ERROR("Buffer creation failure");
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
        ERROR("Buffer memory allocation failed");
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
    VkDeviceSize buffer_size = sizeof(D.cube_vertices);

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &staging_buffer, &staging_buffer_memory);

    void* data;
    vkMapMemory(C.device, staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, D.cube_vertices, buffer_size);
    vkUnmapMemory(C.device, staging_buffer_memory);

    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &C.vertex_buffer, &C.vertex_buffer_memory);

    CopyBuffer(staging_buffer, C.vertex_buffer, buffer_size);

    vkDestroyBuffer(C.device, staging_buffer, NULL);
    vkFreeMemory(C.device, staging_buffer_memory, NULL);
}

static void CreateIndexBuffer(void)
{
    VkDeviceSize buffer_size = sizeof(D.cube_indices);

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &staging_buffer, &staging_buffer_memory);

    void* data;
    vkMapMemory(C.device, staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, D.cube_indices, buffer_size);
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

    D.projection_matrix = Perspective(
        90.0f, C.swapchain_extent.width / C.swapchain_extent.height, 0.1f, 50.0f);
    UniformBufferObject ubo = { D.projection_matrix };

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
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT }
    };

    VkDescriptorPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = ArrayCount(pool_sizes),
        .pPoolSizes = pool_sizes,
        .maxSets = ArrayCount(pool_sizes) * MAX_FRAMES_IN_FLIGHT * 2
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

static void CreateCommandBuffers(Arena* arena)
{
    C.command_buffers = PushArray(arena, VkCommandBuffer, C.swapchain_images_num);

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

static void RecordCommandBuffer(Arena* arena, u32 current_image)
{
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    VkClearValue clear_values[] = {
        [0].color = {{0.0f, 0.0f, 0.0f, 1.0f}},
        [1].depthStencil = {0.0f, 0}
    };

    VkRenderPassBeginInfo render_pass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = C.render_pass,
        .renderArea.offset = {0, 0},
        .renderArea.extent = C.swapchain_extent,
        .clearValueCount = ArrayCount(clear_values),
        .pClearValues = clear_values,
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
        0, sizeof(PushConstantsObject), &D.push_constants);
    vkCmdDrawIndexed(cmd_buffer, ArrayCount(D.cube_indices), 64, 0, 0, 0);

    im3dVkEndFrame(arena, cmd_buffer);
    ImGuiRender(cmd_buffer);
    vkCmdEndRenderPass(cmd_buffer);

    if (vkEndCommandBuffer(cmd_buffer) != VK_SUCCESS)
        ERROR("EndCommandBuffer failure");
}

static void CreateSyncObjects(Arena* arena)
{
    C.image_available_semaphores = PushArray(arena, VkSemaphore, MAX_FRAMES_IN_FLIGHT);
    C.render_finished_semaphores = PushArray(arena, VkSemaphore, MAX_FRAMES_IN_FLIGHT);
    C.in_flight_fences = PushArray(arena, VkFence, MAX_FRAMES_IN_FLIGHT);
    C.images_in_flight = PushArray(arena, VkFence, C.swapchain_images_num);

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

static void InitImGui(Window window)
{
    QueueFamilyIndices indices;
    InitQueueFamilyIndices(&indices);

    ImGui_ImplVulkan_InitInfo init_info = {
        .Instance = C.instance,
        .PhysicalDevice = C.physical_device,
        .Device = C.device,
        .QueueFamily = indices.graphicsFamily,
        .Queue = C.graphics_queue,
        .PipelineCache = NULL,
        .DescriptorPool = C.descriptor_pool,
        .Subpass = 0,
        .MinImageCount = 2,
        .ImageCount = MAX_FRAMES_IN_FLIGHT,
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .Allocator = NULL,
        .CheckVkResultFn = ImGuiCheckVulkanResult
    };
    ImGuiInit(window, init_info, C.render_pass);
    ImGuiUploadFonts(C.device, C.graphics_queue, C.command_pool, C.command_buffers[0]);
    ImGuiNewFrame(0.0f);
}

static void InitIm3d(Arena* arena)
{
    Im3dVkInitInfo info = {
        .device = C.device,
        .physical_device = C.physical_device,
        .render_pass = C.render_pass,
        .swapchain_extent = C.swapchain_extent,
        .command_pool = C.command_pool,
        .graphics_queue = C.graphics_queue,
        .projection_matrix = D.projection_matrix
    };
    im3dVkInit(arena, info);
}

int VkRendererInit(Arena* arena, Window window)
{
    InitVkContext(window);
    CreateInstance(arena);
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice(arena);
    CreateLogicalDevice(arena);
    CreateSwapChain(arena);
    CreateImageViews(arena);
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreatePushConstantsRange();
    CreateGraphicsPipeline(arena);
    CreateCommandPool(arena);
    CreateDepthResources();
    CreateFramebuffers(arena);
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffer();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateCommandBuffers(arena);
    CreateSyncObjects(arena);
    InitImGui(window);
    InitIm3d(arena);

    DEBUG_ArenaPrintAllocations(arena);

    return CU_SUCCESS;
}

void VkRendererDraw(Arena* arena, f32 delta)
{
    vkWaitForFences(C.device, 1, &C.in_flight_fences[C.current_frame], VK_TRUE, UINT64_MAX);

    uint32_t image_index;
    vkAcquireNextImageKHR(C.device, C.swapchain, UINT64_MAX,
        C.image_available_semaphores[C.current_frame], VK_NULL_HANDLE, &image_index);

    if (C.images_in_flight[image_index] != VK_NULL_HANDLE)
        vkWaitForFences(C.device, 1, &C.images_in_flight[image_index], VK_TRUE, UINT64_MAX);

    RecordCommandBuffer(arena, image_index);

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

    ImGuiNewFrame(delta);
    im3dVkNewFrame();
}

void VkRendererShutdown(void)
{
    im3dVkShutdown();
    ImGuiShutdown();
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
    D.push_constants.view = LookAt(position, V3Add(position, direction), up);
    im3dVkSetCamera(position, direction);
}
