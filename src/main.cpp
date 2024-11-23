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
#include "../include/adl_sdk.h"

// Tray icon
#include "tray.h"

#ifndef WIN32
#define HMODULE void *
#endif

#pragma region Modify InputText so we can use std::string
namespace ImGui
{
	// ImGui::InputText() with std::string
	// Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
	IMGUI_API bool InputText(const char *label, std::string *str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void *user_data = nullptr);
	IMGUI_API bool InputTextMultiline(const char *label, std::string *str, const ImVec2 &size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void *user_data = nullptr);
	IMGUI_API bool InputTextWithHint(const char *label, const char *hint, std::string *str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void *user_data = nullptr);
}
#pragma endregion

using namespace std::chrono_literals;
using namespace vr;

static constexpr const char *version = "v1.1.0";

static constexpr const char *iconPath = "icon.png";

static constexpr const std::chrono::milliseconds refreshIntervalBackground = 167ms; // 6fps
static constexpr const std::chrono::milliseconds refreshIntervalFocused = 33ms;		// 30fps

static constexpr const int mainWindowWidth = 350;
static constexpr const int mainWindowHeight = 500;

static constexpr const float bitsToGB = 1073741824;

GLFWwindow *glfwWindow;

bool trayQuit = false;

#pragma region Config
#pragma region Default settings
// Initialization
bool autoStart = 1;
int minimizeOnStart = 0;
int languageIndex = 0;
// General
int resChangeDelayMs = 3000;
int dataAverageSamples = 128;
std::string disabledApps = "steam.app.620980 steam.app.658920 steam.app.2177750 steam.app.2177760"; // Beat Saber and HL2VR
std::set<std::string> disabledAppsSet = {"steam.app.620980", "steam.app.658920", "steam.app.2177750", "steam.app.2177760"};
// Resolution
int initialRes = 100;
int minRes = 70;
int maxRes = 250;
int resIncreaseThreshold = 80;
int resDecreaseThreshold = 88;
int resIncreaseMin = 3;
int resDecreaseMin = 5;
int resIncreaseScale = 140;
int resDecreaseScale = 140;
float minCpuTimeThreshold = 0.6f;
bool resetOnThreshold = true;
// Reprojection
bool alwaysReproject = false;
bool preferReprojection = false;
bool ignoreCpuTime = false;
// VRAM
int vramTarget = 80;
int vramLimit = 90;
bool vramMonitorEnabled = true;
bool vramOnlyMode = false;
#pragma endregion



/// Newline-delimited string to a set
std::set<std::string> multilineStringToSet(const std::string &val)
{
	std::set<std::string> set;
	std::stringstream ss(val);
	std::string word;

	for (std::string line; std::getline(ss, line, '\n');)
		set.insert(line);

	return set;
}



/// Set to a space-delimited string
std::string setToConfigString(std::set<std::string> &valSet)
{
	std::string result;

	for (std::string val : valSet)
	{
		result += (val + " ");
	}
	if (!result.empty())
	{
		// remove trailing space
		result.pop_back();
	}

	return result;
}

