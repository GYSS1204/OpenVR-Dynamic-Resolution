#include <chrono>
#include <set>
#include <sstream>
#include <thread>
#include <cstdlib>
#include <algorithm>
#include <filesystem>

// OpenVR to interact with VR
#include <openvr.h>

// fmt for text formatting
#include <fmt/core.h>

// To include the nvml library at runtime
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include <dlfcn.h>
#endif


#define PRINTF printf

#include "get_info.h"
// Loading and saving .ini configuration file
#include "SimpleIni.h"
#include "setup.hpp"

// Dear ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "LanguageManager.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// Loading png
#include "lodepng.h"


// Tray icon
#include "tray.h"




enum class GpuType
{
    AMD,
    NVIDIA
};



extern bool vramMonitorEnabled;
extern float vramUsed; // Assume we always have free VRAM by default
HMODULE nvmlLibrary;
nvmlDevice_t nvmlDevice;
extern float vramUsedGB;
extern float vramTotalGB;
static constexpr const float bitsToGB = 1073741824;
static constexpr const float MBToGB = 1024;
bool nvmlEnabled = true;
bool ADLXEnabled = true;
extern bool GPUEnabled;
extern int gpuUsage;
GpuType gpuType;
extern float ramUsedGB;
extern float ramTotalGB;
extern float ramUsed;

int lastGPUUsage = 0;




//AMD

#include "../thirdparty/AMD/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "../thirdparty/AMD/SDK/Include/IPerformanceMonitoring.h"
#include "../thirdparty/AMD/SDK/Include/IPerformanceMonitoring2.h"
#include <iostream>

// ASCII °
static const signed char g_degree = 248;
ADLX_RESULT res = ADLX_FAIL;
static ADLXHelper g_ADLXHelp;
IADLXGPUPtr oneGPU;
IADLXPerformanceMonitoringServicesPtr perfMonitoringService;
void ShowCurrentGPUMetrics(IADLXPerformanceMonitoringServicesPtr perfMonitoringServices, IADLXGPUPtr oneGPU);
void ShowGPUMetricsRange(IADLXPerformanceMonitoringServicesPtr perfMonitoringServices, IADLXGPUPtr oneGPU);

int initAMDGPU();
int getAMDGPUinfo();



// NVIDIA
int initNVIDIAGPU();
void getNvidiaGPUinfo();
void cleanupNvidiaGPU();

void GetMemoryUsage();



