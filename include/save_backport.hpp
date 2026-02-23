#pragma once

// Mostly from InterReloc https://www.nexusmods.com/fallout4/mods/84100

namespace SaveBackport {

	struct BGSSaveLoadFileEntry {
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
	using SaveCompleteCallback_t = void (*)(int /*result*/, void** /*files*/);
	using LoadCompleteCallback_t = void (*)(int /*result*/, void** /*files*/);
	using TaskFinishedCallback_t = void (*)(bool /*success*/);

	struct AsyncRequest_POD {
		std::uint32_t taskTypeID;                    // 00
		void* savefile;                              // 08  (BGSSaveLoadFile*)
		TaskFinishedCallback_t taskFinishedCallback; // 10
		const char* previousSaveToDelete;            // 18
	};

	struct BGSSaveLoadThread_POD {
		bool running;
		bool busy;
		void* haveTask;
	};

	struct BSSaveDataSystemUtilityImage_POD {
		std::uint32_t size;   // 00
		std::uint32_t width;  // 04
		std::uint32_t height; // 08
		char* buffer;         // 10
	};

	enum class QUEUED_TASK : std::int32_t {
		kAutoSave = 0x1,
		kForceSave = 0x2,
		kLoadMostRecentSave = 0x4,
		kQuickSave = 0x8,
		kQuickLoad = 0x10,
		kQuickNewSave = 0x20,
		kLoadGame = 0x40,
		kSysUtilLoadGame = 0x80,
		kMissingContentLoad = 0x100,
		kAutoSaveCommit = 0x200,
		kQuickSaveCommit = 0x400,
		kCaptureFaceGen = 0x800,
		kBuildSaveGameList = 0x1000,
		kRefreshScreenshot = 0x2000,
		kSaveAndQuit = 0x4000,
		kConfirmModsLoad = 0x8000,
		kSaveAndQuitToDesktop = 0x10000,
	};

	struct BGSSaveLoadManager_POD {
		void** saveGameList;                                       // 000  (BSTArray<BGSSaveLoadFileEntry*>*)
		bool isSaveListBuilt;                                      // 008
		std::uint32_t saveGameCount;                               // 00C
		std::uint32_t currentSaveGameNumber;                       // 010
		std::uint64_t saveGameListBuildID;                         // 018
		std::int32_t currentAutoSaveNumber;                        // 020
		bool simulatedMode;                                        // 024
		std::uint32_t queuedTasks;                                 // 028
		std::uint32_t saveOperationDelayCounter;                   // 02C
		void* queuedEntryToLoad;                                   // 030  (BGSSaveLoadFileEntry*)
		std::uint64_t lastSaveTime;                                // 038
		bool savingAllowed;                                        // 040
		char* mostRecentSaveGame;                                  // 048
		std::int32_t mostRecentSaveGameDeviceID;                   // 050
		void* playerIDNameMap;                                     // 058  (BSTHashMap<...>*)
		void* editorIdLocationMap;                                 // 060
		std::uint64_t currentPlayerID;                             // 068
		std::uint64_t displayPlayerID;                             // 070
		void* savefileNameToDelete;                                // 078
		void* autosaveFileNames;                                   // 080
		void* quicksaveFileName;                                   // 088
		void* exitsaveFileName;                                    // 090
		bool hasSaveData;                                          // 098
		void* saveLoadFile;                                        // 0A0
		bool returnedFromSysUtil;                                  // 0A8
		bool sysUtilLoadDataComplete;                              // 0A9
		SaveCompleteCallback_t sysUtilSaveCompleteCallback;        // 0B0
		LoadCompleteCallback_t sysUtilLoadCompleteCallback;        // 0B8
		BSSaveDataSystemUtilityImage_POD saveDataCreateImage;      // 0C0
		BSSaveDataSystemUtilityImage_POD saveDataBackgroundImage;  // 0D8
		BSSaveDataSystemUtilityImage_POD saveDataIconEncodedImage; // 0F0
		void* screenshot;                                          // 108  (NiPointer<NiTexture>[1])
		BGSSaveLoadThread_POD saveLoadTasksThread;                 // 110
	};

	// -----------------------
	// FROM Intersave Container InterReloc https://www.nexusmods.com/fallout4/mods/84100
	// -----------------------

	using BufferSceneScreenShot_t = void (*)(void* /*BGSSaveLoadManager**/);

	inline REL::Relocation<BufferSceneScreenShot_t> BufferSceneScreenShot_v1 { REL::Offset(0x00CF1130) };
	inline REL::Relocation<BufferSceneScreenShot_t> BufferSceneScreenShot_v2 { REL::Offset(0x00B7C3D0) };
	inline REL::Relocation<BufferSceneScreenShot_t> BufferSceneScreenShot_v3 { REL::Offset(0x00BFE3C0) };

	inline REL::Relocation<BufferSceneScreenShot_t>& GetBufferSceneScreenShot () noexcept {
#if GAME_VERSION == 1
		return BufferSceneScreenShot_v1;
#elif GAME_VERSION == 3
		return BufferSceneScreenShot_v3;
#else
		static_assert(false, "Unsupported GAME_VERSION");
#endif
	}

	// -----------------------
	// FROM Intersave Container InterReloc https://www.nexusmods.com/fallout4/mods/84100
	// -----------------------

	using SaveGame_t = bool (*)(void*, const char*, std::int32_t, std::uint32_t, bool);

	inline REL::Relocation<SaveGame_t> SaveGame_v1 { REL::Offset(0x00CED1D0) };
	inline REL::Relocation<SaveGame_t> SaveGame_v2 { REL::Offset(0x00B77A30) };
	inline REL::Relocation<SaveGame_t> SaveGame_v3 { REL::Offset(0x00BF8E10) };

	inline REL::Relocation<SaveGame_t>& GetSaveGame () noexcept {
#if GAME_VERSION == 1
		return SaveGame_v1;
#elif GAME_VERSION == 3
		return SaveGame_v3;
#else
		static_assert(false, "Unsupported GAME_VERSION");
#endif
	}

}