bool loadSettings()
{
	// Get ini file
	CSimpleIniA ini;
	SI_Error rc = ini.LoadFile("settings.ini");
	if (rc < 0)
		return false;

	try
	{
		// Startup
		autoStart = std::stoi(ini.GetValue("Startup", "autoStart", std::to_string(autoStart).c_str()));
		minimizeOnStart = std::stoi(ini.GetValue("Startup", "minimizeOnStart", std::to_string(minimizeOnStart).c_str()));
		languageIndex = std::stoi(ini.GetValue("Startup", "languageIndex", std::to_string(languageIndex).c_str()));
		// General
		resChangeDelayMs = std::stoi(ini.GetValue("General", "resChangeDelayMs", std::to_string(resChangeDelayMs).c_str()));
		dataAverageSamples = std::stoi(ini.GetValue("General", "dataAverageSamples", std::to_string(dataAverageSamples).c_str()));
		if (dataAverageSamples > 128)
			dataAverageSamples = 128; // Max stored by OpenVR
		disabledApps = ini.GetValue("General", "disabledApps", disabledApps.c_str());
		std::replace(disabledApps.begin(), disabledApps.end(), ' ', '\n');
		disabledAppsSet = multilineStringToSet(disabledApps);
		// Resolution
		initialRes = std::stoi(ini.GetValue("Resolution", "initialRes", std::to_string(initialRes).c_str()));
		minRes = std::stoi(ini.GetValue("Resolution", "minRes", std::to_string(minRes).c_str()));
		maxRes = std::stoi(ini.GetValue("Resolution", "maxRes", std::to_string(maxRes).c_str()));
		resIncreaseThreshold = std::stoi(ini.GetValue("Resolution", "resIncreaseThreshold", std::to_string(resIncreaseThreshold).c_str()));
		resDecreaseThreshold = std::stoi(ini.GetValue("Resolution", "resDecreaseThreshold", std::to_string(resDecreaseThreshold).c_str()));
		resIncreaseMin = std::stoi(ini.GetValue("Resolution", "resIncreaseMin", std::to_string(resIncreaseMin).c_str()));
		resDecreaseMin = std::stoi(ini.GetValue("Resolution", "resDecreaseMin", std::to_string(resDecreaseMin).c_str()));
		resIncreaseScale = std::stoi(ini.GetValue("Resolution", "resIncreaseScale", std::to_string(resIncreaseScale).c_str()));
		resDecreaseScale = std::stoi(ini.GetValue("Resolution", "resDecreaseScale", std::to_string(resDecreaseScale).c_str()));
		minCpuTimeThreshold = std::stof(ini.GetValue("Resolution", "minCpuTimeThreshold", std::to_string(minCpuTimeThreshold).c_str()));
		resetOnThreshold = std::stoi(ini.GetValue("Resolution", "resetOnThreshold", std::to_string(resetOnThreshold).c_str()));
		// Reprojection
		alwaysReproject = std::stoi(ini.GetValue("Reprojection", "alwaysReproject", std::to_string(alwaysReproject).c_str()));
		preferReprojection = std::stoi(ini.GetValue("Reprojection", "preferReprojection", std::to_string(preferReprojection).c_str()));
		ignoreCpuTime = std::stoi(ini.GetValue("Reprojection", "ignoreCpuTime", std::to_string(ignoreCpuTime).c_str()));
		// VRAM
		vramMonitorEnabled = std::stoi(ini.GetValue("VRAM", "vramMonitorEnabled", std::to_string(vramMonitorEnabled).c_str()));
		vramOnlyMode = std::stoi(ini.GetValue("VRAM", "vramOnlyMode", std::to_string(vramOnlyMode).c_str()));
		vramTarget = std::stoi(ini.GetValue("VRAM", "vramTarget", std::to_string(vramTarget).c_str()));
		vramLimit = std::stoi(ini.GetValue("VRAM", "vramLimit", std::to_string(vramLimit).c_str()));
		return true;
	}
	catch (...)
	{
		return false;
	}
}

void saveSettings()
{
	// Get ini file
	CSimpleIniA ini;

	// Startup
	ini.SetValue("Startup", "autoStart", std::to_string(autoStart).c_str());
	ini.SetValue("Startup", "minimizeOnStart", std::to_string(minimizeOnStart).c_str());
	ini.SetValue("Startup", "languageIndex", std::to_string(languageIndex).c_str());
	// General
	ini.SetValue("General", "resChangeDelayMs", std::to_string(resChangeDelayMs).c_str());
	ini.SetValue("General", "dataAverageSamples", std::to_string(dataAverageSamples).c_str());
	ini.SetValue("General", "disabledApps", setToConfigString(disabledAppsSet).c_str());
	// Resolution
	ini.SetValue("Resolution", "initialRes", std::to_string(initialRes).c_str());
	ini.SetValue("Resolution", "minRes", std::to_string(minRes).c_str());
	ini.SetValue("Resolution", "maxRes", std::to_string(maxRes).c_str());
	ini.SetValue("Resolution", "resIncreaseThreshold", std::to_string(resIncreaseThreshold).c_str());
	ini.SetValue("Resolution", "resDecreaseThreshold", std::to_string(resDecreaseThreshold).c_str());
	ini.SetValue("Resolution", "resIncreaseMin", std::to_string(resIncreaseMin).c_str());
	ini.SetValue("Resolution", "resDecreaseMin", std::to_string(resDecreaseMin).c_str());
	ini.SetValue("Resolution", "resIncreaseScale", std::to_string(resIncreaseScale).c_str());
	ini.SetValue("Resolution", "resDecreaseScale", std::to_string(resDecreaseScale).c_str());
	ini.SetValue("Resolution", "minCpuTimeThreshold", std::to_string(minCpuTimeThreshold).c_str());
	ini.SetValue("Resolution", "resetOnThreshold", std::to_string(resetOnThreshold).c_str());
	// Reprojection
	ini.SetValue("Reprojection", "alwaysReproject", std::to_string(alwaysReproject).c_str());
	ini.SetValue("Reprojection", "preferReprojection", std::to_string(preferReprojection).c_str());
	ini.SetValue("Reprojection", "ignoreCpuTime", std::to_string(ignoreCpuTime).c_str());
	// VRAM
	ini.SetValue("VRAM", "vramMonitorEnabled", std::to_string(vramMonitorEnabled).c_str());
	ini.SetValue("VRAM", "vramOnlyMode", std::to_string(vramOnlyMode).c_str());
	ini.SetValue("VRAM", "vramTarget", std::to_string(vramTarget).c_str());
	ini.SetValue("VRAM", "vramLimit", std::to_string(vramLimit).c_str());

	// Save changes to disk
	ini.SaveFile("settings.ini");
}
#pragma endregion

#pragma region NVML
bool nvmlEnabled = true;

