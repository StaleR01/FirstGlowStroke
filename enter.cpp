#include <iostream>
#include <chrono>
#include <thread>
#include "memory.h"
#include "offsets.h"
using namespace offsets::signatures;
using namespace offsets::netvars;


int glowEnemiesRGB[3] = { 255, 0, 0 };

bool glowTeammates = false;
int glowTeammatesRGB[3] = { 0,0,255 };


memory mem;
std::uintptr_t client;
std::uintptr_t engine;

int main() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (mem.Attach("csgo.exe", PROCESS_ALL_ACCESS))
			break;
	}

	std::cout << "CS:GO attached!" << std::endl;
	client = mem.GetModule("client.dll");
	engine = mem.GetModule("engine.dll");


	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		DWORD localPlayer = mem.Read<DWORD>(client + dwLocalPlayer);
		DWORD localTeam = mem.Read<DWORD>(localPlayer + m_iTeamNum);
		DWORD glowManager = mem.Read<DWORD>(client + dwGlowObjectManager);

		for (int i = 1; i < 32; i++) {
			DWORD player = mem.Read<DWORD>(client + dwEntityList + i * 0x10);
			if (player == 0) continue;
			if (mem.Read<bool>(player + m_bDormant)) continue;
			if (mem.Read<DWORD>(player + m_iTeamNum) == localTeam) if (!glowTeammates) continue;
			else {
				DWORD glowIndex = mem.Read<DWORD>(player + m_iGlowIndex);
				mem.Write<float>(glowManager + glowIndex * 0x38 + 0x8, glowTeammatesRGB[0] / 255); mem.Write<float>(glowManager + glowIndex * 0x38 + 0xC, glowTeammatesRGB[1] / 255); mem.Write<float>(glowManager + glowIndex * 0x38 + 0x10, glowTeammatesRGB[2] / 255); mem.Write<float>(glowManager + glowIndex * 0x38 + 0x14, 1.f); mem.Write<bool>(glowManager + glowIndex * 0x38 + 0x28, true); mem.Write<bool>(glowManager + glowIndex * 0x38 + 0x29, false); continue;
			}
			DWORD glowIndex = mem.Read<DWORD>(player + m_iGlowIndex); mem.Write<float>(glowManager + glowIndex * 0x38 + 0x8, glowEnemiesRGB[0] / 255); mem.Write<float>(glowManager + glowIndex * 0x38 + 0xC, glowEnemiesRGB[1] / 255); mem.Write<float>(glowManager + glowIndex * 0x38 + 0x10, glowEnemiesRGB[2] / 255); mem.Write<float>(glowManager + glowIndex * 0x38 + 0x14, 1.f); mem.Write<bool>(glowManager + glowIndex * 0x38 + 0x28, true); mem.Write<bool>(glowManager + glowIndex * 0x38 + 0x29, false);
		}



	} return 0;
}