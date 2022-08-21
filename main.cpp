#include "memory.h"

#include <thread>
#include <iostream>


namespace offsets
{
	constexpr auto localPlayer = 0xDC04CC;
	constexpr auto m_viewPunchAngle = 0x3030;
	constexpr auto dwClientState_State = 0x108;
	constexpr auto dwClientState_ViewAngles = 0x4D90;


	constexpr auto flags = 0x104;
	constexpr auto forceJump = 0x52868EC;
	constexpr auto entityList = 0x4DDC90C;
	constexpr auto glowObjectManager = 0x53255D8;
	constexpr auto teamNum = 0xF4;
	constexpr auto glowIndex = 0x10488;
	constexpr auto m_iFOV = 0x31F4;
}

__declspec(align(16)) struct Color
{
	constexpr Color(const float r, const float g, const float b, const float a = 1.f) noexcept :
		r(r), g(g), b(b), a(a) { }

	float r, g, b, a;
};



int main(){
	const auto mem = Memory("csgo.exe");
	const auto client = mem.GetModuleAddress("client.dll");

	constexpr const auto color = Color{ 1.f, 0.f, 1.f };

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayer);
		if (!localPlayer)
			continue;

		const auto localPlayerTeam = mem.Read<std::uintptr_t>(localPlayer + offsets::teamNum);
		const auto localPlayerFlags = mem.Read<std::uintptr_t>(localPlayer + offsets::flags);

		if (GetAsyncKeyState(VK_SPACE))
			(localPlayerFlags & (1 << 0)) ?
			mem.Write<std::uintptr_t>(client + offsets::forceJump, 6) :
			mem.Write<std::uintptr_t>(client + offsets::forceJump, 4);
			mem.Write<std::uintptr_t>(client + offsets::m_iFOV, 90);


		const auto glowObjectManager = mem.Read<std::uintptr_t>(client + offsets::glowObjectManager);




		for (auto i = 1; i <= 32; ++i)
		{
			const auto entity = mem.Read<std::uintptr_t>(client + offsets::entityList + i * 0x10);
			if (!entity)
				continue;

			if (mem.Read<std::uintptr_t>(entity + offsets::teamNum) == localPlayerTeam)
				continue;

			const auto glowIndex = mem.Read<std::int32_t>(entity + offsets::glowIndex);
			mem.Write<Color>(glowObjectManager + (glowIndex * 0x38) + 0x8, color);
			mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true);
			mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x29, false);
		}
	}
}
