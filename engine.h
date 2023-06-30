#ifndef ENGINE_H
#define ENGINE_H

#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <vulkan/vulkan_xlib.h>

// TODO: reorder and comment
typedef struct Engine {
    VkInstance instance;
    VkSurfaceKHR surface;

    uint32_t swapchain_image_count;
    VkImage *swapchain_images;
    VkImageView *swapchain_image_views;
    VkFormat swapchain_image_format;

    VkExtent2D window;


    VkFramebuffer *framebuffers;

   
	
    VkPhysicalDevice phys_device;
	
    VkDevice device;

    VkSemaphore present_sema, render_sema;

    VkFence render_fence;

    VkQueue graphics_queue;

    uint32_t graphics_queue_family;

    VkCommandPool command_pool;
	VkCommandBuffer command_buffer;

    VkRenderPass render_pass;

    
	VkSwapchainKHR swapchain;
	
    VkBuffer buffer;
    VkDeviceMemory memory;
    void *mapped_data;
	

	VkPipelineLayout triangle_pipeline_layout;

	VkPipeline triangle_pipeline;
	VkPipeline red_triangle_pipeline;
} Engine;

void engine_init(Engine *e, Display *display, Window window);

void engine_draw(Engine *e, float cycle);

void engine_deinit(Engine *e);

#endif /* ENGINE_H */
