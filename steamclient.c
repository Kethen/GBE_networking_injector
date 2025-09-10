#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include <pthread.h>
#include <time.h>

#include <MinHook.h>

#include "log.h"

#include <windows.h>

HMODULE goldberg = NULL;
HMODULE steam_api = NULL;

#define CALLBACK_THREAD_OVERRIDE 0
#define CALLBACK_THREAD_RATE 75

unsigned int (*SteamAPI_Init_orig)() = NULL;
unsigned int (*SteamAPI_Init_gb)() = NULL;
unsigned int SteamAPI_Init(){
	LOG("%s: goldberg init 0x%x\n", __func__, SteamAPI_Init_gb);
	LOG("%s: SteamAPI_Init_orig 0x%x\n", __func__, SteamAPI_Init_orig());
	LOG("%s: SteamAPI_Init_gb 0x%x\n", __func__, SteamAPI_Init_gb());	
	return true;
}

void *(*SteamUser_gb)() = NULL;
void *SteamUser(){
	void *ret = SteamUser_gb();
	static bool logged = false;
	if (!logged){
		logged = true;
		LOG("%s: SteamUser instance 0x%x\n", __func__, ret);
	}
	return ret;
}

void *(*SteamMatchmaking_gb)() = NULL;
void *SteamMatchmaking(){
	void *ret = SteamMatchmaking_gb();
	static bool logged = false;
	if (!logged){
		logged = true;
		LOG("%s: SteamMatchmaking instance 0x%x\n", __func__, ret);
	}
	return ret;
}

void *(*SteamNetworking_gb)() = NULL;
void *SteamNetworking(){
	void *ret = SteamNetworking_gb();
	static bool logged = false;
	if (!logged){
		logged = true;
		LOG("%s: SteamNetworking instance 0x%x\n", __func__, ret);
	}
	return ret;
}

void *(*SteamFriends_gb)() = NULL;
void *SteamFriends(){
	void *ret = SteamFriends_gb();
	static bool logged = false;
	if (!logged){
		logged = true;
		LOG("%s: SteamFriends instance 0x%x\n", __func__, ret);
	}
	return ret;
}


void *(*SteamUserStats_gb)() = NULL;
void *SteamUserStats(){
	void *ret = SteamUserStats_gb();
	static bool logged = false;
	if (!logged){
		logged = true;
		LOG("%s: SteamUserStats instance 0x%x\n", __func__, ret);
	}
	return ret;
}

void (*SteamAPI_RunCallbacks_orig)() = NULL;
void (*SteamAPI_RunCallbacks_gb)() = NULL;
void SteamAPI_RunCallbacks(){
	#if CALLBACK_THREAD_OVERRIDE
	#else
	SteamAPI_RunCallbacks_gb();
	#endif
}