bool isAMDGPU() {
    DISPLAY_DEVICE dd;
    dd.cb = sizeof(DISPLAY_DEVICE);

    for (int i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++) {
        std::string deviceName = dd.DeviceString;
        if (deviceName.find("AMD") != std::string::npos || deviceName.find("Radeon") != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool isNVIDIAGPU() {
    DISPLAY_DEVICE dd;
    dd.cb = sizeof(DISPLAY_DEVICE);

    for (int i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++) {
        std::string deviceName = dd.DeviceString;
        if (deviceName.find("NVIDIA") != std::string::npos) {
            return true;
        }
    }
    return false;
}






void initGetGPUInfo(){
    if (isAMDGPU()) {
        if(initAMDGPU() == 0){
            GPUEnabled = true;
            gpuType = GpuType::AMD;
        }
    } 
    else if (isNVIDIAGPU()) {
        if(initNVIDIAGPU() == 0){
            GPUEnabled = true;
            gpuType = GpuType::NVIDIA;
        };
    }
    else{
        GPUEnabled = false;
    }
}




void getGPUInfo() {
    if (GPUEnabled) {
        if(gpuType == GpuType::NVIDIA){
            getNvidiaGPUinfo();
        }
        else if(gpuType == GpuType::AMD){
            getAMDGPUinfo();
        }
        GetMemoryUsage();
    }
}



void cleanupGPU(){
    if (GPUEnabled) {
        if(gpuType == GpuType::NVIDIA){
            cleanupNvidiaGPU();
        }
        else if(gpuType == GpuType::AMD){
            
        }
    }
}


 void GetMemoryUsage(){
    // MEMORYSTATUSEX 用于存储内存状态信息
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    // 获取内存状态信息
    if (GlobalMemoryStatusEx(&memInfo)) {
        // 总内存大小（字节）
        DWORDLONG totalMemory = memInfo.ullTotalPhys;

        // 可用内存大小（字节）
        DWORDLONG availableMemory = memInfo.ullAvailPhys;

        // 已使用内存大小 = 总内存 - 可用内存
        DWORDLONG usedMemory = totalMemory - availableMemory;

        ramUsedGB = (double)usedMemory / bitsToGB;
        ramTotalGB = (double)totalMemory / bitsToGB;
        ramUsed = ramUsedGB / ramTotalGB;
    } else {
        // 错误处理
        printf("Failed to get memory status.\n");
    }
}



int initNVIDIAGPU(){


#pragma region Initialise NVML

		nvmlInit_t nvmlInitPtr;
#ifdef _WIN32
		nvmlLibrary = LoadLibraryA("nvml.dll");
		nvmlInitPtr = (nvmlInit_t)GetProcAddress(nvmlLibrary, "nvmlInit");
#else
		void *nvmlLibrary = dlopen("libnvidia-ml.so", RTLD_LAZY);
		nvmlInitPtr = (nvmlInit_t)dlsym(nvmlLibrary, "nvmlInit");
#endif
		if (!nvmlInitPtr)
		{
			nvmlEnabled = false;
            return -1;
		}
		else
		{
			nvmlReturn_t result;
			// Initialize NVML library
			result = nvmlInitPtr();
			if (result != NVML_SUCCESS)
			{
				nvmlEnabled = false;
			}
			else
			{
				// Get device handle
				nvmlDeviceGetHandleByIndex_t nvmlDeviceGetHandleByIndexPtr;
#ifdef _WIN32
				nvmlDeviceGetHandleByIndexPtr = (nvmlDeviceGetHandleByIndex_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetHandleByIndex");
#else
				nvmlDeviceGetHandleByIndexPtr = (nvmlDeviceGetHandleByIndex_t)dlsym(nvmlLibrary, "nvmlDeviceGetHandleByIndex");
#endif
				if (!nvmlDeviceGetHandleByIndexPtr)
					nvmlEnabled = false;
				else
					result = nvmlDeviceGetHandleByIndexPtr(0, &nvmlDevice);

				if (result != NVML_SUCCESS || !nvmlEnabled)
				{
					// Shutdown NVML
					nvmlShutdown_t nvmlShutdownPtr;
#ifdef _WIN32
					nvmlShutdownPtr = (nvmlShutdown_t)GetProcAddress(nvmlLibrary, "nvmlShutdown");
#else
					nvmlShutdownPtr = (nvmlShutdown_t)dlsym(nvmlLibrary, "nvmlShutdown");
#endif
					if (nvmlShutdownPtr)
						nvmlShutdownPtr();

                    return -1;
				}
                return 0;
			}
            return -1;
		}
}




void getNvidiaGPUinfo(){
    			// Get VRAM usage
			
			if (nvmlEnabled)
			{
				// Get GPU info
                nvmlDeviceGetUtilizationRates_t nvmlDeviceGetUtilizationRatesPtr;
				nvmlDeviceGetMemoryInfo_t nvmlDeviceGetMemoryInfoPtr;
#ifdef _WIN32
                nvmlDeviceGetUtilizationRatesPtr = (nvmlDeviceGetUtilizationRates_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetUtilizationRates");
				nvmlDeviceGetMemoryInfoPtr = (nvmlDeviceGetMemoryInfo_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetMemoryInfo");
#else
                nvmlDeviceGetUtilizationRatesPtr = (nvmlDeviceGetUtilizationRates_t)dlsym(nvmlLibrary, "nvmlDeviceGetUtilizationRates");
				nvmlDeviceGetMemoryInfoPtr = (nvmlDeviceGetMemoryInfo_t)dlsym(nvmlLibrary, "nvmlDeviceGetMemoryInfo");
#endif
				nvmlMemory_t nvmlMemory;
				if (nvmlDeviceGetMemoryInfoPtr(nvmlDevice, &nvmlMemory) != NVML_SUCCESS)
					nvmlEnabled = false;
				else
					vramTotalGB = nvmlMemory.total / bitsToGB;

				vramUsedGB = nvmlMemory.used / bitsToGB;
				if (nvmlEnabled) // Get the VRAM used in %
					vramUsed = (float)nvmlMemory.used / (float)nvmlMemory.total;
                
                nvmlUtilization_t utilization;
                if (nvmlDeviceGetUtilizationRatesPtr(nvmlDevice, &utilization) != NVML_SUCCESS) {
                    //std::cerr << "Failed to get utilization rates: " << nvmlErrorString(result) << std::endl;
                    nvmlEnabled = false;
                }
                else {
                    int currentgpuUsage = utilization.gpu;
                    gpuUsage = (currentgpuUsage + lastGPUUsage) / 2;
                    lastGPUUsage = currentgpuUsage;
                }
			}
}

void cleanupNvidiaGPU()
{
	// NVML cleanup
#ifdef _WIN32
	nvmlShutdown_t nvmlShutdownPtr = (nvmlShutdown_t)GetProcAddress(nvmlLibrary, "nvmlShutdown");
	if (nvmlShutdownPtr)
	{
		nvmlShutdownPtr();
	}
	FreeLibrary(nvmlLibrary);
#else
	nvmlShutdown_t nvmlShutdownPtr = (nvmlShutdown_t)dlsym(nvmlLibrary, "nvmlShutdown");
	if (nvmlShutdownPtr)
	{
		nvmlShutdownPtr();
	}
	dlclose(nvmlLibrary);
#endif
}









int initAMDGPU()
{

    // Initialize ADLX
    res = g_ADLXHelp.Initialize();

    if (ADLX_SUCCEEDED(res))
    {
        // Get Performance Monitoring services
        res = g_ADLXHelp.GetSystemServices()->GetPerformanceMonitoringServices(&perfMonitoringService);
        if (ADLX_SUCCEEDED(res))
        {
            IADLXGPUListPtr gpus;
            // Get GPU list
            res = g_ADLXHelp.GetSystemServices()->GetGPUs(&gpus);
            if (ADLX_SUCCEEDED(res))
            {
                // Use the first GPU in the list
                res = gpus->At(gpus->Begin(), &oneGPU);
                std::cout << "\tGet GPU Success " << std::endl;
                ShowGPUMetricsRange(perfMonitoringService, oneGPU);
                return 0;
            }
            else
                std::cout << "\tGet GPU list failed" << std::endl;
        }
        else
            std::cout << "\tGet performance monitoring services failed" << std::endl;
    }
    else
        std::cout << "\tADLX initialization failed" << std::endl;
        return -1;
}


int uninitializeADL()
{
        // Destroy ADLX
    res = g_ADLXHelp.Terminate();
    std::cout << "Destroy ADLX result: " << res << std::endl;
    return 0;
}




int getAMDGPUinfo() {

    if (ADLX_SUCCEEDED(res))
    {
        ShowCurrentGPUMetrics(perfMonitoringService, oneGPU);
        return 0;
    }
    else
    {
        std::cout << "\tGet particular GPU failed" << std::endl;
        return -1;
    }
}



// Use ADLX SDK example code

// Display the system time stamp (in ms)
int64_t GetTimeStamp(IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_int64 timeStamp = 0;
    ADLX_RESULT res = gpuMetrics->TimeStamp(&timeStamp);
    if (ADLX_SUCCEEDED(res))
    {
        //std::cout << "The GPU timp stamp is: " << timeStamp << "ms" << std::endl;
        return timeStamp;
    }
    return 0;
}

// Display GPU usage (in %)
double ShowGPUUsage(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Display GPU usage support status
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUUsage(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //std::cout << "GPU usage support status: " << supported << std::endl;
        if (supported)
        {
            adlx_double usage = 0;
            res = gpuMetrics->GPUUsage(&usage);
            if (ADLX_SUCCEEDED(res))
            {
                //std::cout << "The GPU usage is: " << usage << "%" << std::endl;
                return usage;

            }
        }
    }
    return 0;
}


// Display GPU VRAM (in MB)
int ShowGPUVRAM(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Display GPU VRAM support status
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUVRAM(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //std::cout << "GPU VRAM support status: " << supported << std::endl;
        if (supported)
        {
            adlx_int VRAM = 0;
            res = gpuMetrics->GPUVRAM(&VRAM);
            if (ADLX_SUCCEEDED(res)){
                //std::cout << "The GPU VRAM is: " << VRAM << "MB" << std::endl;
                return VRAM;
            }
                

        }
    }
    return 0;
}

// Display GPU temperature(in °C)
double ShowGPUTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;

    // Display the GPU temperature support status
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUTemperature(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //std::cout << "GPU temperature support status: " << supported << std::endl;
        if (supported)
        {
            adlx_double temperature = 0;
            res = gpuMetrics->GPUTemperature(&temperature);
            if (ADLX_SUCCEEDED(res))
                //std::cout << "The GPU temperature is: " << temperature << g_degree <<"C" << std::endl;
                return temperature;
        }
    }
    return 0;
}

// Display GPU hotspot temperature(in °C)
double ShowGPUHotspotTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;

    // Display GPU hotspot temperature support status
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUHotspotTemperature(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //std::cout << "GPU hotspot temperature support status: " << supported << std::endl;
        if (supported)
        {
            adlx_double hotspotTemperature = 0;
            res = gpuMetrics->GPUHotspotTemperature(&hotspotTemperature);
            if (ADLX_SUCCEEDED(res))
            {                
                //std::cout << "The GPU hotspot temperature is: " << hotspotTemperature << g_degree <<"C" << std::endl;
                return hotspotTemperature;
            }
        }
    }
    return 0;
}


// Display GPU total board power(in W)
double ShowGPUTotalBoardPower(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Display GPU total board power support status
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUTotalBoardPower(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //std::cout << "GPU total board power support status: " << supported << std::endl;
        if (supported)
        {
            adlx_double power = 0;
            res = gpuMetrics->GPUTotalBoardPower(&power);
            if (ADLX_SUCCEEDED(res))
            {
                //std::cout << "The GPU total board power is: " << power << "W" << std::endl;
                return power;

            }
        }
    }
    return 0;
}


// Display GPU metrics range
void ShowGPUMetricsRange(IADLXPerformanceMonitoringServicesPtr perfMonitoringServices, IADLXGPUPtr oneGPU)
{
    // Get GPU metrics support
    IADLXGPUMetricsSupportPtr gpuMetricsSupport;
    ADLX_RESULT res = perfMonitoringServices->GetSupportedGPUMetrics(oneGPU, &gpuMetricsSupport);
    if (ADLX_SUCCEEDED(res))
    {
        adlx_int minValue = 0, maxValue = 0;
        // Get GPU VRAM range
        res = gpuMetricsSupport->GetGPUVRAMRange(&minValue, &maxValue);
        if (ADLX_SUCCEEDED(res))
        {
            //std::cout << "The GPU VRAM range between " << minValue << "MB and " << maxValue << "MB" << std::endl;
            vramTotalGB = (float)maxValue / 1024;
        }
        else if (res == ADLX_NOT_SUPPORTED)
            std::cout << "Don't support GPU VRAM range" << std::endl;
        // Get GPU total board power range
        res = gpuMetricsSupport->GetGPUTotalBoardPowerRange(&minValue, &maxValue);
        if (ADLX_SUCCEEDED(res))
            std::cout << "The GPU total board power range between " << minValue << "W and " << maxValue << "W" << std::endl;
        else if (res == ADLX_NOT_SUPPORTED)
            std::cout << "Don't support GPU total board power range" << std::endl;

    }
}


// Display current GPU metrics
void ShowCurrentGPUMetrics(IADLXPerformanceMonitoringServicesPtr perfMonitoringServices, IADLXGPUPtr oneGPU)
{
    // Get GPU metrics support
    IADLXGPUMetricsSupportPtr gpuMetricsSupport;
    ADLX_RESULT res1 = perfMonitoringServices->GetSupportedGPUMetrics(oneGPU, &gpuMetricsSupport);

    // Accumulate and display current metrics for each of 10 loops
    IADLXGPUMetricsPtr gpuMetrics;
    // Get current GPU metrics
    ADLX_RESULT res2 = perfMonitoringServices->GetCurrentGPUMetrics(oneGPU, &gpuMetrics);

    // Display timestamp and GPU metrics
    if (ADLX_SUCCEEDED(res1) && ADLX_SUCCEEDED(res2))
    {
        //std::cout << "The current GPU metrics: " << std::endl;
        //std::cout << std::boolalpha;  // Display boolean variable as true or false
        //GetTimeStamp(gpuMetrics);

        int currentgpuUsage = (int)ShowGPUUsage(gpuMetricsSupport, gpuMetrics);
        gpuUsage = (currentgpuUsage + lastGPUUsage) / 2;
        lastGPUUsage = currentgpuUsage;

        //ShowGPUTemperature(gpuMetricsSupport, gpuMetrics);
        //ShowGPUHotspotTemperature(gpuMetricsSupport, gpuMetrics);
        //ShowGPUTotalBoardPower(gpuMetricsSupport, gpuMetrics);
        vramUsedGB = ShowGPUVRAM(gpuMetricsSupport, gpuMetrics) / MBToGB;
        vramUsed = vramUsedGB / vramTotalGB;
        //std::cout << std::noboolalpha;
    }

}













