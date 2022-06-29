#pragma once
#ifdef CIMGUI_USE_SDL

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
struct SDL_Window;
struct SDL_Renderer;
typedef union SDL_Event SDL_Event;
CIMGUI_API bool ImGui_ImplSDL2_InitForOpenGL(SDL_Window* window,void* sdl_gl_context);
CIMGUI_API bool ImGui_ImplSDL2_InitForVulkan(SDL_Window* window);
CIMGUI_API bool ImGui_ImplSDL2_InitForD3D(SDL_Window* window);
CIMGUI_API bool ImGui_ImplSDL2_InitForMetal(SDL_Window* window);
CIMGUI_API bool ImGui_ImplSDL2_InitForSDLRenderer(SDL_Window* window,SDL_Renderer* renderer);
CIMGUI_API void ImGui_ImplSDL2_Shutdown(void);
CIMGUI_API void ImGui_ImplSDL2_NewFrame(void);
CIMGUI_API bool ImGui_ImplSDL2_ProcessEvent(const SDL_Event* event);

#endif
#ifdef CIMGUI_USE_VULKAN
typedef struct VkInstance_T *VkInstance;
typedef struct VkPhysicalDevice_T *VkPhysicalDevice;
typedef struct VkDevice_T *VkDevice;
typedef struct VkQueue_T *VkQueue;
typedef struct VkPipelineCache_T *VkPipelineCache;
typedef struct VkDescriptorPool_T *VkDescriptorPool;
typedef struct VkCommandBuffer_T *VkCommandBuffer;
typedef struct VkImageView_T *VkImageView;
typedef enum VkImageLayout VkImageLayout;
typedef struct VkRenderPass_T *VkRenderPass;
typedef struct VkPipeline_T *VkPipeline;
typedef struct VkDescriptorSet_T *VkDescriptorSet;
typedef struct VkSampler_T *VkSampler;
typedef enum VkSampleCountFlagBits VkSampleCountFlagBits;
typedef struct VkAllocationCallbacks VkAllocationCallbacks;
typedef enum VkResult VkResult;
typedef void (__stdcall *PFN_vkVoidFunction)(void);

typedef struct ImGui_ImplVulkanH_Frame ImGui_ImplVulkanH_Frame;
typedef struct ImGui_ImplVulkanH_Window ImGui_ImplVulkanH_Window;
typedef struct ImGui_ImplVulkan_InitInfo ImGui_ImplVulkan_InitInfo;
struct ImGui_ImplVulkan_InitInfo
{
    VkInstance                      Instance;
    VkPhysicalDevice                PhysicalDevice;
    VkDevice                        Device;
    uint32_t                        QueueFamily;
    VkQueue                         Queue;
    VkPipelineCache                 PipelineCache;
    VkDescriptorPool                DescriptorPool;
    uint32_t                        Subpass;
    uint32_t                        MinImageCount;
    uint32_t                        ImageCount;
    VkSampleCountFlagBits           MSAASamples;
    const VkAllocationCallbacks*    Allocator;
    void                            (*CheckVkResultFn)(VkResult err);
};
CIMGUI_API bool ImGui_ImplVulkan_Init(ImGui_ImplVulkan_InitInfo* info,VkRenderPass render_pass);
CIMGUI_API void ImGui_ImplVulkan_Shutdown(void);
CIMGUI_API void ImGui_ImplVulkan_NewFrame(void);
CIMGUI_API void ImGui_ImplVulkan_RenderDrawData(ImDrawData* draw_data,VkCommandBuffer command_buffer,VkPipeline pipeline);
CIMGUI_API bool ImGui_ImplVulkan_CreateFontsTexture(VkCommandBuffer command_buffer);
CIMGUI_API void ImGui_ImplVulkan_DestroyFontUploadObjects(void);
CIMGUI_API void ImGui_ImplVulkan_SetMinImageCount(uint32_t min_image_count);
CIMGUI_API VkDescriptorSet ImGui_ImplVulkan_AddTexture(VkSampler sampler,VkImageView image_view,VkImageLayout image_layout);
CIMGUI_API bool ImGui_ImplVulkan_LoadFunctions(PFN_vkVoidFunction(*loader_func)(const char* function_name,void* user_data),void* user_data);

#endif
