/************************************************************************************
Filename    :   Win32_VulkanAppUtil.h
Content     :   Vulkan application/hWnd setup functionality for RoomTiny
Created     :   02/09/2017
Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*************************************************************************************/

#ifndef Win32_VulkanAppUtil_h
#define Win32_VulkanAppUtil_h

#include <sstream>
#include <string>
#include <array>
#include <functional>
#include <unordered_map>
#include <assert.h>

#if defined(_WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
    #if !defined(VALIDATE)
        #define VALIDATE(x, msg) \
            if (!(x)) \
            { \
                MessageBoxA(NULL, (msg), ORT_NAME, MB_ICONERROR | MB_OK); \
                exit(-1); \
            }
    #endif
    #if defined(_DEBUG)
        #define ADD_DEBUG_LAYERS
    #endif
#else
    #error Other platforms not yet supported
#endif
#include <vulkan/vulkan.h>

// XXX Switch to a non-OVR math library (glm?)
#define OVR_EXCLUDE_CAPI_FROM_MATH
#include "Extras/OVR_Math.h"

#define countof(x) (sizeof(x)/sizeof((x)[0]))

#define _widen_cstring(str) L##str
#define widen_cstring(str) _widen_cstring(str)

using namespace OVR;

static struct
{
    void Log(const std::string& str)
    {
        std::string out = str + "\n";
        OutputDebugStringA(out.c_str());

    }
    void Log(const std::wstring& str)
    {
        std::wstring out = str + L"\n";
        OutputDebugStringW(out.c_str());
    }
} Debug;

static std::string VulkanResultString(VkResult res)
{
    switch (res)
    {
    case VK_SUCCESS: return "SUCCESS";
    case VK_NOT_READY: return "NOT_READY";
    case VK_TIMEOUT: return "TIMEOUT";
    case VK_EVENT_SET: return "EVENT_SET";
    case VK_EVENT_RESET: return "EVENT_RESET";
    case VK_INCOMPLETE: return "INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY: return "ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED: return "ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST: return "ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED: return "ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT: return "ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT: return "ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT: return "ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER: return "ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS: return "ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED: return "ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_SURFACE_LOST_KHR: return "ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR: return "SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR: return "ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT: return "ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV: return "ERROR_INVALID_SHADER_NV";
    default: return std::to_string(res);
    }
}

template<typename T> std::string dumpHex(T& data)
{
    std::string out = "";
    uint8_t* p = (uint8_t*)&data;
    static const char* hex = "0123456789abcdef";
    for (size_t i = 0; i < sizeof(data); ++i)
    {
        auto b = p[i];
        out.push_back(hex[(b >> 4) & 0xf]);
        out.push_back(hex[(b >> 0) & 0xf]);
    }
    return out;
}

// Base class for object wrappers which manage lifetime with Create/Release
class VulkanObject
{
public:
    VkResult lastResult = VK_SUCCESS;
    virtual void Release() {};
    virtual ~VulkanObject()
    {
        Release();
    }
};