void redirect_functions(){
	steam_api = LoadLibraryA("steam_api.dll");
	if (steam_api == NULL){
		LOG("%s: failed loading steam_api.dll, terminating\n", __func__);
		exit(1);
	}

	goldberg = LoadLibraryA("goldberg.bsi");
	if (goldberg == NULL){
		goldberg = LoadLibraryA("goldberg64.bsi");
	}
	if (goldberg == NULL){
		LOG("%s: failed loading goldberg.bsi, terminating\n", __func__);
		exit(1);
	}

	MH_STATUS init_status = MH_Initialize();
	if (init_status != MH_OK && init_status != MH_ERROR_ALREADY_INITIALIZED){
		LOG("%s: minhook init failed, %d\n", __func__, init_status);
		exit(1);
	}

	#define STR(s) #s
	#define HOOK_DIRECT(name) { \
		void *orig = (void *)GetProcAddress(steam_api, STR(name)); \
		if (orig == NULL){ \
			LOG("%s: failed fetching %s from steam_api.dll\n", __func__, STR(name)); \
			exit(1); \
		} \
		void *gb = (void *)GetProcAddress(goldberg, STR(name)); \
		if (gb == NULL){ \
			LOG("%s: failed fetching %s from goldberg\n", __func__, STR(name)); \
			exit(1); \
		} \
		MH_STATUS ret = MH_CreateHook(orig, gb, NULL); \
		if (ret != MH_OK){ \
			LOG("%s: failed creating hook for %s, %d\n", __func__, STR(name), ret); \
			exit(1); \
		} \
		ret = MH_EnableHook(orig); \
		if (ret != MH_OK){ \
			LOG("%s: failed enabling hook for %s, %d\n", __func__, STR(name), ret); \
			exit(1); \
		} \
		LOG("%s: directly hooked %s\n", __func__, STR(name)); \
	}

	//HOOK_DIRECT(SteamUser);
	//HOOK_DIRECT(SteamMatchmaking);
	//HOOK_DIRECT(SteamFriends);
	//HOOK_DIRECT(SteamApps);
	//HOOK_DIRECT(SteamRemoteStorage);
	//HOOK_DIRECT(SteamUserStats);
	//HOOK_DIRECT(SteamNetworking);
	//HOOK_DIRECT(SteamUtils);

	HOOK_DIRECT(SteamAPI_RegisterCallback);
	HOOK_DIRECT(SteamAPI_UnregisterCallResult);
	HOOK_DIRECT(SteamAPI_RegisterCallResult);
	HOOK_DIRECT(SteamAPI_UnregisterCallback);
	//HOOK_DIRECT(SteamAPI_RunCallbacks);

	//HOOK_DIRECT(SteamAPI_RestartAppIfNecessary);
	//HOOK_DIRECT(SteamAPI_GetHSteamPipe);
	//HOOK_DIRECT(SteamAPI_GetHSteamUser);
	//HOOK_DIRECT(SteamInternal_CreateInterface);

	#define HOOK(name, detour, trampoline){ \
		void *orig = (void *)GetProcAddress(steam_api, STR(name)); \
		if (orig == NULL){ \
			LOG("%s: failed fetching %s from steam_api.dll\n", __func__, STR(name)); \
			exit(1); \
		} \
		MH_STATUS ret = MH_CreateHook(orig, detour, (void *)trampoline); \
		if (ret != MH_OK){ \
			LOG("%s: failed creating hook for %s, %d\n", __func__, STR(name), ret); \
			exit(1); \
		} \
		ret = MH_EnableHook(orig); \
		if (ret != MH_OK){ \
			LOG("%s: failed enabling hook for %s, %d\n", __func__, STR(name), ret); \
			exit(1); \
		} \
		LOG("%s: hooked %s\n", __func__, STR(name)); \
	}

	//HOOK(SteamAPI_Init, SteamAPI_Init, &SteamAPI_Init_orig);
	HOOK(SteamUser, SteamUser, NULL);
	HOOK(SteamAPI_RunCallbacks, SteamAPI_RunCallbacks, &SteamAPI_RunCallbacks_orig);
	HOOK(SteamMatchmaking, SteamMatchmaking, NULL);
	HOOK(SteamNetworking, SteamNetworking, NULL);
	HOOK(SteamFriends, SteamFriends, NULL);
	HOOK(SteamUserStats, SteamUserStats, NULL);

	#define GET_GB_FUNCTION(name){ \
		name##_gb = (void *)GetProcAddress(goldberg, STR(name)); \
		if (name##_gb == NULL){ \
			LOG("%s: failed fetching %s from goldberg\n", __func__, STR(name)); \
			exit(1); \
		} \
	}

	GET_GB_FUNCTION(SteamAPI_Init);
	GET_GB_FUNCTION(SteamUser);
	GET_GB_FUNCTION(SteamAPI_RunCallbacks);
	GET_GB_FUNCTION(SteamMatchmaking);
	GET_GB_FUNCTION(SteamNetworking);
	GET_GB_FUNCTION(SteamFriends);
	GET_GB_FUNCTION(SteamUserStats);
}

#if CALLBACK_THREAD_OVERRIDE
void *callback_thread_func(void *arg){
	LOG("%s: begin\n", __func__);
	struct timespec wait_time = {
		.tv_sec = 0,
		.tv_nsec = 1000000000 / CALLBACK_THREAD_RATE
	};

	while(true){
		SteamAPI_RunCallbacks_gb();
		nanosleep(&wait_time, NULL);
	}

	return NULL;
}
#endif

void *CreateInterface(const char *pName, int *pReturnCode)
{
	SteamAPI_Init_gb();
    void *(*create_interface_gb)(const char*) = (void *)GetProcAddress(goldberg, "SteamInternal_CreateInterface");
	// make a goldberg client to back the networking and user interfaces
	void *goldberg_client = create_interface_gb(pName);
	LOG("%s: created goldberg client 0x%x version %s\n", __func__, goldberg_client, pName);

    void *(*create_interface)(const char*) = (void *)GetProcAddress(steam_api, "SteamInternal_CreateInterface");
    void *client = create_interface(pName);
	LOG("%s: created client version 0x%x, %s\n", __func__, client, pName);

	#if CALLBACK_THREAD_OVERRIDE
	pthread_t callback_thread;
	pthread_create(&callback_thread, NULL, callback_thread_func, NULL);
	#endif

    return client;
}

void asi_loader(){
	DIR *cwd = opendir("./");
	while(true){
		struct dirent *entry = readdir(cwd);
		if (entry == NULL){
			break;
		}
		int name_len = strlen(entry->d_name);
		if (name_len < 4){
			continue;
		}
		if (strcmp(&entry->d_name[name_len - 4], ".csi") == 0){
			HMODULE asi_mod = LoadLibraryA(entry->d_name);
			LOG("%s: found %s, load status 0x%x\n", __func__, entry->d_name, asi_mod);
		}
	}
	closedir(cwd);
}

__attribute__((constructor))
int init(){
	log_init();
	LOG("%s: module loaded\n", __func__);
	redirect_functions();
	asi_loader();
}