typedef enum nvmlReturn_enum
{
	NVML_SUCCESS = 0,					// The operation was successful.
	NVML_ERROR_UNINITIALIZED = 1,		// NVML was not first initialized with nvmlInit.
	NVML_ERROR_INVALID_ARGUMENT = 2,	// A supplied argument is invalid.
	NVML_ERROR_NOT_SUPPORTED = 3,		// The requested operation is not available on target device.
	NVML_ERROR_NO_PERMISSION = 4,		// The currrent user does not have permission for operation.
	NVML_ERROR_ALREADY_INITIALIZED = 5, // NVML has already been initialized.
	NVML_ERROR_NOT_FOUND = 6,			// A query to find an object was unccessful.
	NVML_ERROR_UNKNOWN = 7,				// An internal driver error occurred.
} nvmlReturn_t;
typedef nvmlReturn_t (*nvmlInit_t)();
typedef nvmlReturn_t (*nvmlShutdown_t)();
typedef struct
{
	unsigned long long total;
	unsigned long long free;
	unsigned long long used;
} nvmlMemory_t;
typedef nvmlReturn_t (*nvmlDevice_t)();
typedef nvmlReturn_t (*nvmlDeviceGetHandleByIndex_t)(unsigned int, nvmlDevice_t *);
typedef nvmlReturn_t (*nvmlDeviceGetMemoryInfo_t)(nvmlDevice_t, nvmlMemory_t *);
#ifdef _WIN32
typedef HMODULE(nvmlLib);
#else
typedef void *(nvmlLib);
#endif
#pragma endregion

long getCurrentTimeMillis()
{
	auto since_epoch = std::chrono::system_clock::now().time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
	return millis.count();
}

void pushGrayButtonColour()
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12, 0.12, 0.12, 12));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25, 0.25, 0.25, 1));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.17, 0.17, 0.17, 1));
}

void pushRedButtonColour()
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.62, 0.12, 0.12, 12));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.73, 0.25, 0.25, 1));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.68, 0.17, 0.17, 1));
}

void pushGreenButtonColour()
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12, 0.62, 0.12, 12));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25, 0.73, 0.25, 1));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.17, 0.68, 0.17, 1));
}

/**
 * Returns the current VR application key (steam.app.000000)
 * or an empty string if no app is running.
 */
std::string getCurrentApplicationKey()
{
	char applicationKey[vr::k_unMaxApplicationKeyLength];

	uint32_t processId = vr::VRApplications()->GetCurrentSceneProcessId();
	if (!processId)
		return "";

	EVRApplicationError err = vr::VRApplications()->GetApplicationKeyByProcessId(processId, applicationKey, vr::k_unMaxApplicationKeyLength);
	if (err)
		return "";

	return {applicationKey};
}

bool isApplicationDisabled(std::string appKey)
{
	return disabledAppsSet.find(appKey) != disabledAppsSet.end() || appKey == "";
}

bool shouldAdjustResolution(std::string appKey, bool manualRes, float cpuTime)
{
	// Check if the SteamVR dashboard is open
	bool inDashboard = vr::VROverlay()->IsDashboardVisible();
	// Check that we're in a supported application
	bool isCurrentAppDisabled = isApplicationDisabled(appKey);
	// Only adjust resolution if not in dashboard, in a supported application. user didn't pause res and cpu time isn't below threshold
	return !inDashboard && !isCurrentAppDisabled && !manualRes && !(resetOnThreshold && cpuTime < minCpuTimeThreshold);
}

void printLine(std::string text, long duration)
{
	long startTime = getCurrentTimeMillis();

	while (getCurrentTimeMillis() < startTime + duration && !glfwWindowShouldClose(glfwWindow))
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Create the Print window
		ImGui::Begin("printLine", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

		// Set position and size to fill the main window
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(mainWindowWidth, mainWindowHeight));

		// Display the line
		ImGui::TextWrapped("%s", text.c_str());

		// Stop creating the main window
		ImGui::End();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(glfwWindow);

		// Sleep to display the text for a set duration
		std::this_thread::sleep_for(refreshIntervalBackground);
	}
}