// Check a VulkanObject bool expression for success (true)
#define CHECK(exp) \
do { \
    if (!(exp)) \
    { \
        Debug.Log(std::string(__FILE__) + "(" + std::to_string(__LINE__) + "): " + #exp " failed"); \
        Release(); \
        return false; \
    } \
} while (0)

// Check a VulkanObject bool expression for success (true), log a message on failure
#define CHECKMSG(exp, msg) \
do { \
    if (!(exp)) \
    { \
        Debug.Log(std::string(__FILE__) + "(" + std::to_string(__LINE__) + "): " + (msg)); \
        Release(); \
        return false; \
    } \
} while (0)

// Check a VulkanObject VkResult expression for success (VK_SUCCESS)
#define CHECKVK(exp) \
do { \
    lastResult = (exp); \
    if (lastResult != VK_SUCCESS) \
    { \
        Debug.Log(std::string(__FILE__) + "(" + std::to_string(__LINE__) + "): " + #exp + " returned " + VulkanResultString(lastResult)); \
        Release(); \
        return false; \
    } \
} while (0)

// Check an Oculus ovrResult expression for success
#define CHECKOVR(exp) \
do { \
    auto ret = (exp); \
    if (!OVR_SUCCESS(ret)) \
    { \
        ovrErrorInfo info; \
        ovr_GetLastErrorInfo(&info); \
        Debug.Log(std::string(__FILE__) + "(" + std::to_string(__LINE__) + "): " + #exp + " returned " + std::string(info.ErrorString) + " code " + std::to_string(ret)); \
        Release(); \
        return false; \
    } \
} while (0)

// Swapchain
class Swapchain: public VulkanObject
{
public:
    VkFormat                format;
    VkSurfaceKHR            surface;
    VkSwapchainKHR          swapchain;
    VkFence                 readyFence;
    VkFence                 presentFence;
    static const uint32_t   maxImages = 4;
    uint32_t                swapchainCount;
    uint32_t                renderImageIdx;
    VkImage                 image[maxImages];

    Swapchain() :
        format(VK_FORMAT_B8G8R8A8_SRGB),
        surface(VK_NULL_HANDLE),
        swapchain(VK_NULL_HANDLE),
        readyFence(VK_NULL_HANDLE),
        presentFence(VK_NULL_HANDLE),
        swapchainCount(0),
        renderImageIdx(0)
    {
        for (uint32_t i = 0; i < maxImages; ++i)
            image[i] = VK_NULL_HANDLE;
    }

    bool Init();
    bool Create();
    void Prepare(VkCommandBuffer buf);
    bool Wait();
    bool Acquire(VkSemaphore readySemaphore = VK_NULL_HANDLE);
    bool Present(VkQueue queue, VkSemaphore drawComplete = VK_NULL_HANDLE);
    bool Recreate();
    void Release();
};

// CmdBuffer
class CmdBuffer : public VulkanObject
{
public:
    #define LIST_CMDBUFFER_STATES(_) \
    _(Undefined) \
    _(Initialized) \
    _(Recording) \
    _(Executable) \
    _(Executing)
    enum class CmdBufferState
    {
        #define MK_ENUM(name) name,
        LIST_CMDBUFFER_STATES(MK_ENUM)
    };
    CmdBufferState          state;
    VkCommandPool           pool;
    VkCommandBuffer         buf;
    VkFence                 execFence;

    CmdBuffer() :
        state(CmdBufferState::Undefined),
        pool(VK_NULL_HANDLE),
        buf(nullptr),
        execFence(VK_NULL_HANDLE)
    {
    }

    CmdBuffer(const CmdBuffer& that) = delete;

    std::string StateString(CmdBufferState s)
    {
        switch (s)
        {
        #define MK_CASE(name) case CmdBufferState::name: return #name;
        LIST_CMDBUFFER_STATES(MK_CASE)
        }
        return "(Unknown)";
    }

    #define CHECKCBSTATE(s) \
    do if (state != (s)) \
        { \
            Debug.Log(std::string(__FILE__) + "(" + std::to_string(__LINE__) + "): Expecting state " #s " from " + std::string{__FUNCTION__} + ", in " + StateString(state)); \
            return false; \
        } while (0)

    bool Init(uint32_t queueFamilyIndex);
    bool Begin();
    bool End();
    bool Exec(VkQueue queue);
    bool Wait();
    bool Reset();
    void SetState(CmdBufferState newState)
    {
        state = newState;
    }

    void Release();
};

class KeyStore {
protected:
    std::array<bool, 256> keys_;

public:
    bool& operator[](uint8_t idx) {
        return keys_[static_cast<unsigned int>(idx)];
    }

    #if defined(_WIN32)
        bool& operator[](WPARAM idx) {
            return keys_[idx];
        }
    #endif

    bool& operator[](int idx) {
        return keys_[idx];
    }
};

// Vulkan platform
class Vulkan: public VulkanObject
{
public:
    #if defined(_WIN32)
        HINSTANCE                       hInst = NULL;
        HWND                            hWnd = NULL;
    #endif

    bool                                Running;
    KeyStore                            Key;

    VkInstance                          instance;
    VkPhysicalDevice                    physicalDevice;
    bool                                isAMD;
    VkPhysicalDeviceLimits              deviceLimits;
    VkPhysicalDeviceMemoryProperties    memProps;
    VkDevice                            device;
    VkQueue                             drawQueue;
    VkQueue                             xferQueue;
    VkSemaphore                         drawDone;
    VkSemaphore                         xferDone;
    VkExtent2D                          size;
    std::array<CmdBuffer, Swapchain::maxImages> drawCmd;
    int                                 currentDrawCmd;
    CmdBuffer                           xferCmd;
    Swapchain                           sc;
    struct found
    {
        std::string             gpuName;
        uint32_t                drawQueueFamilyIndex;
        uint32_t                drawQueueIndex;
        uint32_t                xferQueueFamilyIndex;
        uint32_t                xferQueueIndex;
        uint32_t                heap;
    } found;

    #define LIST_VK_INSTANCE_PROCS(_) \
    _(vkGetPhysicalDeviceProperties2KHR) \
    _(vkGetPhysicalDeviceImageFormatProperties2KHR) \
    _(vkCreateDebugReportCallbackEXT) \
    _(vkDestroyDebugReportCallbackEXT)
    //_(vkGetPhysicalDeviceExternalBufferPropertiesKHX)
    #define LIST_VK_DEVICE_PROCS(_) \
    _(vkGetMemoryWin32HandlePropertiesKHR)

    struct api
    {
        #define DECL_VK_PROCS(name) PFN_##name name;
        LIST_VK_INSTANCE_PROCS(DECL_VK_PROCS)
        LIST_VK_DEVICE_PROCS(DECL_VK_PROCS)

        bool LoadInstanceProcs(VkInstance instance)
        {
            #define LOAD_VK_INSTANCE_PROCS(name) \
            if (!(name = (PFN_##name)vkGetInstanceProcAddr(instance, #name))) \
            { \
                Debug.Log("Failed to load instance extension " #name); \
                return false; \
            }
            LIST_VK_INSTANCE_PROCS(LOAD_VK_INSTANCE_PROCS)
            return true;
        }

        bool LoadDeviceProcs(VkDevice device)
        {
            #define LOAD_VK_DEVICE_PROCS(name) \
            if (!(name = (PFN_##name)vkGetDeviceProcAddr(device, #name))) \
            { \
                Debug.Log("Failed to load device extension " #name); \
                return false; \
            }
            LIST_VK_DEVICE_PROCS(LOAD_VK_DEVICE_PROCS)
            return true;
        }
    } api;

    Vulkan() :
        instance(VK_NULL_HANDLE),
        physicalDevice(VK_NULL_HANDLE),
        isAMD(false),
        deviceLimits{},
        memProps{},
        device(VK_NULL_HANDLE),
        drawQueue(VK_NULL_HANDLE),
        xferQueue(VK_NULL_HANDLE),
        drawDone(VK_NULL_HANDLE),
        xferDone(VK_NULL_HANDLE),
        size({ 0, 0 }),
        drawCmd(),
        currentDrawCmd(0),
        xferCmd(),
        sc(),
        found({ "(not found)" })
    {
    }

    void Release()
    {
        ReleaseDevice();
        CloseWindow();
    }

    CmdBuffer& CurrentDrawCmd()
    {
        return drawCmd[currentDrawCmd];
    }

    void NextDrawCmd()
    {
        currentDrawCmd = (currentDrawCmd + 1) % drawCmd.size();
        drawCmd[currentDrawCmd].Wait();
    }

    void DumpVkPhysicalDevice() const
    {
        VkPhysicalDeviceIDPropertiesKHR gpuDevID{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR };
        VkPhysicalDeviceProperties2KHR gpuProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR, &gpuDevID };
        api.vkGetPhysicalDeviceProperties2KHR(physicalDevice, &gpuProps);
        std::string deviceType = "unknown";
        switch (gpuProps.properties.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER: deviceType = "other"; break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: deviceType = "integrated"; break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: deviceType = "discrete"; break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: deviceType = "virtual"; break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU: deviceType = "cpu"; break;
        }

        Debug.Log("GPU " + std::string(gpuProps.properties.deviceName) + " (" + deviceType + ") LUID: " + (gpuDevID.deviceLUIDValid ? dumpHex(gpuDevID.deviceLUID) : std::string("<invalid>")));

        VkQueueFamilyProperties queueFamilyProps[10];
        uint32_t queueFamilyCount = countof(queueFamilyProps);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProps);

        for (uint32_t j = 0; j < queueFamilyCount; ++j)
        {
            std::string flags;
            if (queueFamilyProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) flags += "GRAPHICS ";
            if (queueFamilyProps[j].queueFlags & VK_QUEUE_COMPUTE_BIT) flags += "COMPUTE ";
            if (queueFamilyProps[j].queueFlags & VK_QUEUE_TRANSFER_BIT) flags += "TRANSFER ";
            if (queueFamilyProps[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) flags += "SPARSE ";
            Debug.Log("queue[" + std::to_string(j) + "] count=" + std::to_string(queueFamilyProps[j].queueCount) + " flags=" + flags);
        }

        VkPhysicalDeviceMemoryProperties mem_props{};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &mem_props);

        Debug.Log("mem heaps=" + std::to_string(mem_props.memoryHeapCount) + " types=" + std::to_string(mem_props.memoryTypeCount));
        for (uint32_t j = 0; j < mem_props.memoryHeapCount; ++j)
        {
            std::string flags;
            if (mem_props.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) flags += "DEVICE_LOCAL ";
            Debug.Log("mem heap[" + std::to_string(j) + "] size=" + std::to_string(mem_props.memoryHeaps[j].size >> 30) + "G flags=" + flags);
        }
        for (uint32_t j = 0; j < mem_props.memoryTypeCount; ++j)
        {
            std::string flags;
            if (mem_props.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) flags += "DEVICE_LOCAL ";
            if (mem_props.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) flags += "HOST_VISIBLE ";
            if (mem_props.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) flags += "HOST_COHERENT ";
            if (mem_props.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) flags += "HOST_CACHED ";
            if (mem_props.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) flags += "LAZILY_ALLOCATED ";
            Debug.Log("mem type[" + std::to_string(j) + "] index=" + std::to_string(mem_props.memoryTypes[j].heapIndex) + " flags=" + flags);
        }

        VkExtensionProperties ext[100];
        uint32_t instExtCount = countof(ext);
        vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, ext);
        Debug.Log("Found " + std::to_string(instExtCount) + " instance extensions:");
        for (uint32_t i = 0; i < instExtCount; ++i)
        {
            Debug.Log(ext[i].extensionName);
        }

        uint32_t devExtCount = countof(ext);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &devExtCount, ext);
        Debug.Log("Found " + std::to_string(devExtCount) + " device extensions:");
        for (uint32_t i = 0; i < devExtCount; ++i)
        {
            Debug.Log(ext[i].extensionName);
        }
    }

    static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        Vulkan* p = reinterpret_cast<Vulkan*>(GetWindowLongPtr(hWnd, 0));
        switch (Msg)
        {
        case WM_KEYDOWN:
            p->Key[wParam & 0xff] = true;
            break;
        case WM_KEYUP:
            p->Key[wParam & 0xff] = false;
            break;
        case WM_DESTROY:
            p->Running = false;
            break;
        default:
            return DefWindowProcW(hWnd, Msg, wParam, lParam);
        }
        if ((p->Key['Q'] && p->Key[VK_CONTROL]) || p->Key[VK_ESCAPE])
        {
            p->Running = false;
        }
        return 0;
    }

    bool InitWindow(HINSTANCE hInstance, LPCWSTR title)
    {
        hInst = hInstance;
        Running = true;

        #if defined(_WIN32)
            WNDCLASSW wc{};
            wc.style = CS_CLASSDC;
            wc.lpfnWndProc = WindowProc;
            wc.cbWndExtra = sizeof(this);
            wc.hInstance = hInst;
            wc.lpszClassName = widen_cstring(ORT_NAME);
            RegisterClassW(&wc);

            // adjust the window size and show at InitDevice time
            #if defined(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
                // Make sure we're 1:1 for HMD pixels
                SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            #endif
            hWnd = CreateWindowW(wc.lpszClassName, title, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, 0, 0, hInst, 0);
            if (!hWnd) return false;

            SetWindowLongPtr(hWnd, 0, LONG_PTR(this));

            return true;
        #else // defined(_WIN32)
            return false;
        #endif
    }

    void CloseWindow()
    {
        #if defined(_WIN32)
            if (hWnd)
            {
                DestroyWindow(hWnd);
                hWnd = nullptr;
                UnregisterClassW(widen_cstring(ORT_NAME), hInst);
            }
        #endif
    }

    void ParseExtensionString(char* names, uint32_t& count, const char* const*& arrayPtr)
    {
        uint32_t extensionCount = 0;
        char* nextExtensionName = names;
        static std::array<const char*, 100> extensionNamePtrs;
        while (*nextExtensionName && (extensionCount < extensionNamePtrs.size()))
        {
            extensionNamePtrs[extensionCount++] = nextExtensionName;
            // Skip to a space or null
            while (*(++nextExtensionName))
            {
                if (*nextExtensionName == ' ')
                {
                    // Null-terminate and break out of the loop
                    *nextExtensionName++ = '\0';
                    break;
                }
            }
        }

        count = extensionCount;
        arrayPtr = &extensionNamePtrs[0];
    }

    VkDebugReportCallbackEXT    debugReporter;

    VkBool32 debugReport(
        VkDebugReportFlagsEXT                       flags,
        VkDebugReportObjectTypeEXT                  objectType,
        uint64_t                                    object,
        size_t                                      location,
        int32_t                                     messageCode,
        const char*                                 pLayerPrefix,
        const char*                                 pMessage)
    {
        std::string flagNames;
        std::string objName;

        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
            flagNames += "ERROR:";
        if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
            flagNames += "WARN:";
        if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
            flagNames += "PERF:";
        if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
            flagNames += "DEBUG:";
        if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
            flagNames += "INFO:";

        #define LIST_OBJECT_TYPES(_) \
        _(UNKNOWN) \
        _(INSTANCE) \
        _(PHYSICAL_DEVICE) \
        _(DEVICE) \
        _(QUEUE) \
        _(SEMAPHORE) \
        _(COMMAND_BUFFER) \
        _(FENCE) \
        _(DEVICE_MEMORY) \
        _(BUFFER) \
        _(IMAGE) \
        _(EVENT) \
        _(QUERY_POOL) \
        _(BUFFER_VIEW) \
        _(IMAGE_VIEW) \
        _(SHADER_MODULE) \
        _(PIPELINE_CACHE) \
        _(PIPELINE_LAYOUT) \
        _(RENDER_PASS) \
        _(PIPELINE) \
        _(DESCRIPTOR_SET_LAYOUT) \
        _(SAMPLER) \
        _(DESCRIPTOR_POOL) \
        _(DESCRIPTOR_SET) \
        _(FRAMEBUFFER) \
        _(COMMAND_POOL) \
        _(SURFACE_KHR) \
        _(SWAPCHAIN_KHR) \
        _(DEBUG_REPORT_CALLBACK_EXT) \
        _(DISPLAY_KHR) \
        _(DISPLAY_MODE_KHR) \
        _(DESCRIPTOR_UPDATE_TEMPLATE_KHR)

        #define MK_OBJECT_TYPE_CASE(name) \
        case VK_DEBUG_REPORT_OBJECT_TYPE_##name##_EXT: \
            objName = #name; \
            break;

        switch (objectType)
        {
        default:
        LIST_OBJECT_TYPES(MK_OBJECT_TYPE_CASE)
        }

        if ((objectType == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT) &&
            (strcmp(pLayerPrefix, "Loader Message") == 0) &&
            (strncmp(pMessage, "Device Extension:", 17) == 0))
        {
            return VK_FALSE;
        }

        char hex[32];
        sprintf_s(hex, "0x%llx", object);
        Debug.Log(flagNames + " (" + objName + " " + hex + ") [" + pLayerPrefix + "] " + pMessage);
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            return VK_FALSE;
        }
        if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            return VK_FALSE;
        }
        return VK_FALSE;
    }

    static VkBool32 VKAPI_CALL debugReportThunk(
        VkDebugReportFlagsEXT                       flags,
        VkDebugReportObjectTypeEXT                  objectType,
        uint64_t                                    object,
        size_t                                      location,
        int32_t                                     messageCode,
        const char*                                 pLayerPrefix,
        const char*                                 pMessage,
        void*                                       pUserData)
    {
        return static_cast<Vulkan*>(pUserData)->debugReport(flags,objectType, object, location, messageCode, pLayerPrefix, pMessage);
    }

    struct VrApi {
        // Get the required Vulkan extensions for Vulkan instance creation
        std::function<bool(char* extensionNames, uint32_t* extensionNamesSize)> GetInstanceExtensionsVk;
        // Get the physical device corresponding to the VR session
        std::function<bool(VkInstance instance, VkPhysicalDevice* physicalDevice)> GetSessionPhysicalDeviceVk;
        // Get the required Vulkan extensions for Vulkan device creation
        std::function<bool(char* extensionNames, uint32_t* extensionNamesSize)> GetDeviceExtensionsVk;
    };

    bool InitDevice(const char* appName, int vpW, int vpH, const VrApi& vrApi, bool windowed = true)
    {
        size.width = vpW;
        size.height = vpH;
        Debug.Log("Viewport: " + std::to_string(size.width) + "x" + std::to_string(size.height));

        VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.pApplicationName = appName;
        appInfo.applicationVersion = 1;
        appInfo.pEngineName = ORT_NAME;
        appInfo.engineVersion = 1;
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Get the required Vulkan extensions
        char extensionNames[4096];
        uint32_t extensionNamesSize = sizeof(extensionNames);
        if (!vrApi.GetInstanceExtensionsVk(extensionNames, &extensionNamesSize))
        {
            Release();
            return false;
        }

        // Note: AMD interop currently doesn't work with layers enabled, use the Release build
        #if defined(ADD_DEBUG_LAYERS)
            static const char* layers[] =
            {
                //"VK_LAYER_LUNARG_api_dump",
                //"VK_LAYER_LUNARG_core_validation",
                //"VK_LAYER_LUNARG_device_limits",
                //"VK_LAYER_LUNARG_image",
                //"VK_LAYER_LUNARG_object_tracker",
                //"VK_LAYER_LUNARG_parameter_validation",
                //"VK_LAYER_LUNARG_screenshot",
                //"VK_LAYER_LUNARG_swapchain",
                //"VK_LAYER_GOOGLE_threading",
                //"VK_LAYER_GOOGLE_unique_objects",
                //"VK_LAYER_LUNARG_vktrace",
                //"VK_LAYER_VALVE_steam_overlay",
                //"VK_LAYER_RENDERDOC_Capture",
                "VK_LAYER_LUNARG_standard_validation",
            };
        #endif // defined(ADD_DEBUG_LAYERS)

        VkInstanceCreateInfo instInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instInfo.pApplicationInfo = &appInfo;
        #if defined(ADD_DEBUG_LAYERS)
            instInfo.enabledLayerCount = countof(layers);
            instInfo.ppEnabledLayerNames = layers;
        #endif

        strcat_s(extensionNames, " VK_EXT_debug_report");
        ParseExtensionString(extensionNames, instInfo.enabledExtensionCount, instInfo.ppEnabledExtensionNames);

        // Show the window
        #if defined(_WIN32)
            RECT size = { 0, 0, vpW, vpH };
            AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, false);
            const UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;
            CHECK(SetWindowPos(hWnd, nullptr, 0, 0, size.right - size.left, size.bottom - size.top, flags));
        #endif

        VkExtensionProperties ext[100];
        uint32_t instExtCount = countof(ext);
        vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, ext);
        Debug.Log("Found " + std::to_string(instExtCount) + " Vulkan Extensions:");
        for (uint32_t i = 0; i < instExtCount; ++i)
        {
            Debug.Log(std::string("  ") + ext[i].extensionName);
        }
        Debug.Log("Requried Vulkan Extensions:");
        for (uint32_t i = 0; i < instInfo.enabledExtensionCount; ++i)
        {
            Debug.Log(std::string("  ") + instInfo.ppEnabledExtensionNames[i]);
        }

        VkLayerProperties layerProps[100];
        uint32_t instLayerCount = countof(layerProps);
        CHECKVK(vkEnumerateInstanceLayerProperties(&instLayerCount, layerProps));

        Debug.Log("Vulkan Layers:");
        for (uint32_t i = 0; i < instLayerCount; ++i)
        {
            Debug.Log("Layer " + std::to_string(i) + " " + layerProps[i].layerName + " \"" + layerProps[i].description + "\"");

            VkExtensionProperties instExtensions[100];
            uint32_t instExtCount = countof(instExtensions);

            CHECKVK(vkEnumerateInstanceExtensionProperties(layerProps[i].layerName, &instExtCount, instExtensions));
            for (uint32_t j = 0; j < instExtCount; ++j)
            {
                Debug.Log("    Extension " + std::to_string(j) + " " + instExtensions[j].extensionName);
            }
        }

        //
        // create instance & load instance extensions
        //

        CHECKVK(vkCreateInstance(&instInfo, nullptr, &instance));

        CHECK(api.LoadInstanceProcs(instance));

        VkDebugReportCallbackCreateInfoEXT debugInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT };
        debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        #if defined(ADD_DEBUG_LAYERS)
            debugInfo.flags |=
                VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                VK_DEBUG_REPORT_DEBUG_BIT_EXT;
        #endif
        debugInfo.pfnCallback = debugReportThunk;
        debugInfo.pUserData = this;
        CHECKVK(api.vkCreateDebugReportCallbackEXT(instance, &debugInfo, nullptr, &debugReporter));

        //
        // enumerate devices
        //

        if (!vrApi.GetSessionPhysicalDeviceVk(instance, &physicalDevice))
        {
            Release();
            return false;
        }

        VkPhysicalDeviceProperties devProps{};
        vkGetPhysicalDeviceProperties(physicalDevice, &devProps);
        deviceLimits = devProps.limits;

        DumpVkPhysicalDevice();

        VkPhysicalDeviceProperties gpuProps = {};
        vkGetPhysicalDeviceProperties(physicalDevice, &gpuProps);
        found.gpuName = gpuProps.deviceName;
        // To be cleaned up when a common interop is specified
        static const uint32_t AMDVendorId = 0x1002;
        isAMD = (gpuProps.vendorID == AMDVendorId);

        CHECK(sc.Init());

        VkQueueFamilyProperties queueFamilyProps[10];
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        assert(queueFamilyCount < countof(queueFamilyProps));
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProps);

        found.drawQueueFamilyIndex = found.xferQueueFamilyIndex = queueFamilyCount;

        for (uint32_t i = 0; i < queueFamilyCount; ++i)
        {
            VkBool32 presentable = false;
            CHECKVK(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, sc.surface, &presentable));
            // Only need graphics (not presentation) for draw queue
            if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                found.drawQueueFamilyIndex = i;
            // Only need to vkCmdCopyImage on the xfer queue
            if (presentable && (queueFamilyProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
            {
                found.xferQueueFamilyIndex = i;
            }
        }

        CHECK(found.drawQueueFamilyIndex < queueFamilyCount);
        CHECK(found.xferQueueFamilyIndex < queueFamilyCount);

        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

        for (found.heap = 0; found.heap < memProps.memoryHeapCount; ++found.heap)
        {
            if (memProps.memoryHeaps[found.heap].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                break;
        }

        CHECK(found.heap < memProps.memoryHeapCount);

        VkDeviceQueueCreateInfo queueInfo[2] = { { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO }, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO } };
        VkDeviceCreateInfo deviceInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        float queuePriorities[] = { 1, 0 };

        if (found.drawQueueFamilyIndex == found.xferQueueFamilyIndex)
        {
            CHECK(queueFamilyProps[found.xferQueueFamilyIndex].queueCount >= 2);
            found.drawQueueIndex = 0;
            found.xferQueueIndex = 1;
            queueInfo[0].queueCount = 2;
            queueInfo[0].queueFamilyIndex = found.drawQueueFamilyIndex;
            queueInfo[0].pQueuePriorities = queuePriorities;
            deviceInfo.queueCreateInfoCount = 1;
        }
        else
        {
            found.drawQueueIndex = found.xferQueueIndex = 0;
            queueInfo[0].queueCount = 1;
            queueInfo[0].queueFamilyIndex = found.drawQueueFamilyIndex;
            queueInfo[0].pQueuePriorities = &queuePriorities[0];
            queueInfo[1].queueCount = 1;
            queueInfo[1].queueFamilyIndex = found.xferQueueFamilyIndex;
            queueInfo[1].pQueuePriorities = &queuePriorities[1];
            deviceInfo.queueCreateInfoCount = countof(queueInfo);
        }

        deviceInfo.pQueueCreateInfos = queueInfo;

        // Get required device extensions
        extensionNamesSize = sizeof(extensionNames);
        if (!vrApi.GetDeviceExtensionsVk(extensionNames, &extensionNamesSize))
        {
            Release();
            return false;
        }

        // We could enable VK_KHR_maintenance1 so we can flip the viewport to get GL NDC, but we flip the
        // projection matrix instead
        //strcat_s(extensionNames, " VK_KHR_maintenance1");
        ParseExtensionString(extensionNames, deviceInfo.enabledExtensionCount, deviceInfo.ppEnabledExtensionNames);

        deviceInfo.enabledLayerCount = 0;
        deviceInfo.ppEnabledLayerNames = nullptr;
        VkPhysicalDeviceFeatures features{};
        features.samplerAnisotropy = VK_TRUE;
        // Setting this quiets down a validation error triggered by the runtime
        features.shaderStorageImageMultisample = VK_TRUE;
        deviceInfo.pEnabledFeatures = &features;

        Debug.Log("Creating device " + found.gpuName);
        CHECKVK(vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device));

        if (!isAMD)
        {
            CHECK(api.LoadDeviceProcs(device));
        }

        vkGetDeviceQueue(device, found.drawQueueFamilyIndex, found.drawQueueIndex, &drawQueue);
        vkGetDeviceQueue(device, found.xferQueueFamilyIndex, found.xferQueueIndex, &xferQueue);

        // Semaphore to block on draw complete
        VkSemaphoreCreateInfo semInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        CHECKVK(vkCreateSemaphore(device, &semInfo, nullptr, &drawDone));
        // Semaphore to block on xfer complete
        CHECKVK(vkCreateSemaphore(device, &semInfo, nullptr, &xferDone));

        for (auto& cmd: drawCmd)
        {
            CHECK(cmd.Init(found.drawQueueFamilyIndex));
        }
        CHECK(xferCmd.Init(found.xferQueueFamilyIndex));

        CHECK(sc.Create());

        return true;
    }

    void ReleaseDevice()
    {
        for (auto& cmd: drawCmd)
            cmd.Release();
        xferCmd.Release();
        sc.Release();
        if (device)
        {
            if (drawDone) vkDestroySemaphore(device, drawDone, nullptr);
            if (xferDone) vkDestroySemaphore(device, xferDone, nullptr);
            vkDestroyDevice(device, nullptr);
        }
        if (instance)
        {
            if (debugReporter)
            {
                api.vkDestroyDebugReportCallbackEXT(instance, debugReporter, nullptr);
                debugReporter = VK_NULL_HANDLE;
            }
            vkDestroyInstance(instance, nullptr);
        }
        drawDone = VK_NULL_HANDLE;
        xferDone = VK_NULL_HANDLE;
        drawQueue = VK_NULL_HANDLE;
        xferQueue = VK_NULL_HANDLE;
        device = VK_NULL_HANDLE;
        instance = VK_NULL_HANDLE;
    }

    VkResult AllocateMemory(VkMemoryRequirements const& memReqs, VkDeviceMemory* mem, VkFlags flags = 0, const void* pNext = nullptr) const
    {
        // Search memtypes to find first index with those properties
        for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
        {
            if (memReqs.memoryTypeBits & (1 << i))
            {
                // Type is available, does it match user properties?
                if ((memProps.memoryTypes[i].propertyFlags & flags) == flags)
                {
                    VkMemoryAllocateInfo memAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, pNext };
                    memAlloc.allocationSize = memReqs.size;
                    memAlloc.memoryTypeIndex = i;
                    return vkAllocateMemory(device, &memAlloc, nullptr, mem);
                }
            }
        }

        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    VkResult AllocateBufferMemory(VkBuffer buf, VkDeviceMemory* mem, VkFlags flags = 0) const
    {
        VkMemoryRequirements memReq = {};
        vkGetBufferMemoryRequirements(device, buf, &memReq);
        return AllocateMemory(memReq, mem, flags);
    }

    VkResult AllocateImageMemory(VkImage img, VkDeviceMemory* mem, VkFlags flags = 0) const
    {
        VkMemoryRequirements memReq = {};
        vkGetImageMemoryRequirements(device, img, &memReq);
        return AllocateMemory(memReq, mem, flags);
    }

    bool HandleMessages(void)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return Running;
    }

    void Run(bool (*MainLoop)(bool retryCreate))
    {
        while (HandleMessages())
        {
            // true => we'll attempt to retry for ovrError_DisplayLost
            if (!MainLoop(true))
                break;
            // Sleep a bit before retrying to reduce CPU load while the HMD is disconnected
            Sleep(10);
        }
    }
};

