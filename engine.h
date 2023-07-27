#ifndef ENGINE_H
#define ENGINE_H

#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan.h>

typedef struct Engine {
    // BASE
    VkInstance instance;

    VkSurfaceKHR surface;

    VkPhysicalDevice phys_device;

    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode;

    uint32_t graphics_queue_family;
    VkDevice device;
    VkQueue graphics_queue;

    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;

    VkRenderPass render_pass;

    VkFence render_fence;
    VkSemaphore present_sema, render_sema;


    // MEMORY for vertices
    VkBuffer buffer;
    VkDeviceMemory memory;
    void *mapped_data;
    

    // SWAPCHAIN and friends
    VkSwapchainKHR swapchain;
    uint32_t swapchain_image_count;
    VkImageView *swapchain_image_views;

    VkFramebuffer *framebuffers;

    VkExtent2D window;

    int resize_pending;
    // Set initially or signaled by resize, used to check Vulkan behaviour
    int signaled_width, signaled_height;


    // TRIANGLE pipeline
    VkPipeline triangle_pipeline;
} Engine;

void engine_init_xlib(Engine *e, int width, int height, Display *display, Window window);

void engine_signal_resize(Engine *e, int width, int height);

void engine_draw(Engine *e, float cycle);

void engine_deinit(Engine *e);

#endif /* ENGINE_H */