void cleanup(nvmlLib nvmlLibrary)
{
	// OpenVR cleanup
	vr::VR_Shutdown();

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

	// GUI cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

void addTooltip(const char *text)
{
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(mainWindowWidth - 4);
		ImGui::TextWrapped("%s", text);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

std::string executable_path;

std::string get_executable_path()
{
	return executable_path;
}

int main(int argc, char *argv[])
{
	 InitImGuiWithChineseFonts();
	executable_path = argc > 0 ? std::filesystem::absolute(std::filesystem::path(argv[0])).string() : "";
	auto& lang = LanguageManager::getInstance();
#pragma region GUI init
	if (!glfwInit())
		return 1;

	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Make window non-resizable
	glfwWindowHint(GLFW_RESIZABLE, false);

	// Create window with graphics context
	glfwWindow = glfwCreateWindow(mainWindowWidth, mainWindowHeight, fmt::format("OVR Dynamic Resolution {}", version).c_str(), nullptr, nullptr);
	if (glfwWindow == nullptr)
		return 1;
	glfwMakeContextCurrent(glfwWindow);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	// Don't save Dear ImGui window state
	io.IniFilename = NULL;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.03, 0.03, 0.03, 1));

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Set window icon
	GLFWimage icon;
	unsigned iconWidth, iconHeight;
	unsigned test = lodepng_decode32_file(&(icon.pixels), &(iconWidth), &(iconHeight), iconPath);
	icon.width = (int)iconWidth;
	icon.height = (int)iconHeight;
	glfwSetWindowIcon(glfwWindow, 1, &icon);
#pragma endregion

#pragma region VR init
	EVRInitError init_error;
	std::unique_ptr<IVRSystem, decltype(&shutdown_vr)> system(
		VR_Init(&init_error, VRApplication_Overlay), &shutdown_vr);
	if (init_error)
	{
		system = nullptr;
		printLine(VR_GetVRInitErrorAsEnglishDescription(init_error), 6000l);
		return EXIT_FAILURE;
	}
	if (!VRCompositor())
	{
		printLine("Failed to initialize VR compositor.", 6000l);
		return EXIT_FAILURE;
	}
#pragma endregion

	// Load settings from ini file
	if (!loadSettings())
		saveSettings(); // Restore settings
	setLanguage(languageIndex);

	// Set auto-start
	int autoStartResult = handle_setup(autoStart);
	if (autoStartResult != 0)
		printLine(fmt::format("Error toggling auto-start ({}) ", autoStartResult), 6000l);

	// Minimize or hide the window according to config
	if (minimizeOnStart == 1) // Minimize
		glfwIconifyWindow(glfwWindow);
	else if (minimizeOnStart == 2) // Hide
		glfwHideWindow(glfwWindow);

	// Make sure we can set resolution ourselves (Custom instead of Auto)
	vr::VRSettings()->SetInt32(vr::k_pch_SteamVR_Section,
							   vr::k_pch_SteamVR_SupersampleManualOverride_Bool, true);

	// Set default resolution
	vr::VRSettings()->SetFloat(vr::k_pch_SteamVR_Section,
							   vr::k_pch_SteamVR_SupersampleScale_Float, initialRes / 100.0f);

#pragma region Initialise NVML
	HMODULE nvmlLibrary;
	nvmlDevice_t nvmlDevice;
	float vramTotalGB;

	if (vramMonitorEnabled)
	{
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
				}
			}
		}
	}
	else
	{
		nvmlEnabled = false;
	}
#pragma endregion
#if defined(_WIN32)
	const char *hideToggleText = "Hide";
	if (minimizeOnStart == 2)
	{
		hideToggleText = "Show";
	}

	tray trayInstance = {
		"icon.ico",
		"OVR Dynamic Resolution",
		[](tray *trayInstance)
		{ trayInstance->menu->text = "Hide"; tray_update(trayInstance); glfwShowWindow(glfwWindow); },
		new tray_menu_item[5]{
			{hideToggleText, 0, 0, [](tray_menu_item *item)
			 { if (item->text == "Hide") { item->text = "Show"; tray_update(tray_get_instance()); glfwHideWindow(glfwWindow); } 
			   else { item->text = "Hide"; tray_update(tray_get_instance()); glfwShowWindow(glfwWindow); } }},
			{"-", 0, 0, nullptr},
			{"Quit", 0, 0, [](tray_menu_item *item)
			 { trayQuit = true; }},
			{nullptr, 0, 0, nullptr}}};

	tray_init(&trayInstance);

	std::thread trayThread([&]
						   { while(tray_loop(1) == 0); trayQuit = true; });