// Global Vulkan state
static Vulkan Platform;

// Swapchain
bool Swapchain::Init()
{
    // Create a WSI surface for the window:
    #if defined(VK_USE_PLATFORM_WIN32_KHR)
        VkWin32SurfaceCreateInfoKHR create_info = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        create_info.flags = 0;
        create_info.hinstance = Platform.hInst;
        create_info.hwnd = Platform.hWnd;
        CHECKVK(vkCreateWin32SurfaceKHR(Platform.instance, &create_info, nullptr, &surface));
    #else
        #error CreateSurface not supported on this OS
    #endif // defined(VK_USE_PLATFORM_WIN32_KHR)

    return true;
}

bool Swapchain::Create()
{
    VkSurfaceCapabilitiesKHR surfCaps;
    CHECKVK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Platform.physicalDevice, surface, &surfCaps));
    CHECK(surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    VkSurfaceFormatKHR surfFmts[100];
    uint32_t surfFmtCount = 0;
    CHECKVK(vkGetPhysicalDeviceSurfaceFormatsKHR(Platform.physicalDevice, surface, &surfFmtCount, nullptr));
    assert(surfFmtCount < countof(surfFmts));
    CHECKVK(vkGetPhysicalDeviceSurfaceFormatsKHR(Platform.physicalDevice, surface, &surfFmtCount, surfFmts));
    uint32_t foundFmt;
    for (foundFmt = 0; foundFmt < surfFmtCount; ++foundFmt)
    {
        if (surfFmts[foundFmt].format == format)
            break;
    }

    CHECK(foundFmt < surfFmtCount);

    VkPresentModeKHR presentModes[10];
    uint32_t presentModeCount = 0;
    CHECKVK(vkGetPhysicalDeviceSurfacePresentModesKHR(Platform.physicalDevice, surface, &presentModeCount, nullptr));
    assert(presentModeCount < countof(presentModes));
    CHECKVK(vkGetPhysicalDeviceSurfacePresentModesKHR(Platform.physicalDevice, surface, &presentModeCount, presentModes));

    // Do not use VSYNC for the mirror window, but Nvidia doesn't support IMMEDIATE so fall back to MAILBOX
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    for (uint32_t i = 0; i < presentModeCount; ++i)
    {
        if ((presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) || (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR))
        {
            presentMode = presentModes[i];
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapchainInfo.flags = 0;
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = surfCaps.minImageCount;
    swapchainInfo.imageFormat = format;
    swapchainInfo.imageColorSpace = surfFmts[foundFmt].colorSpace;
    swapchainInfo.imageExtent = Platform.size;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices = nullptr;
    swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = true;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    CHECKVK(vkCreateSwapchainKHR(Platform.device, &swapchainInfo, nullptr, &swapchain));

    // Fence to throttle host on acquire
    VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    CHECKVK(vkCreateFence(Platform.device, &fenceInfo, nullptr, &readyFence));

    swapchainCount = 0;
    CHECKVK(vkGetSwapchainImagesKHR(Platform.device, swapchain, &swapchainCount, nullptr));
    assert(swapchainCount < countof(image));
    CHECKVK(vkGetSwapchainImagesKHR(Platform.device, swapchain, &swapchainCount, image));
    if (swapchainCount > maxImages)
    {
        Debug.Log("Reducing swapchain length from " + std::to_string(swapchainCount) + " to " + std::to_string(maxImages));
        swapchainCount = maxImages;
    }

    Debug.Log("Swapchain length " + std::to_string(swapchainCount));

    return true;
}

void Swapchain::Prepare(VkCommandBuffer buf)
{
    // Convert swapchain images to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    for (uint32_t i = 0; i < swapchainCount; ++i)
    {
        VkImageMemoryBarrier imgBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        imgBarrier.srcAccessMask = 0; // XXX was VK_ACCESS_TRANSFER_READ_BIT wrong?
        imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imgBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imgBarrier.image = image[i];
        imgBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(buf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgBarrier);
    }
}

bool Swapchain::Wait()
{
    if (presentFence)
    {
        // Wait for the fence...
        CHECKVK(vkWaitForFences(Platform.device, 1, &presentFence, VK_TRUE, UINT64_MAX));
        // ...then reset the fence for future Acquire calls
        CHECKVK(vkResetFences(Platform.device, 1, &presentFence));
        presentFence = VK_NULL_HANDLE;
    }

    return true;
}

bool Swapchain::Acquire(VkSemaphore readySemaphore)
{
    // If we're not using a semaphore to rate-limit the GPU, rate limit the host with a fence instead
    if (readySemaphore == VK_NULL_HANDLE)
    {
        if (!Wait())
            return Recreate();
        presentFence = readyFence;
    }

    lastResult = vkAcquireNextImageKHR(Platform.device, swapchain, UINT64_MAX, readySemaphore, presentFence, &renderImageIdx);
    if (lastResult != VK_SUCCESS)
        return Recreate();

    return true;
}

bool Swapchain::Present(VkQueue queue, VkSemaphore drawComplete)
{
    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    if (drawComplete)
    {
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &drawComplete;
    }
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &renderImageIdx;
    lastResult = vkQueuePresentKHR(queue, &presentInfo);
    if (lastResult != VK_SUCCESS)
        return Recreate();

    return true;
}

bool Swapchain::Recreate()
{
    switch (lastResult)
    {
    case VK_ERROR_OUT_OF_DATE_KHR:
        Debug.Log("Swapchain out of date");
        break;
    case VK_SUBOPTIMAL_KHR:
        Debug.Log("Swapchain suboptimal");
        break;
    default:
        Debug.Log("Swapchain failure " + VulkanResultString(lastResult));
        return false;
    }

    Release();

    // Recreate the surface destroyed by Release
    if (!Init())
    {
        return false;
    }

    // Once you create a new swapchain, surface validation will complain if you don't call vkGetPhysicalDeviceSurfaceSupportKHR() again
    // Here we just check that we've still got a presentably queue family, if we lose the device we tear everything down so this should
    // always be true.
    VkBool32 presentable = false;
    CHECKVK(vkGetPhysicalDeviceSurfaceSupportKHR(Platform.physicalDevice, Platform.found.drawQueueFamilyIndex, surface, &presentable));
    if (!presentable)
    {
        Debug.Log("Swapchain recreated but not presentable");
        return false;
    }

    if (!Create())
    {
        return false;
    }

    // Transition the swapchain surface from UNDEFINED to PRESENTABLE so the main loop doesn't trigger validation warnings
    bool prepared = true;
    prepared &= Platform.xferCmd.Reset();
    prepared &= Platform.xferCmd.Begin();
    Prepare(Platform.xferCmd.buf);
    prepared &= Platform.xferCmd.End();
    prepared &= Platform.xferCmd.Exec(Platform.xferQueue);
    prepared &= Platform.xferCmd.Wait();

    return prepared;
}

void Swapchain::Release()
{
    if (Platform.device)
    {
        // Flush any pending Present() calls which are using the fence
        Wait();
        if (swapchain) vkDestroySwapchainKHR(Platform.device, swapchain, nullptr);
        if (readyFence) vkDestroyFence(Platform.device, readyFence, nullptr);
    }

    if (Platform.instance && surface) vkDestroySurfaceKHR(Platform.instance, surface, nullptr);

    readyFence = VK_NULL_HANDLE;
    presentFence = VK_NULL_HANDLE;
    swapchain = VK_NULL_HANDLE;
    surface = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < swapchainCount; ++i)
    {
        image[i] = VK_NULL_HANDLE;
    }
    swapchainCount = 0;
}

// CmdBuffer
bool CmdBuffer::Init(uint32_t queueFamilyIndex)
{
    CHECKCBSTATE(CmdBufferState::Undefined);

    // Create a command pool to allocate our command buffer from
    VkCommandPoolCreateInfo cmdPoolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
    CHECKVK(vkCreateCommandPool(Platform.device, &cmdPoolInfo, nullptr, &pool));

    // Create the command buffer from the command pool
    VkCommandBufferAllocateInfo cmd{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    cmd.commandPool = pool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;
    CHECKVK(vkAllocateCommandBuffers(Platform.device, &cmd, &buf));

    VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    CHECKVK(vkCreateFence(Platform.device, &fenceInfo, nullptr, &execFence));

    SetState(CmdBufferState::Initialized);
    return true;
}

bool CmdBuffer::Begin()
{
    CHECKCBSTATE(CmdBufferState::Initialized);

    VkCommandBufferBeginInfo cmdBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    CHECKVK(vkBeginCommandBuffer(buf, &cmdBeginInfo));

    SetState(CmdBufferState::Recording);
    return true;
}

bool CmdBuffer::End()
{
    CHECKCBSTATE(CmdBufferState::Recording);

    CHECKVK(vkEndCommandBuffer(buf));

    SetState(CmdBufferState::Executable);
    return true;
}

bool CmdBuffer::Exec(VkQueue queue)
{
    CHECKCBSTATE(CmdBufferState::Executable);

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buf;
    CHECKVK(vkQueueSubmit(queue, 1, &submitInfo, execFence));

    SetState(CmdBufferState::Executing);
    return true;
}

bool CmdBuffer::Wait()
{
    // Waiting on a not-in-flight command buffer is a no-op
    if (state == CmdBufferState::Initialized)
    {
        return true;
    }

    CHECKCBSTATE(CmdBufferState::Executing);

    const uint32_t timeoutNs = 1 * 1000 * 1000 * 1000;
    for (int i = 0; i < 5; ++i)
    {
        lastResult = vkWaitForFences(Platform.device, 1, &execFence, VK_TRUE, timeoutNs);
        if (lastResult == VK_SUCCESS)
        {
            // Buffer can be executed multiple times...
            SetState(CmdBufferState::Executable);
            return true;
        }
        Debug.Log("Waiting for CmdBuffer fence timed out, retrying...");
    }

    return false;
}

bool CmdBuffer::Reset()
{
    if (state != CmdBufferState::Initialized)
    {
        CHECKCBSTATE(CmdBufferState::Executable);

        CHECKVK(vkResetFences(Platform.device, 1, &execFence));
        CHECKVK(vkResetCommandBuffer(buf, 0));

        SetState(CmdBufferState::Initialized);
    }

    return true;
}

void CmdBuffer::Release()
{
    if (Platform.device)
    {
        if (buf) vkFreeCommandBuffers(Platform.device, pool, 1, &buf);
        if (pool) vkDestroyCommandPool(Platform.device, pool, nullptr);
        if (execFence) vkDestroyFence(Platform.device, execFence, nullptr);
    }
    SetState(CmdBufferState::Undefined);
    buf = nullptr;
    pool = VK_NULL_HANDLE;
    execFence = VK_NULL_HANDLE;
}

// RenderPass wrapper
class RenderPass: public VulkanObject
{
public:
    VkFormat                colorFmt;
    VkFormat                depthFmt;
    VkRenderPass            pass;

    RenderPass() :
        pass(VK_NULL_HANDLE)
    {
    }

    bool Create(VkFormat aColorFmt, VkFormat aDepthFmt)
    {
        colorFmt = aColorFmt;
        depthFmt = aDepthFmt;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkAttachmentReference colorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        VkAttachmentReference depthRef = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

        std::array<VkAttachmentDescription, 2> at = {};

        VkRenderPassCreateInfo rpInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        rpInfo.attachmentCount = 0;
        rpInfo.pAttachments = at.data();
        rpInfo.subpassCount = 1;
        rpInfo.pSubpasses = &subpass;

        if (colorFmt != VK_FORMAT_UNDEFINED)
        {
            colorRef.attachment = rpInfo.attachmentCount++;

            at[colorRef.attachment].format = colorFmt;
            at[colorRef.attachment].samples = VK_SAMPLE_COUNT_1_BIT;
            at[colorRef.attachment].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            at[colorRef.attachment].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            at[colorRef.attachment].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            at[colorRef.attachment].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            at[colorRef.attachment].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            // Use VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL since we're blitting this to the mirror window
            at[colorRef.attachment].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorRef;
        }

        if (depthFmt != VK_FORMAT_UNDEFINED)
        {
            depthRef.attachment = rpInfo.attachmentCount++;

            at[depthRef.attachment].format = depthFmt;
            at[depthRef.attachment].samples = VK_SAMPLE_COUNT_1_BIT;
            at[depthRef.attachment].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            at[depthRef.attachment].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            at[depthRef.attachment].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            at[depthRef.attachment].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            at[depthRef.attachment].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            at[depthRef.attachment].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            subpass.pDepthStencilAttachment = &depthRef;
        }

        CHECKVK(vkCreateRenderPass(Platform.device, &rpInfo, nullptr, &pass));

        return true;
    }

    void Release()
    {
        if (Platform.device)
        {
            if (pass) vkDestroyRenderPass(Platform.device, pass, nullptr);
        }
        pass = VK_NULL_HANDLE;
    }
};

// UniformBuffer base class
class UniformBufferBase: public VulkanObject
{
public:
    VkBuffer                buf;
    VkDeviceMemory          mem;
    VkDescriptorBufferInfo  descInfo;

    UniformBufferBase() :
        buf(),
        mem(),
        descInfo()
    {
    }

    const VkDescriptorBufferInfo* GetInfo() const
    {
        return &descInfo;
    }

    void Release()
    {
        if (Platform.device)
        {
            if (buf) vkDestroyBuffer(Platform.device, buf, nullptr);
            if (mem) vkFreeMemory(Platform.device, mem, nullptr);
        }
        buf = VK_NULL_HANDLE;
        mem = VK_NULL_HANDLE;
        descInfo = {};
    }
};

// UniformBuffer template to wrap a C++ struct containing the uniform state
template<typename T> class UniformBuffer : public UniformBufferBase
{
public:
    void Create()
    {
        VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufInfo.size = sizeof(T);
        CHECKVK(vkCreateBuffer(Platform.device, &bufInfo, nullptr, &buf));

        CHECKVK(Platform.AllocateBufferMemory(buf, &mem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

        CHECKVK(vkBindBufferMemory(Platform.device, buf, mem, 0));

        descInfo.buffer = buf;
        descInfo.offset = 0;
        descInfo.range = sizeof(T);

        return true;
    }

    bool Update(const T& data)
    {
        T* map = nullptr;
        CHECKVK(vkMapMemory(Platform.device, mem, 0, sizeof(T), 0, (void **)&map));
        *map = data;
        CHECKVK(vkUnmapMemory(Platform.device, mem));

        return true;
    }
};

// VertexBuffer base class
class VertexBufferBase: public VulkanObject
{
public:
    VkBuffer                                        idxBuf;
    VkDeviceMemory                                  idxMem;
    VkBuffer                                        vtxBuf;
    VkDeviceMemory                                  vtxMem;
    VkVertexInputBindingDescription                 bindDesc;
    std::vector<VkVertexInputAttributeDescription>  attrDesc;

    VertexBufferBase() :
        idxBuf(),
        idxMem(),
        vtxBuf(),
        vtxMem(),
        bindDesc(),
        attrDesc()
    {
    }

    // Chainable method to define vertex attribute layout
    VertexBufferBase& Attr(const VkVertexInputAttributeDescription& attr)
    {
        attrDesc.emplace_back(attr);
        return *this;
    }

    void Release()
    {
        if (Platform.device)
        {
            if (idxBuf) vkDestroyBuffer(Platform.device, idxBuf, nullptr);
            if (idxMem) vkFreeMemory(Platform.device, idxMem, nullptr);
            if (vtxBuf) vkDestroyBuffer(Platform.device, vtxBuf, nullptr);
            if (vtxMem) vkFreeMemory(Platform.device, vtxMem, nullptr);
        }
        idxBuf = VK_NULL_HANDLE;
        idxMem = VK_NULL_HANDLE;
        vtxBuf = VK_NULL_HANDLE;
        vtxMem = VK_NULL_HANDLE;
        bindDesc = {};
        attrDesc.clear();
    }
};

// VertexBuffer template to wrap the indicies and vertexes
template<typename T> class VertexBuffer : public VertexBufferBase
{
public:
    bool Create(uint32_t idxCount, uint32_t vtxCount)
    {
        VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufInfo.size = sizeof(uint16_t) * idxCount;
        CHECKVK(vkCreateBuffer(Platform.device, &bufInfo, nullptr, &idxBuf));
        CHECKVK(Platform.AllocateBufferMemory(idxBuf, &idxMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        CHECKVK(vkBindBufferMemory(Platform.device, idxBuf, idxMem, 0));

        bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufInfo.size = sizeof(T) * vtxCount;
        CHECKVK(vkCreateBuffer(Platform.device, &bufInfo, nullptr, &vtxBuf));
        CHECKVK(Platform.AllocateBufferMemory(vtxBuf, &vtxMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        CHECKVK(vkBindBufferMemory(Platform.device, vtxBuf, vtxMem, 0));

        bindDesc.binding = 0;
        bindDesc.stride = sizeof(T);
        bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return true;
    }

    bool UpdateIndicies(const uint16_t* data, uint32_t count, uint32_t offset = 0)
    {
        uint16_t* map = nullptr;
        CHECKVK(vkMapMemory(Platform.device, idxMem, sizeof(map[0]) * offset, sizeof(map[0]) * count, 0, (void **)&map));
        for (size_t i = 0; i < count; ++i)
            map[i] = data[i];
        vkUnmapMemory(Platform.device, idxMem);

        return true;
    }

    bool UpdateVertexes(const T* data, uint32_t count, uint32_t offset = 0)
    {
        T* map = nullptr;
        CHECKVK(vkMapMemory(Platform.device, vtxMem, sizeof(map[0]) * offset, sizeof(map[0]) * count, 0, (void **)&map));
        for (size_t i = 0; i < count; ++i)
            map[i] = data[i];
        vkUnmapMemory(Platform.device, vtxMem);

        return true;
    }
};

// Image wrapper
class Image: public VulkanObject
{
public:
    enum class Style { WHITE, WALL, FLOOR, CEILING, GRID };

    uint32_t        w, h, mipLevels;
    VkImage         img;
    VkDeviceMemory  mem;
    VkImageView     view;
    struct
    {
        VkImage         img;
        VkDeviceMemory  mem;
    } staging;

    Image() :
        w(0),
        h(0),
        mipLevels(0),
        img(VK_NULL_HANDLE),
        mem(VK_NULL_HANDLE),
        view(VK_NULL_HANDLE)
    {
        staging = { VK_NULL_HANDLE, VK_NULL_HANDLE };
    }

    void SetLayout(uint32_t mipLevel, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        // Keep things simple and synchronize any transfer read/write around the layout change
        VkImageMemoryBarrier imgBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        switch (oldLayout)
        {
        default:
        case VK_IMAGE_LAYOUT_UNDEFINED:
            imgBarrier.srcAccessMask = 0;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            imgBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imgBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        }
        VkPipelineStageFlags dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
        switch (newLayout)
        {
        default:
        case VK_IMAGE_LAYOUT_UNDEFINED:
            imgBarrier.dstAccessMask = 0;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            dstStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        }
        imgBarrier.oldLayout = oldLayout;
        imgBarrier.newLayout = newLayout;
        imgBarrier.image = img;
        imgBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, mipLevel, 1, 0, 1 };

        vkCmdPipelineBarrier(Platform.CurrentDrawCmd().buf, VK_PIPELINE_STAGE_TRANSFER_BIT, dstStageFlags, 0, 0, nullptr, 0, nullptr, 1, &imgBarrier);
    }

    bool Create(Style style, uint32_t width, uint32_t height)
    {
        w = width;
        h = height;
        mipLevels = 1 + (uint32_t)std::floor(std::log2(w | h));

        // Create a linear staging buffer for mip 0

        VkImageCreateInfo imgInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imgInfo.imageType = VK_IMAGE_TYPE_2D;
        imgInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        imgInfo.extent = { w, h, 1 };
        imgInfo.mipLevels = 1;
        imgInfo.arrayLayers = 1;
        imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imgInfo.tiling = VK_IMAGE_TILING_LINEAR;
        imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        imgInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        CHECKVK(vkCreateImage(Platform.device, &imgInfo, nullptr, &staging.img));

        CHECKVK(Platform.AllocateImageMemory(staging.img, &staging.mem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

        CHECKVK(vkBindImageMemory(Platform.device, staging.img, staging.mem, 0));

        VkImageSubresource imgSubRes = {};
        imgSubRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgSubRes.mipLevel = 0;
        imgSubRes.arrayLayer = 0;
        VkSubresourceLayout layout = {};
        vkGetImageSubresourceLayout(Platform.device, staging.img, &imgSubRes, &layout);

        uint8_t* data = nullptr;
        CHECKVK(vkMapMemory(Platform.device, staging.mem, layout.offset, layout.size, 0, (void**)&data));

        // Generate mip level 0 texture on the CPU
        std::function<uint32_t(uint32_t x, uint32_t y)> f;
        switch (style)
        {
        default:
        case Style::WHITE:
            f = [](uint32_t, uint32_t)->uint32_t
            {
                return 0xffffffff;
            };
            break;
        case Style::WALL:
            f = [](uint32_t x, uint32_t y)->uint32_t
            {
                return (((y / 4 & 15) == 0) || (((x / 4 & 15) == 0) && ((((x / 4 & 31) == 0) ^ ((y / 4 >> 4) & 1)) == 0))) ? 0xff3c3c3c : 0xffb4b4b4;
            };
            break;
        case Style::FLOOR:
            f = [](uint32_t x, uint32_t y)->uint32_t
            {
                return (((x >> 7) ^ (y >> 7)) & 1) ? 0xffb4b4b4 : 0xff505050;
            };
            break;
        case Style::CEILING:
            f = [](uint32_t x, uint32_t y)->uint32_t
            {
                return ((x / 4) == 0 || ((y / 4) == 0)) ? 0xff505050 : 0xffb4b4b4;
            };
            break;
        case Style::GRID:
            f = [width, height](uint32_t x, uint32_t y)->uint32_t
            {
                return ((x < 4) || (x >= (width - 4)) || (y < 4) || (y >= (height - 4))) ? 0xffffffff : 0xff000000;
            };
            break;
        }

        for (uint32_t y = 0; y < h; ++y)
        {
            uint32_t* row = (uint32_t*)&data[layout.offset + y * layout.rowPitch];
            for (uint32_t x = 0; x < w; ++x)
            {
                row[x] = f(x, y);
            }
        }

        vkUnmapMemory(Platform.device, staging.mem);

        // Create the device-local image

        // Use same create info as staging with these changes
        imgInfo.mipLevels = mipLevels;
        imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        CHECKVK(vkCreateImage(Platform.device, &imgInfo, nullptr, &img));

        CHECKVK(Platform.AllocateImageMemory(img, &mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

        CHECKVK(vkBindImageMemory(Platform.device, img, mem, 0));

        VkImageBlit blit =
        {
            { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            { { 0, 0, 0 }, { (int32_t)w, (int32_t)h, 1 } },
            { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            { { 0, 0, 0 }, { (int32_t)w, (int32_t)h, 1 } }
        };

        // Switch the staging buffer from PREINITIALIZED -> TRANSFER_SRC_OPTIMAL
        VkImageMemoryBarrier imgBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        imgBarrier.srcAccessMask = 0;
        imgBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imgBarrier.image = staging.img;
        imgBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(Platform.CurrentDrawCmd().buf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgBarrier);

        // Switch to writing the finest level
        SetLayout(0, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vkCmdBlitImage(Platform.CurrentDrawCmd().buf, staging.img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        for (uint32_t mipLevel = 1; mipLevel < mipLevels; ++mipLevel)
        {
            // Switch to reading from the finer level...
            SetLayout(mipLevel - 1, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
            // ...and writing the coarser level
            SetLayout(mipLevel, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            blit.srcSubresource.mipLevel = mipLevel - 1;
            blit.srcOffsets[1] = blit.dstOffsets[1];
            blit.dstSubresource.mipLevel = mipLevel;
            blit.dstOffsets[1].x >>= 1;
            if (blit.dstOffsets[1].x < 1)
                blit.dstOffsets[1].x = 1;
            blit.dstOffsets[1].y >>= 1;
            if (blit.dstOffsets[1].y < 1)
                blit.dstOffsets[1].y = 1;
            vkCmdBlitImage(Platform.CurrentDrawCmd().buf, img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
        }

        // Get all the mip levels ready for sampling
        for (uint32_t mipLevel = 0; mipLevel < (mipLevels - 1); ++mipLevel)
        {
            SetLayout(mipLevel, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        // Coarsest level is still DST_OPTIMAL
        SetLayout(mipLevels - 1, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Create image view
        VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.image = img;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imgInfo.format;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = imgInfo.mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        CHECKVK(vkCreateImageView(Platform.device, &viewInfo, nullptr, &view));

        return true;
    }

    void Release()
    {
        if (Platform.device)
        {
            if (staging.img) vkDestroyImage(Platform.device, staging.img, nullptr);
            if (staging.mem) vkFreeMemory(Platform.device, staging.mem, nullptr);
            if (view) vkDestroyImageView(Platform.device, view, nullptr);
            if (img) vkDestroyImage(Platform.device, img, nullptr);
            if (mem) vkFreeMemory(Platform.device, mem, nullptr);
        }
        staging.img = VK_NULL_HANDLE;
        staging.mem = VK_NULL_HANDLE;
        view = VK_NULL_HANDLE;
        mem = VK_NULL_HANDLE;
        img = VK_NULL_HANDLE;
    }
};

// DepthBuffer
class DepthBuffer: public VulkanObject
{
public:
    VkFormat                format;
    VkDeviceMemory          mem;
    VkImage                 img;
    VkImageView             view;

    DepthBuffer() :
        format(VK_FORMAT_D32_SFLOAT),
        mem(),
        img(),
        view()
    {
    }

    bool Create(VkExtent2D extent, VkFormat aFormat = VK_FORMAT_D32_SFLOAT)
    {
        format = aFormat;

        VkImageCreateInfo imgInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imgInfo.imageType = VK_IMAGE_TYPE_2D;
        imgInfo.format = format;
        imgInfo.extent = { extent.width, extent.height, 1 };
        imgInfo.mipLevels = 1;
        imgInfo.arrayLayers = 1;
        imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imgInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        CHECKVK(vkCreateImage(Platform.device, &imgInfo, nullptr, &img));

        CHECKVK(Platform.AllocateImageMemory(img, &mem));

        CHECKVK(vkBindImageMemory(Platform.device, img, mem, 0));

        VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.image = img;
        viewInfo.format = format;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.flags = 0;
        CHECKVK(vkCreateImageView(Platform.device, &viewInfo, nullptr, &view));

        return true;
    }

    void Release()
    {
        if (Platform.device)
        {
            if (view) vkDestroyImageView(Platform.device, view, nullptr);
            if (img) vkDestroyImage(Platform.device, img, nullptr);
            if (mem) vkFreeMemory(Platform.device, mem, nullptr);
        }
        view = VK_NULL_HANDLE;
        img = VK_NULL_HANDLE;
        mem = VK_NULL_HANDLE;
    }
};

// Texture sampler
class Sampler: public VulkanObject
{
public:
    VkSampler sampler;

    Sampler() :
        sampler()
    {
    }

    bool Create()
    {
        VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = Platform.deviceLimits.maxSamplerAnisotropy;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 99.0f;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        CHECKVK(vkCreateSampler(Platform.device, &samplerCreateInfo, nullptr, &sampler));

        return true;
    }

    void Release()
    {
        if (Platform.device)
        {
            vkDestroySampler(Platform.device, sampler, nullptr);
        }
        sampler = VK_NULL_HANDLE;
    }
};

// ShaderProgram to hold vertex/fragment shaders
class ShaderProgram: public VulkanObject
{
public:
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderInfo;

    ShaderProgram() :
        shaderInfo{}
    {
    }

    const bool LoadSPVResource(const char* name, VkShaderModule* module)
    {
        VkShaderModuleCreateInfo modInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };

        #if defined(_WIN32)
            auto hmod = ::GetModuleHandle(NULL);
            CHECKMSG(hmod, "Failed to get module handle");
            auto rc = ::FindResourceA(hmod, name, "spv");
            CHECKMSG(rc, "Failed to find resource " + std::string(name));
            auto hgbl = ::LoadResource(hmod, rc);
            CHECKMSG(hgbl, "Failed to load resource " + std::string(name));
            modInfo.codeSize = ::SizeofResource(hmod, rc);
            modInfo.pCode = static_cast<const uint32_t*>(::LockResource(hgbl));
        #else
            #error Shader resources not defined
        #endif

        CHECKMSG((modInfo.codeSize > 0) && modInfo.pCode, "Invalid shader " + std::string(name));

        CHECKVK(vkCreateShaderModule(Platform.device, &modInfo, nullptr, module));

        return true;
    }

    bool Create(const char* vertName = "vert", const char* fragName = "frag")
    {
        shaderInfo =
        {{
            { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
            { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO }
        }};
        shaderInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderInfo[0].pName = "main";
        shaderInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderInfo[1].pName = "main";

        if (!LoadSPVResource(vertName, &shaderInfo[0].module))
            goto Abort;

        if (!LoadSPVResource(fragName, &shaderInfo[1].module))
            goto Abort;

        Debug.Log("Loaded shaders");
        return true;

    Abort:
        Release();
        return false;
    }

    void Release()
    {
        if (Platform.device)
        {
            if (shaderInfo[0].module) vkDestroyShaderModule(Platform.device, shaderInfo[0].module, nullptr);
            if (shaderInfo[1].module) vkDestroyShaderModule(Platform.device, shaderInfo[1].module, nullptr);
        }
        shaderInfo[0].module = VK_NULL_HANDLE;
        shaderInfo[1].module = VK_NULL_HANDLE;
        shaderInfo = {};
    }
};

// Framebuffer wrapper
class Framebuffer: public VulkanObject
{
public:
    VkFramebuffer   fb;

    Framebuffer() :
        fb(VK_NULL_HANDLE)
    {
    }

    bool Create(VkExtent2D extent, const RenderPass& rp, VkImageView colorView, VkImageView depthView)
    {
        std::array<VkImageView, 2> attachments = {};
        uint32_t attachmentCount = 0;
        if (colorView != VK_NULL_HANDLE)
            attachments[attachmentCount++] = colorView;
        if (depthView != VK_NULL_HANDLE)
            attachments[attachmentCount++] = depthView;

        VkFramebufferCreateInfo fbInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO  };
        fbInfo.renderPass = rp.pass;
        fbInfo.attachmentCount = attachmentCount;
        fbInfo.pAttachments = attachments.data();
        fbInfo.width = extent.width;
        fbInfo.height = extent.height;
        fbInfo.layers = 1;
        CHECKVK(vkCreateFramebuffer(Platform.device, &fbInfo, nullptr, &fb));

        return true;
    }

    void Release()
    {
        if (Platform.device)
        {
            if (fb) vkDestroyFramebuffer(Platform.device, fb, nullptr);
        }
        fb = VK_NULL_HANDLE;
    }
};

// Simple vertex MVP xform & textured pixel shader layout
class PipelineLayout: public VulkanObject
{
public:
    VkDescriptorSetLayout   descLayout;
    VkPipelineLayout        pipeLayout;

    PipelineLayout() :
        descLayout(VK_NULL_HANDLE),
        pipeLayout(VK_NULL_HANDLE)
    {
    }

    bool Create()
    {
        // MVP matrix is a push_constant
        VkPushConstantRange pcr = {};
        pcr.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pcr.offset = 0;
        pcr.size = 4 * 4 * sizeof(float);

        // Texture sampler descriptor
        VkDescriptorSetLayoutBinding db = {};
        db.binding = 0;
        db.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        db.descriptorCount = 1;
        db.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo descLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        descLayoutInfo.bindingCount = 1;
        descLayoutInfo.pBindings = &db;
        CHECKVK(vkCreateDescriptorSetLayout(Platform.device, &descLayoutInfo, nullptr, &descLayout));

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &descLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pcr;
        CHECKVK(vkCreatePipelineLayout(Platform.device, &pipelineLayoutCreateInfo, nullptr, &pipeLayout));

        return true;
    }

    void Release()
    {
        if (Platform.device)
        {
            if (pipeLayout) vkDestroyPipelineLayout(Platform.device, pipeLayout, nullptr);
            if (descLayout) vkDestroyDescriptorSetLayout(Platform.device, descLayout, nullptr);
        }
        descLayout = VK_NULL_HANDLE;
        pipeLayout = VK_NULL_HANDLE;
    }
};

// Texture sampler descriptor set
class DescriptorSet: public VulkanObject
{
public:
    VkDescriptorPool    descPool;
    VkDescriptorSet     descSet;

    DescriptorSet() :
        descPool(VK_NULL_HANDLE),
        descSet(VK_NULL_HANDLE)
    {
    }

    bool Create(const PipelineLayout& layout)
    {
        std::array<VkDescriptorPoolSize, 1> descPoolSizes =
        {{
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
        }};

        VkDescriptorPoolCreateInfo descPoolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        descPoolInfo.maxSets = 1;
        descPoolInfo.poolSizeCount = (uint32_t)descPoolSizes.size();
        descPoolInfo.pPoolSizes = descPoolSizes.data();
        CHECKVK(vkCreateDescriptorPool(Platform.device, &descPoolInfo, nullptr, &descPool));

        VkDescriptorSetAllocateInfo descAllocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        descAllocInfo.descriptorPool = descPool;
        descAllocInfo.descriptorSetCount = 1;
        descAllocInfo.pSetLayouts = &layout.descLayout;
        CHECKVK(vkAllocateDescriptorSets(Platform.device, &descAllocInfo, &descSet));

        return true;
    }

    void Write(const VkDescriptorImageInfo& imgInfo)
    {
        VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.dstSet = descSet;
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &imgInfo;

        vkUpdateDescriptorSets(Platform.device, 1, &write, 0, nullptr);
    }

    void Bind(const PipelineLayout& pipeLayout)
    {
        vkCmdBindDescriptorSets(Platform.CurrentDrawCmd().buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeLayout.pipeLayout, 0, 1, &descSet, 0, nullptr);
    }

    void Release()
    {
        if (Platform.device)
        {
            if (descPool) vkDestroyDescriptorPool(Platform.device, descPool, nullptr);
        }
        descSet = VK_NULL_HANDLE;
        descPool = VK_NULL_HANDLE;
    }
};

// Combined texture image/sampler
class Texture: public VulkanObject
{
public:
    Sampler                 sampler;
    DescriptorSet           desc;
    VkDescriptorImageInfo   imgInfo;

    Texture() :
        sampler(),
        desc(),
        imgInfo()
    {
    }

    bool Create(const PipelineLayout& layout, const Image& img)
    {
        CHECK(sampler.Create());
        CHECK(desc.Create(layout));

        imgInfo.sampler = sampler.sampler;
        imgInfo.imageView = img.view;
        imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        desc.Write(imgInfo);

        return true;
    }

    void Bind(const PipelineLayout& pipeLayout)
    {
        desc.Bind(pipeLayout);
    }

    void Release()
    {
        sampler.Release();
        desc.Release();
    }
};

// Pipeline wrapper for rendering pipeline state
class Pipeline: public VulkanObject
{
public:
    VkPipeline                  pipe;
    VkPrimitiveTopology         topology;
    std::vector<VkDynamicState> dynamicStateEnables;

    Pipeline() :
        pipe(),
        topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
        dynamicStateEnables()
    {
    }

    void Dynamic(VkDynamicState state)
    {
        dynamicStateEnables.emplace_back(state);
    }

    bool Create(VkExtent2D size, const PipelineLayout& layout, const RenderPass& rp, const ShaderProgram& sp, const VertexBufferBase& vb)
    {
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.dynamicStateCount = (uint32_t)dynamicStateEnables.size();
        dynamicState.pDynamicStates = dynamicStateEnables.data();

        VkPipelineVertexInputStateCreateInfo vi = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vi.vertexBindingDescriptionCount = 1;
        vi.pVertexBindingDescriptions = &vb.bindDesc;
        vi.vertexAttributeDescriptionCount = (uint32_t)vb.attrDesc.size();
        vi.pVertexAttributeDescriptions = vb.attrDesc.data();

        VkPipelineInputAssemblyStateCreateInfo ia = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        ia.primitiveRestartEnable = VK_FALSE;
        ia.topology = topology;

        VkPipelineRasterizationStateCreateInfo rs = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rs.polygonMode = VK_POLYGON_MODE_FILL;
        rs.cullMode = VK_CULL_MODE_BACK_BIT;
        rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rs.depthClampEnable = VK_FALSE;
        rs.rasterizerDiscardEnable = VK_FALSE;
        rs.depthBiasEnable = VK_FALSE;
        rs.depthBiasConstantFactor = 0;
        rs.depthBiasClamp = 0;
        rs.depthBiasSlopeFactor = 0;
        rs.lineWidth = 1.0f;

        VkPipelineColorBlendAttachmentState attachState = {};
        attachState.blendEnable = 0;
        attachState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        attachState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachState.colorBlendOp = VK_BLEND_OP_ADD;
        attachState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        attachState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachState.alphaBlendOp = VK_BLEND_OP_ADD;
        attachState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo cb = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        cb.attachmentCount = 1;
        cb.pAttachments = &attachState;
        cb.logicOpEnable = VK_FALSE;
        cb.logicOp = VK_LOGIC_OP_NO_OP;
        cb.blendConstants[0] = 1.0f;
        cb.blendConstants[1] = 1.0f;
        cb.blendConstants[2] = 1.0f;
        cb.blendConstants[3] = 1.0f;

        VkRect2D scissor = { { 0, 0 }, size };
        VkViewport viewport = { 0.0f, 0.0f, (float)size.width, (float)size.height, 0.0f, 1.0f };
        VkPipelineViewportStateCreateInfo vp = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        vp.viewportCount = 1;
        vp.pViewports = &viewport;
        vp.scissorCount = 1;
        vp.pScissors = &scissor;

        VkPipelineDepthStencilStateCreateInfo ds = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        ds.depthTestEnable = VK_TRUE;
        ds.depthWriteEnable = VK_TRUE;
        ds.depthCompareOp = VK_COMPARE_OP_LESS;
        ds.depthBoundsTestEnable = VK_FALSE;
        ds.stencilTestEnable = VK_FALSE;
        ds.front.failOp = VK_STENCIL_OP_KEEP;
        ds.front.passOp = VK_STENCIL_OP_KEEP;
        ds.front.depthFailOp = VK_STENCIL_OP_KEEP;
        ds.front.compareOp = VK_COMPARE_OP_ALWAYS;
        ds.back = ds.front;
        ds.minDepthBounds = 0.0f;
        ds.maxDepthBounds = 1.0f;

        VkPipelineMultisampleStateCreateInfo ms = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkGraphicsPipelineCreateInfo pipeInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipeInfo.stageCount = (uint32_t)sp.shaderInfo.size();
        pipeInfo.pStages = sp.shaderInfo.data();
        pipeInfo.pVertexInputState = &vi;
        pipeInfo.pInputAssemblyState = &ia;
        pipeInfo.pTessellationState = nullptr;
        pipeInfo.pViewportState = &vp;
        pipeInfo.pRasterizationState = &rs;
        pipeInfo.pMultisampleState = &ms;
        pipeInfo.pDepthStencilState = &ds;
        pipeInfo.pColorBlendState = &cb;
        if (dynamicState.dynamicStateCount > 0)
        {
            pipeInfo.pDynamicState = &dynamicState;
        }
        pipeInfo.layout = layout.pipeLayout;
        pipeInfo.renderPass = rp.pass;
        pipeInfo.subpass = 0;
        CHECKVK(vkCreateGraphicsPipelines(Platform.device, VK_NULL_HANDLE, 1, &pipeInfo, nullptr, &pipe));

        return true;
    }

    void Release()
    {
        if (Platform.device)
        {
            if (pipe) vkDestroyPipeline(Platform.device, pipe, nullptr);
        }
        pipe = VK_NULL_HANDLE;
    }
};

// The actual vertex format hard-coded into our hard-coded shaders:
struct Vertex
{
    float pos[4];
    float color[4];
    float texcoord[2];
};

class Model: public VulkanObject
{
public:
    Vector3f                pos;
    Texture&                tex; // Might be better to use std::shared_ptr<>
    std::vector<uint16_t>   idx;
    std::vector<Vertex>     vtx;
    uint32_t                idxCount;
    uint32_t                vtxCount;
    uint32_t                firstIdx;
    int32_t                 vtxOffset;

    Model(Vector3f pos, Texture& tex) :
        pos(pos),
        tex(tex),
        idx(),
        vtx(),
        idxCount(0),
        vtxCount(0),
        firstIdx(0),
        vtxOffset(0)
    {
        vtx.reserve(100);
        idx.reserve(100);
    }

    void AddIndex(uint16_t i)
    {
        idx.emplace_back(i);
    }

    void AddVertex(const Vertex& v)
    {
        vtx.emplace_back(v);
    }

    void AddSolidColorBox(float x1, float y1, float z1, float x2, float y2, float z2, uint32_t c)
    {
        static std::array<uint16_t, 2 * 3 * 6> cubeIdx =
        {
             0,  1,  3,  3,  1,  2,
             5,  4,  6,  6,  4,  7,
             8,  9, 11, 11,  9, 10,
            13, 12, 14, 14, 12, 15,
            16, 17, 19, 19, 17, 18,
            21, 20, 22, 22, 20, 23
        };

        uint16_t firstVtx = (uint16_t)vtx.size();

        for (auto i: cubeIdx)
            AddIndex(firstVtx + i);

        // Generate a quad for each box face
        std::array<Vertex, 6 * 4> cubeVtx =
        {{
            { {x1, y2, z1, 1}, {}, {z1, x1} },
            { {x2, y2, z1, 1}, {}, {z1, x2} },
            { {x2, y2, z2, 1}, {}, {z2, x2} },
            { {x1, y2, z2, 1}, {}, {z2, x1} },

            { {x1, y1, z1, 1}, {}, {z1, x1} },
            { {x2, y1, z1, 1}, {}, {z1, x2} },
            { {x2, y1, z2, 1}, {}, {z2, x2} },
            { {x1, y1, z2, 1}, {}, {z2, x1} },

            { {x1, y1, z2, 1}, {}, {z2, y1} },
            { {x1, y1, z1, 1}, {}, {z1, y1} },
            { {x1, y2, z1, 1}, {}, {z1, y2} },
            { {x1, y2, z2, 1}, {}, {z2, y2} },

            { {x2, y1, z2, 1}, {}, {z2, y1} },
            { {x2, y1, z1, 1}, {}, {z1, y1} },
            { {x2, y2, z1, 1}, {}, {z1, y2} },
            { {x2, y2, z2, 1}, {}, {z2, y2} },

            { {x1, y1, z1, 1}, {}, {x1, y1} },
            { {x2, y1, z1, 1}, {}, {x2, y1} },
            { {x2, y2, z1, 1}, {}, {x2, y2} },
            { {x1, y2, z1, 1}, {}, {x1, y2} },

            { {x1, y1, z2, 1}, {}, {x1, y1} },
            { {x2, y1, z2, 1}, {}, {x2, y1} },
            { {x2, y2, z2, 1}, {}, {x2, y2} },
            { {x1, y2, z2, 1}, {}, {x1, y2} }
        }};

        for (const auto& vtx: cubeVtx)
        {
            // Add sometoken lighting to each vertex
            Vertex v = vtx;
            Vector3f p(v.pos[0], v.pos[1], v.pos[2]);
            float dist1 = (p - Vector3f(-2, 4, -2)).Length();
            float dist2 = (p - Vector3f( 3, 4, -3)).Length();
            float dist3 = (p - Vector3f(-4, 3, 25)).Length();
            float bri = ((std::rand() % 160) + 192.0f * (0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
            v.color[2] = (((c >>  0) & 0xff) / 255.0f) * bri; // red
            v.color[1] = (((c >>  8) & 0xff) / 255.0f) * bri; // green
            v.color[0] = (((c >> 16) & 0xff) / 255.0f) * bri; // blue
            v.color[3] = (((c >> 24) & 0xff) / 255.0f); // alpha
            AddVertex(v);
        }
    }

    void SizeBuffers(uint32_t* runningIdxCount, uint32_t* runningVtxCount)
    {
        idxCount = (uint32_t)idx.size();
        vtxCount = (uint32_t)vtx.size();
        firstIdx = *runningIdxCount;
        *runningIdxCount += idxCount;
        vtxOffset = *runningVtxCount;
        *runningVtxCount += vtxCount;
    }

    bool UploadBuffers(VertexBuffer<Vertex>& vb)
    {
        CHECK(vb.UpdateIndicies(idx.data(), idxCount, firstIdx));
        CHECK(vb.UpdateVertexes(vtx.data(), vtxCount, vtxOffset));

        return true;
    }

    void Render(const Matrix4f& vp, const PipelineLayout& pipeLayout, const VertexBuffer<Vertex>& vb)
    {
        Matrix4f mvp = vp * Matrix4f::Translation(pos);
        // Get back into column-major order
        mvp.Transpose();

        auto& cmd = Platform.CurrentDrawCmd();
        vkCmdPushConstants(cmd.buf, pipeLayout.pipeLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvp), &mvp.M[0][0]);

        tex.Bind(pipeLayout);

        // Bind index and vertex buffers at the Model's byte offsets
        VkDeviceSize offset = firstIdx * sizeof(uint16_t);
        vkCmdBindIndexBuffer(cmd.buf, vb.idxBuf, offset, VK_INDEX_TYPE_UINT16);

        offset = vtxOffset * sizeof(Vertex);
        vkCmdBindVertexBuffers(cmd.buf, 0, 1, &vb.vtxBuf, &offset);

        //Debug.Log("Drawing " + std::to_string(idxCount) + " indices at " + std::to_string(firstIdx) + " vertex offset " + std::to_string(vtxOffset));
        vkCmdDrawIndexed(cmd.buf, idxCount, 1, 0, 0, 0);
    }

    void Release()
    {
        vtx.clear();
        idx.clear();
    }
};

// A scene of models
class Scene: public VulkanObject
{
public:
    Image floorImage;
    Image wallImage;
    Image ceilingImage;
    Image whiteImage;
    Texture floorTexture;
    Texture wallTexture;
    Texture ceilingTexture;
    Texture whiteTexture;
    std::vector<Model> models;
    std::vector<Model> handModels;

    Scene() :
        floorImage(),
        wallImage(),
        ceilingImage(),
        whiteImage(),
        floorTexture(),
        wallTexture(),
        ceilingTexture(),
        whiteTexture(),
        models()
    {
    }

    Model* Add(Model& model)
    {
        models.emplace_back(model);
        return &models.back();
    }

    Model* Add(Model&& model)
    {
        models.emplace_back(model);
        return &models.back();
    }

    void Render(const Matrix4f& view, const Matrix4f& proj, const PipelineLayout& pipeLayout, const VertexBuffer<Vertex>& vb)
    {
        Matrix4f vp = proj * view;
        for (auto& model: models)
            model.Render(vp, pipeLayout, vb);
    }

    void RenderHand(int hand, const Matrix4f& view, const Matrix4f& proj, const PipelineLayout& pipeLayout, const VertexBuffer<Vertex>& vb)
    {
        Matrix4f vp = proj * view;
        handModels[hand].Render(vp, pipeLayout, vb);
    }

    bool Create(const PipelineLayout& pipeLayout, VertexBuffer<Vertex>& vb, bool includeIntensiveGPUobject = false)
    {
        CHECK(floorImage.Create(Image::Style::FLOOR, 256, 256));
        CHECK(wallImage.Create(Image::Style::WALL, 256, 256));
        CHECK(ceilingImage.Create(Image::Style::CEILING, 256, 256));
        CHECK(whiteImage.Create(Image::Style::WHITE, 256, 256));

        CHECK(floorTexture.Create(pipeLayout, floorImage));
        CHECK(wallTexture.Create(pipeLayout, wallImage));
        CHECK(ceilingTexture.Create(pipeLayout, ceilingImage));
        CHECK(whiteTexture.Create(pipeLayout, whiteImage));

        // Construct geometry

        handModels.emplace_back(Model(Vector3f(0, 0, 0), floorTexture));
        Model* m = &handModels.back();
        // Left is red
        m->AddSolidColorBox(-0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0xff801010);
        handModels.emplace_back(Model(Vector3f(0, 0, 0), floorTexture));
        m = &handModels.back();
        // Right is green
        m->AddSolidColorBox(-0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0xff108010);

        // Moving box
        m = Add(Model(Vector3f(0, 0, 0), ceilingTexture));
        m->AddSolidColorBox(0, 0, 0, +1.0f, +1.0f, 1.0f, 0xff404040);

        // Walls
        m = Add(Model(Vector3f(0, 0, 0), wallTexture));
        m->AddSolidColorBox(-10.1f, 0.0f, -20.0f, -10.0f, 4.0f, 20.0f, 0xff808080); // Left Wall
        m->AddSolidColorBox(-10.0f, -0.1f, -20.1f, 10.0f, 4.0f, -20.0f, 0xff808080); // Back Wall
        m->AddSolidColorBox(10.0f, -0.1f, -20.0f, 10.1f, 4.0f, 20.0f, 0xff808080); // Right Wall

        if (includeIntensiveGPUobject)
        {
            // More floors
            m = Add(Model(Vector3f(0, 0, 0), floorTexture));
            for (float depth = 0.0f; depth > -3.0f; depth -= 0.1f)
                m->AddSolidColorBox(9.0f, 0.5f, -depth, -9.0f, 3.5f, -depth, 0x10ff80ff); // Partition
        }

        // Floors
        m = Add(Model(Vector3f(0, 0, 0), floorTexture));
        m->AddSolidColorBox(-10.0f, -0.1f, -20.0f, 10.0f, 0.0f, 20.1f, 0xff808080); // Main floor
        m->AddSolidColorBox(-15.0f, -6.1f, 18.0f, 15.0f, -6.0f, 30.0f, 0xff808080); // Bottom floor

        // Ceiling
        m = Add(Model(Vector3f(0, 0, 0), ceilingTexture));
        m->AddSolidColorBox(-10.0f, 4.0f, -20.0f, 10.0f, 4.1f, 20.1f, 0xff808080);

        // Fixtures & furniture
        m = Add(Model(Vector3f(0, 0, 0), whiteTexture));
        m->AddSolidColorBox(9.5f, 0.75f, 3.0f, 10.1f, 2.5f, 3.1f, 0xff383838); // Right side shelf / Verticals
        m->AddSolidColorBox(9.5f, 0.95f, 3.7f, 10.1f, 2.75f, 3.8f, 0xff383838); // Right side shelf
        m->AddSolidColorBox(9.55f, 1.20f, 2.5f, 10.1f, 1.30f, 3.75f, 0xff383838); // Right side shelf / Horizontals
        m->AddSolidColorBox(9.55f, 2.00f, 3.05f, 10.1f, 2.10f, 4.2f, 0xff383838); // Right side shelf
        m->AddSolidColorBox(5.0f, 1.1f, 20.0f, 10.0f, 1.2f, 20.1f, 0xff383838); // Right railing
        m->AddSolidColorBox(-10.0f, 1.1f, 20.0f, -5.0f, 1.2f, 20.1f, 0xff383838); // Left railing
        for (float f = 5.0f; f <= 9.0f; f += 1.0f)
        {
            m->AddSolidColorBox(f, 0.0f, 20.0f, f + 0.1f, 1.1f, 20.1f, 0xff505050); // Left Bars
            m->AddSolidColorBox(-f, 1.1f, 20.0f, -f - 0.1f, 0.0f, 20.1f, 0xff505050); // Right Bars
        }
        m->AddSolidColorBox(-1.8f, 0.8f, 1.0f, 0.0f, 0.7f, 0.0f, 0xff505000); // Table
        m->AddSolidColorBox(-1.8f, 0.0f, 0.0f, -1.7f, 0.7f, 0.1f, 0xff505000); // Table Leg
        m->AddSolidColorBox(-1.8f, 0.7f, 1.0f, -1.7f, 0.0f, 0.9f, 0xff505000); // Table Leg
        m->AddSolidColorBox(0.0f, 0.0f, 1.0f, -0.1f, 0.7f, 0.9f, 0xff505000); // Table Leg
        m->AddSolidColorBox(0.0f, 0.7f, 0.0f, -0.1f, 0.0f, 0.1f, 0xff505000); // Table Leg
        m->AddSolidColorBox(-1.4f, 0.5f, -1.1f, -0.8f, 0.55f, -0.5f, 0xff202050); // Chair Set
        m->AddSolidColorBox(-1.4f, 0.0f, -1.1f, -1.34f, 1.0f, -1.04f, 0xff202050); // Chair Leg 1
        m->AddSolidColorBox(-1.4f, 0.5f, -0.5f, -1.34f, 0.0f, -0.56f, 0xff202050); // Chair Leg 2
        m->AddSolidColorBox(-0.8f, 0.0f, -0.5f, -0.86f, 0.5f, -0.56f, 0xff202050); // Chair Leg 2
        m->AddSolidColorBox(-0.8f, 1.0f, -1.1f, -0.86f, 0.0f, -1.04f, 0xff202050); // Chair Leg 2
        m->AddSolidColorBox(-1.4f, 0.97f, -1.05f, -0.8f, 0.92f, -1.10f, 0xff202050); // Chair Back high bar
        for (float f = 3.0f; f <= 6.6f; f += 0.4f)
            m->AddSolidColorBox(-3, 0.0f, f, -2.9f, 1.3f, f + 0.1f, 0xff404040); // Posts

        // Allocate and upload index and vertex buffer contents
        uint32_t idxCount = 0;
        uint32_t vtxCount = 0;
        for (auto& model: models)
        {
            model.SizeBuffers(&idxCount, &vtxCount);
        }
        for (auto& model: handModels)
        {
            model.SizeBuffers(&idxCount, &vtxCount);
        }
        vb.Create(idxCount, vtxCount);
        for (auto& model: models)
        {
            CHECK(model.UploadBuffers(vb));
        }
        for (auto& model: handModels)
        {
            CHECK(model.UploadBuffers(vb));
        }
        Debug.Log("Loaded " + std::to_string(models.size()) + " models (" + std::to_string(idxCount) + " indices and " + std::to_string(vtxCount) + " vertexes)");

        return true;
    }

    void Release()
    {
        floorTexture.Release();
        wallTexture.Release();
        ceilingTexture.Release();
        whiteTexture.Release();
        floorImage.Release();
        wallImage.Release();
        ceilingImage.Release();
        whiteImage.Release();
        models.clear();
        handModels.clear();
    }
};

#endif // Win32_VulkanAppUtil_h
