#include "App.h"
#include "HardwareSettings.h"
#include "SDL2/SDL.h"


HardwareInfo::HardwareInfo(App* app) : Module(app)
{
	SDL_version version;
	SDL_GetVersion(&version);
	sprintf_s(settings.sdlVersion, "%d.%d.%d", version.major, version.minor, version.patch);

	settings.ram = (float)SDL_GetSystemRAM() / 1024;
	settings.cpuCores = SDL_GetCPUCount();
	settings.cacheLineSize = SDL_GetCPUCacheLineSize();
	settings.rdtsc = SDL_HasRDTSC() == SDL_TRUE;
	settings.altiVec = SDL_HasAltiVec() == SDL_TRUE;
	settings.now3d = SDL_Has3DNow() == SDL_TRUE;
	settings.mmx = SDL_HasMMX() == SDL_TRUE;
	settings.sse = SDL_HasSSE() == SDL_TRUE;
	settings.sse2 = SDL_HasSSE2() == SDL_TRUE;
	settings.sse3 = SDL_HasSSE3() == SDL_TRUE;
	settings.sse41 = SDL_HasSSE41() == SDL_TRUE;	
	settings.sse42 = SDL_HasSSE42() == SDL_TRUE;
	settings.avx = SDL_HasAVX() == SDL_TRUE;
	settings.avx2 = SDL_HasAVX2() == SDL_TRUE;
}

HardwareInfo::~HardwareInfo()
{
}

bool HardwareInfo::Awake()
{
	return true;
}

bool HardwareInfo::Start()
{
	return true;
}

bool HardwareInfo::CleanUp()
{
	return true;
}

const HardwareSettings& HardwareInfo::GetSettings() const
{
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);	
	const GLubyte* driver = glGetString(GL_VERSION);

	GLint vramBudget = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vramBudget);

	GLint vramAvaliable = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vramAvaliable);

	GLint vramUsage = 0;
	vramUsage = vramBudget - vramAvaliable;

	settings.gpuVendor.assign((const char*)vendor);
	settings.gpuBrand.assign((const char*)renderer);
	settings.gpuDriverVersion.assign((const char*)driver);

	settings.vramBudget = float(vramBudget) / 1024;
	settings.vramAvailable = float(vramAvaliable) / 1024;
	settings.vramUsage = float(vramUsage) / 1024;

	return settings;
}