#endif // _WIN32

	// Initialize loop variables
	Compositor_FrameTiming *frameTiming = new vr::Compositor_FrameTiming[dataAverageSamples];
	long lastChangeTime = getCurrentTimeMillis() - resChangeDelayMs - 1;
	bool adjustResolution = true;
	bool openvrQuit = false;
	bool manualRes = false;

	// Resolution variables to display in GUI
	float averageGpuTime = 0;
	float averageCpuTime = 0;
	float averageFrameShown = 0;
	int newRes = 0;
	int targetFps = 0;
	float targetFrametime = 0;
	int hmdHz = 0;
	float hmdFrametime = 0;
	int currentFps = 0;
	float vramUsedGB = 0;

	// GUI variables
	bool showSettings = false;
	bool prevAutoStart = autoStart;

	// event loop
	while (!glfwWindowShouldClose(glfwWindow) && !openvrQuit && !trayQuit)
	{
		// Get current time
		long currentTime = getCurrentTimeMillis();

		// Doesn't run every loop
		if (currentTime - resChangeDelayMs > lastChangeTime)
		{
			lastChangeTime = currentTime;

#pragma region Getting data
			// Fetch resolution and target fps
			newRes = vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_SupersampleScale_Float) * 100;
			float lastRes = newRes;
			targetFps = std::round(vr::VRSystem()->GetFloatTrackedDeviceProperty(0, Prop_DisplayFrequency_Float));
			targetFrametime = 1000.0f / targetFps;
			hmdHz = targetFps;
			hmdFrametime = targetFrametime;

			// Define totals
			float totalGpuTime = 0;
			float totalCpuTime = 0;
			int frameShownTotal = 0;

			// Loop through past frames
			frameTiming->m_nSize = sizeof(Compositor_FrameTiming);
			vr::VRCompositor()->GetFrameTimings(frameTiming, dataAverageSamples);
			for (int i = 0; i < dataAverageSamples; i++)
			{
				// Get GPU frametime
				float gpuTime = frameTiming[i].m_flTotalRenderGpuMs;

				// Calculate CPU frametime
				// https://github.com/Louka3000/OpenVR-Dynamic-Resolution/issues/18#issuecomment-1833105172
				float cpuTime = frameTiming[i].m_flCompositorRenderCpuMs									 // Compositor
								+ (frameTiming[i].m_flNewFrameReadyMs - frameTiming[i].m_flNewPosesReadyMs); // Application & Late Start

				// How many times the current frame repeated (>1 = reprojecting)
				int frameShown = std::max((int)frameTiming[i].m_nNumFramePresents, 1);
				// Reason reprojection is happening
				int reprojectionFlag = frameTiming[i].m_nReprojectionFlags;

				// Add to totals
				totalGpuTime += gpuTime;
				totalCpuTime += cpuTime;
				frameShownTotal += frameShown;
			}

			// Calculate averages
			averageGpuTime = totalGpuTime / dataAverageSamples;
			averageCpuTime = totalCpuTime / dataAverageSamples;
			averageFrameShown = (float)frameShownTotal / (float)dataAverageSamples;

			// Estimated current FPS
			currentFps = targetFps / averageFrameShown;

			// Double the target frametime if the user wants to,
			// or if CPU Frametime is double the target frametime,
			// or if preferReprojection is true and CPU Frametime is greated than targetFrametime.
			if ((((averageCpuTime > targetFrametime && preferReprojection) ||
				  averageCpuTime / 2 > targetFrametime) &&
				 !ignoreCpuTime) ||
				alwaysReproject)
			{
				targetFps /= 2;
				targetFrametime *= 2;
			}

			// Get VRAM usage
			float vramUsed = 0; // Assume we always have free VRAM by default
			if (nvmlEnabled)
			{
				// Get memory info
				nvmlDeviceGetMemoryInfo_t nvmlDeviceGetMemoryInfoPtr;
#ifdef _WIN32
				nvmlDeviceGetMemoryInfoPtr = (nvmlDeviceGetMemoryInfo_t)GetProcAddress(nvmlLibrary, "nvmlDeviceGetMemoryInfo");
#else
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
			}
#pragma endregion

#pragma region Resolution adjustment
			// Get the current application key
			std::string appKey = getCurrentApplicationKey();
			adjustResolution = shouldAdjustResolution(appKey, manualRes, averageCpuTime);
			if (adjustResolution)
			{
				// Adjust resolution
				if ((averageCpuTime > minCpuTimeThreshold || vramOnlyMode))
				{
					// Frametime
					if (averageGpuTime < targetFrametime * (resIncreaseThreshold / 100.0f) && vramUsed < vramTarget / 100.0f && !vramOnlyMode)
					{
						// Increase resolution
						newRes += (((targetFrametime * (resIncreaseThreshold / 100.0f)) - averageGpuTime) *
								   (resIncreaseScale / 100.0f)) +
								  resIncreaseMin;
					}
					else if (averageGpuTime > targetFrametime * (resDecreaseThreshold / 100.0f) && !vramOnlyMode)
					{
						// Decrease resolution
						newRes -= ((averageGpuTime - (targetFrametime * (resDecreaseThreshold / 100.0f))) *
								   (resDecreaseScale / 100.0f)) +
								  resDecreaseMin;
					}

					// VRAM
					if (vramUsed > vramLimit / 100.0f)
					{
						// Force the resolution to decrease when the vram limit is reached
						newRes -= resDecreaseMin;
					}
					else if (vramOnlyMode && newRes < initialRes && vramUsed < vramTarget / 100.0f)
					{
						// When in VRAM-only mode, make sure the res goes back up when possible.
						newRes = std::min(initialRes, newRes + resIncreaseMin);
					}

					// Clamp the new resolution
					newRes = std::clamp(newRes, minRes, maxRes);
				}
			}
			else if ((appKey == "" || (resetOnThreshold && averageCpuTime < minCpuTimeThreshold)) && !manualRes)
			{
				// If (in SteamVR void or cpuTime below threshold) and user didn't pause res
				// Reset to initialRes
				newRes = initialRes;
			}

			if (newRes != lastRes)
			{
				// Sets the new resolution
				vr::VRSettings()->SetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_SupersampleScale_Float, newRes / 100.0f);
			}
		}
#pragma endregion

#pragma region Gui rendering
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Make sure buttons are gray
		pushGrayButtonColour();

