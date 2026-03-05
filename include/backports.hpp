#pragma once

#if GAME_VERSION == 1
namespace RE {
	// shadowboost f4
	struct TESLoadGameEvent {
	public:
		[[nodiscard]] static BSTEventSource<TESLoadGameEvent>* GetEventSource () {
			using func_t = decltype(&TESLoadGameEvent::GetEventSource);
			static REL::Relocation<func_t> func { REL::ID(823570) };
			return func();
		}
	};

	static_assert(sizeof(TESLoadGameEvent) == 0x1);

	class BGSSaveLoadFileEntry {
	public:
		const char* fileName;           // 00
		const char* playerName;         // 08
		const char* playerTitle;        // 10
		const char* location;           // 18
		const char* playTime;           // 20
		const char* raceName;           // 28
		std::int32_t version;           // 30
		std::uint32_t saveGameNumber;   // 34
		std::uint32_t playerLevel;      // 38
		float levelProgress;            // 3C
		float levelThreshold;           // 40
		std::uint32_t screenshotWidth;  // 44
		std::uint32_t screenshotHeight; // 48
		std::uint32_t screenshotOffset; // 4C
		std::uint64_t fileTime;         // 50
		std::uint64_t saveTime;         // 58
		std::int32_t deviceID;          // 60
		bool loaded;                    // 64
		bool corrupt;                   // 65
		bool needsSync;                 // 66
	};

	static_assert(sizeof(BGSSaveLoadFileEntry) == 0x68);
}
#endif

class BGSSaveLoadManagerEx : public RE::BGSSaveLoadManager {
public:
	using BufferSceneScreenShot_t = void (*)(RE::BGSSaveLoadManager*);
	using SaveGame_t = bool (*)(RE::BGSSaveLoadManager*, const char*, std::int32_t, std::uint32_t, bool);
	using LoadGame_t = void (*)(RE::BGSSaveLoadManager*, const char*, std::int32_t, std::uint32_t, bool);

	[[nodiscard]] static BGSSaveLoadManagerEx* GetSingleton () noexcept {
		return reinterpret_cast<BGSSaveLoadManagerEx*>(RE::BGSSaveLoadManager::GetSingleton());
	}

	// From Intersave Container InterReloc https://www.nexusmods.com/fallout4/mods/84100
	void BufferSceneScreenShot () {
#if GAME_VERSION == 1
		static REL::Relocation<BufferSceneScreenShot_t> func { REL::Offset(0x00CF1130) };
#elif GAME_VERSION == 3
		static REL::Relocation<BufferSceneScreenShot_t> func { REL::Offset(0x00BFE3C0) };
#else
		static_assert(false, "Unsupported GAME_VERSION");
#endif
		func(this);
	}

	// From Intersave Container InterReloc https://www.nexusmods.com/fallout4/mods/84100
	bool SaveGame (const char* fileName, std::int32_t deviceID, std::uint32_t outputStats, bool checkForMods) {
#if GAME_VERSION == 1
		static REL::Relocation<SaveGame_t> func { REL::Offset(0x00CED1D0) };
#elif GAME_VERSION == 3
		static REL::Relocation<SaveGame_t> func { REL::Offset(0x00BF8E10) };
#else
		static_assert(false, "Unsupported GAME_VERSION");
#endif

		return func(this, fileName, deviceID, outputStats, checkForMods);
	}

	// From Intersave Container InterReloc https://www.nexusmods.com/fallout4/mods/84100
	void LoadGame (const char* fileName, std::int32_t deviceID, std::uint32_t outputStats, bool checkForMods) {
#if GAME_VERSION == 1
		static REL::Relocation<LoadGame_t> func { REL::Offset(0x00CED6D0) };
#elif GAME_VERSION == 2
		static REL::Relocation<LoadGame_t> func { REL::Offset(0x00B77FD0) };
#elif GAME_VERSION == 3
		static REL::Relocation<LoadGame_t> func { REL::Offset(0x00BF93B0) };
#else
		static_assert(false, "Unsupported GAME_VERSION");
#endif

		func(this, fileName, deviceID, outputStats, checkForMods);
	}
};