#pragma region Main window
		if (!showSettings)
		{
			// Create the main window
			ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

			// Set position and size to fill the viewport
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::SetWindowSize(ImVec2(mainWindowWidth, mainWindowHeight));

			// Title
			ImGui::Text("%s", LanguageManager::getInstance().translate("app_title").c_str());

			ImGui::Separator();
			ImGui::NewLine();

			// 使用 UTF-8 字面量
			// HMD Hz
			ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("hmd_refresh_rate"), hmdHz, hmdFrametime).c_str());
			// 在渲染代码中使用翻译
			// Target FPS and frametime
			if (!vramOnlyMode)
			{
				ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("target_fps"), targetFps, targetFrametime).c_str());
			}
			else
			{
				ImGui::Text("%s", LanguageManager::getInstance().translate("target_fps_disabled").c_str());
			}
			// VRAM target and limit
			if (nvmlEnabled && nvmlEnabled)
			{
				ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("target_VRAM"), vramTarget / 100.0f * vramTotalGB).c_str());
				ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("limit_VRAM"), vramLimit / 100.0f * vramTotalGB).c_str());
			}
			else{
				ImGui::Text("%s", LanguageManager::getInstance().translate("target_VRAM_disabled").c_str());
				ImGui::Text("%s", LanguageManager::getInstance().translate("limit_VRAM_disabled").c_str());
			}

			ImGui::NewLine();

			ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("FPS").c_str(), currentFps).c_str());
			ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("GPU_frametime").c_str(), averageGpuTime).c_str());
			ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("CPU_frametime").c_str(), averageCpuTime).c_str());

			// VRAM usage
			if (nvmlEnabled)
				ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("VRAM_usage").c_str(), vramUsedGB).c_str());
			else
				ImGui::Text("%s", LanguageManager::getInstance().translate("VRAM_usage_disabled").c_str());

			ImGui::NewLine();

			// Reprojection ratio
			ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("Reprojection_ratio"), averageFrameShown - 1).c_str());
			// Current resolution
			if (manualRes)
			{
				ImGui::Text("%s", LanguageManager::getInstance().translate("Resolution_info_2").c_str());
			}
			else
			{
				ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("Resolution_info"), newRes).c_str());
			}

			// Resolution adjustment status
			if (!adjustResolution)
			{
				ImGui::SameLine(0, 10);
				if (manualRes)
				{
					ImGui::PushItemWidth(192);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
					if (ImGui::SliderInt("", &newRes, 20, 500, "%d", ImGuiSliderFlags_AlwaysClamp))
					{
						vr::VRSettings()->SetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_SupersampleScale_Float, newRes / 100.0f);
					}
					ImGui::PopStyleVar();
				}
				else
				{
					ImGui::Text("%s", lang.translate("Adjustment_paused").c_str());
				}
			}

			ImGui::NewLine();

			// Open settings
			bool settingsPressed = ImGui::Button(lang.translate("Settings").c_str(), ImVec2(82, 28));
			if (settingsPressed)
				showSettings = true;

			// Resolution pausing
			ImGui::SameLine();

			std::string pauseText = lang.translate(manualRes ? "Dynamic_resolution" : "Manual_resolution");
			bool pausePressed = ImGui::Button(pauseText.c_str(), ImVec2(142, 28));
			if (pausePressed)
			{
				manualRes = !manualRes;
				adjustResolution = shouldAdjustResolution(getCurrentApplicationKey(), manualRes, averageCpuTime);
			}

			// Stop creating the main window
			ImGui::End();
		}
#pragma endregion


#pragma region Settings window
if (showSettings)
{
    // Create the settings window
    ImGui::Begin(LanguageManager::getInstance().translate("Settings").c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Set position and size to fill the viewport
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(mainWindowWidth, mainWindowHeight));

    // Set the labels' width
    ImGui::PushItemWidth(96);

    // Title
    ImGui::Text(LanguageManager::getInstance().translate("Settings").c_str());

    ImGui::Separator();
    ImGui::NewLine();

    // GUI settings inputs
    if (ImGui::CollapsingHeader(LanguageManager::getInstance().translate("Startup").c_str()))
    {
        ImGui::Checkbox(LanguageManager::getInstance().translate("Start_with_SteamVR").c_str(), &autoStart);
        addTooltip(LanguageManager::getInstance().translate("Tooltip_start_with_SteamVR").c_str());

        ImGui::Text(LanguageManager::getInstance().translate("Startup_behavior").c_str());
        ImGui::RadioButton(LanguageManager::getInstance().translate("Visible").c_str(), &minimizeOnStart, 0);
        addTooltip(LanguageManager::getInstance().translate("Tooltip_visible").c_str());
        ImGui::RadioButton(LanguageManager::getInstance().translate("Minimized_taskbar").c_str(), &minimizeOnStart, 1);
        addTooltip(LanguageManager::getInstance().translate("Tooltip_minimized_taskbar").c_str());
        ImGui::RadioButton(LanguageManager::getInstance().translate("Hidden_tray").c_str(), &minimizeOnStart, 2);
        addTooltip(LanguageManager::getInstance().translate("Tooltip_hidden_tray").c_str());
    }

	if (ImGui::CollapsingHeader(LanguageManager::getInstance().translate("General").c_str()))
	{
		// 添加语言选择的下拉框
		static const char* languages[] = { "English", "中文" }; // 可扩展的语言选项
		static int selectedLanguageIndex = languageIndex; // 初始选择索引，0对应English
		// 显示下拉框并更新语言索引
		if (ImGui::Combo(LanguageManager::getInstance().translate("Select_language").c_str(), &selectedLanguageIndex, languages, IM_ARRAYSIZE(languages)))
		{
			languageIndex = selectedLanguageIndex;
			// 更新语言代码，根据索引选择语言
			setLanguage(languageIndex);
		}
		addTooltip(LanguageManager::getInstance().translate("Tooltip_select_language").c_str());

		// 其他设置项
		if (ImGui::InputInt(LanguageManager::getInstance().translate("Resolution_change_delay_ms").c_str(), &resChangeDelayMs, 100))
			resChangeDelayMs = std::max(resChangeDelayMs, 100);
		addTooltip(LanguageManager::getInstance().translate("Tooltip_resolution_change_delay_ms").c_str());

		if (ImGui::InputInt(LanguageManager::getInstance().translate("Data_average_samples").c_str(), &dataAverageSamples, 2))
		{
			dataAverageSamples = std::clamp(dataAverageSamples, 1, 128); // Max stored by OpenVR
			frameTiming = new vr::Compositor_FrameTiming[dataAverageSamples];
		}
		addTooltip(LanguageManager::getInstance().translate("Tooltip_data_average_samples").c_str());

		if (ImGui::InputTextMultiline(LanguageManager::getInstance().translate("Disable_application").c_str(), &disabledApps, ImVec2(130, 60)))
			disabledAppsSet = multilineStringToSet(disabledApps);
		addTooltip(LanguageManager::getInstance().translate("Tooltip_disable_current_application").c_str());

		if (ImGui::Button(LanguageManager::getInstance().translate("Disable_current_application").c_str(), ImVec2(200, 26)))
		{
			std::string appKey = getCurrentApplicationKey();
			if (!isApplicationDisabled(appKey))
			{
				disabledAppsSet.insert(appKey);
				disabledApps += "\n" + appKey;
			}
		}
		addTooltip(LanguageManager::getInstance().translate("Tooltip_disable_current_application").c_str());
	}

    if (ImGui::CollapsingHeader(LanguageManager::getInstance().translate("Resolution").c_str()))
    {
        if (ImGui::InputInt(LanguageManager::getInstance().translate("Initial_resolution").c_str(), &initialRes, 5))
            initialRes = std::clamp(initialRes, 20, 500);
        addTooltip(LanguageManager::getInstance().translate("Tooltip_initial_resolution").c_str());

        if (ImGui::InputInt(LanguageManager::getInstance().translate("Minimum_resolution").c_str(), &minRes, 5))
            minRes = std::clamp(minRes, 20, 500);
        addTooltip(LanguageManager::getInstance().translate("Tooltip_minimum_resolution").c_str());

        if (ImGui::InputInt(LanguageManager::getInstance().translate("Maximum_resolution").c_str(), &maxRes, 5))
            maxRes = std::clamp(maxRes, 20, 500);
        addTooltip(LanguageManager::getInstance().translate("Tooltip_maximum_resolution").c_str());

        if (ImGui::TreeNodeEx(LanguageManager::getInstance().translate("Advanced").c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen))
        {
            if (ImGui::InputInt(LanguageManager::getInstance().translate("Increase_threshold").c_str(), &resIncreaseThreshold, 1))
                resIncreaseThreshold = std::max(resIncreaseThreshold, 0);
            addTooltip(LanguageManager::getInstance().translate("Tooltip_increase_threshold").c_str());

            if (ImGui::InputInt(LanguageManager::getInstance().translate("Decrease_threshold").c_str(), &resDecreaseThreshold, 1))
                resDecreaseThreshold = std::max(resDecreaseThreshold, 0);
            addTooltip(LanguageManager::getInstance().translate("Tooltip_decrease_threshold").c_str());

            ImGui::InputInt(LanguageManager::getInstance().translate("Increase_minimum").c_str(), &resIncreaseMin, 1);
            addTooltip(LanguageManager::getInstance().translate("Tooltip_increase_minimum").c_str());

            ImGui::InputInt(LanguageManager::getInstance().translate("Decrease_minimum").c_str(), &resDecreaseMin, 1);
            addTooltip(LanguageManager::getInstance().translate("Tooltip_decrease_minimum").c_str());

            ImGui::InputInt(LanguageManager::getInstance().translate("Increase_scale").c_str(), &resIncreaseScale, 10);
            addTooltip(LanguageManager::getInstance().translate("Tooltip_increase_scale").c_str());

            ImGui::InputInt(LanguageManager::getInstance().translate("Decrease_scale").c_str(), &resDecreaseScale, 10);
            addTooltip(LanguageManager::getInstance().translate("Tooltip_decrease_scale").c_str());

            ImGui::InputFloat(LanguageManager::getInstance().translate("Minimum_CPU_time_threshold").c_str(), &minCpuTimeThreshold, 0.1);
            addTooltip(LanguageManager::getInstance().translate("Tooltip_minimum_CPU_time_threshold").c_str());

            ImGui::Checkbox(LanguageManager::getInstance().translate("Reset_on_CPU_time_threshold").c_str(), &resetOnThreshold);
            addTooltip(LanguageManager::getInstance().translate("Tooltip_reset_on_CPU_time_threshold").c_str());
        }
    }
		if (ImGui::CollapsingHeader(LanguageManager::getInstance().translate("header_reprojection").c_str()))
		{
			ImGui::Checkbox(LanguageManager::getInstance().translate("Always_reproject").c_str(), &alwaysReproject);
			addTooltip(LanguageManager::getInstance().translate("Tooltip_always_reproject").c_str());

			ImGui::Checkbox(LanguageManager::getInstance().translate("Prefer_reprojection").c_str(), &preferReprojection);
			addTooltip(LanguageManager::getInstance().translate("Tooltip_prefer_reprojection").c_str());

			ImGui::Checkbox(LanguageManager::getInstance().translate("Ignore_CPU_time").c_str(), &ignoreCpuTime);
			addTooltip(LanguageManager::getInstance().translate("Tooltip_ignore_CPU_time").c_str());
		}

		if (ImGui::CollapsingHeader(LanguageManager::getInstance().translate("VRAM").c_str()))
		{
			ImGui::Checkbox(LanguageManager::getInstance().translate("VRAM_monitor_enabled").c_str(), &vramMonitorEnabled);
			addTooltip(LanguageManager::getInstance().translate("Tooltip_vram_monitor").c_str());

			ImGui::Checkbox(LanguageManager::getInstance().translate("VRAM-only_mode").c_str(), &vramOnlyMode);
			addTooltip(LanguageManager::getInstance().translate("Tooltip_VRAM-only_mode").c_str());

			if (ImGui::InputInt(LanguageManager::getInstance().translate("VRAM_target").c_str(), &vramTarget, 2))
				vramTarget = std::clamp(vramTarget, 0, 100);
			addTooltip(LanguageManager::getInstance().translate("Tooltip_vram_target").c_str());

			if (ImGui::InputInt(LanguageManager::getInstance().translate("VRAM_limit").c_str(), &vramLimit, 2))
				vramLimit = std::clamp(vramLimit, 0, 100);
			addTooltip(LanguageManager::getInstance().translate("Tooltip_vram_limit").c_str());
		}


    // Buttons
    bool closePressed = ImGui::Button(LanguageManager::getInstance().translate("Close").c_str(), ImVec2(82, 28));
    if (closePressed)
    {
        showSettings = false;
    }
    ImGui::SameLine();
    pushRedButtonColour();
    bool revertPressed = ImGui::Button(LanguageManager::getInstance().translate("Revert").c_str(), ImVec2(82, 28));
    if (revertPressed)
    {
        int oldDataAverageSamples = dataAverageSamples;
        loadSettings();
        if (oldDataAverageSamples != dataAverageSamples)
            frameTiming = new vr::Compositor_FrameTiming[dataAverageSamples];
    }
    ImGui::SameLine();
    pushGreenButtonColour();
    bool savePressed = ImGui::Button(LanguageManager::getInstance().translate("Save").c_str(), ImVec2(82, 28));
    if (savePressed)
    {
        saveSettings();
        if (prevAutoStart != autoStart)
        {
            handle_setup(autoStart);
            prevAutoStart = autoStart;
        }
    }

    // Stop creating the settings window
    ImGui::End();
}
#pragma endregion

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(glfwWindow);
#pragma endregion

		// Check if OpenVR is quitting so we can quit alongside it
		VREvent_t vrEvent;
		while (vr::VRSystem()->PollNextEvent(&vrEvent, sizeof(vr::VREvent_t)))
		{
			if (vrEvent.eventType == vr::VREvent_Quit)
			{
				vr::VRSystem()->AcknowledgeQuit_Exiting();
				openvrQuit = true;
				break;
			}
		}

		// Calculate how long to sleep for depending on if the window is focused or not.
		std::chrono::milliseconds sleepTime;
		if (glfwGetWindowAttrib(glfwWindow, GLFW_FOCUSED))
			sleepTime = refreshIntervalFocused;
		else
			sleepTime = refreshIntervalBackground;

		// ZZzzzz
		std::this_thread::sleep_for(sleepTime);
	}

	cleanup(nvmlLibrary);

#if defined(_WIN32)
	tray_exit();
#endif // _WIN32

	return 0;
}

// Custom WinMain method to not have the terminal
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	main(NULL, NULL);
}
#endif
