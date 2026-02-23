#pragma once

#include "util.hpp"
#include "save_backport.hpp"

namespace Papyrus {
	using BSLM = RE::BGSSaveLoadManager;

	namespace Internal {
		inline void SetPlayerId (std::uint32_t low) {
			auto* bslm = BSLM::GetSingleton();
			auto id = static_cast<std::uint64_t>(low);
			bslm->currentPlayerID = id;
		}

		inline std::uint32_t GetPlayerId () {
			auto* bslm = BSLM::GetSingleton();

			auto id = bslm->currentPlayerID;
			auto low = static_cast<std::uint32_t>(id & 0xFFFFFFFFULL);

			return low;
		}

		inline std::string GetCallerName (RE::BSScript::IVirtualMachine& a_vm, std::uint32_t a_stackID) {
			RE::BSTSmartPointer<RE::BSScript::Stack> stack;
			static std::string fallback = "Unknown";

			auto* vmInternal = reinterpret_cast<RE::BSScript::Internal::VirtualMachine*>(&a_vm);
			if (!vmInternal) {
				return fallback;
			}

			if (!vmInternal->GetStackByID(a_stackID, stack) || !stack || !stack->top) {
				return fallback;
			}

			auto* frame = stack->top;
			if (!frame) {
				return fallback;
			}

			auto* caller = frame->previousFrame;
			if (caller && caller->owningObjectType) {
				const char* name = caller->owningObjectType->GetName();
				return name ? std::string { name } : fallback;
			}

			return fallback;
		}
	}

	inline SaveBackport::BGSSaveLoadFileEntry g_load_entry {};
	inline std::string g_load_name;

	inline void Test (std::monostate mono) {
		spdlog::debug("TEST THIS SHIT");
	}

	inline void ShowMessage (
		std::monostate mono,
		std::string_view fmtMessage,
		std::string_view as1,
		std::string_view as2,
		std::string_view as3,
		std::string_view as4,
		std::string_view as5,
		std::string_view as6,
		std::string_view as7,
		std::string_view as8,
		std::string_view as9
	) {
		spdlog::debug("ShowMessage called with message={}, as1={}", fmtMessage, as1);
		// no sound, no throttle, not a warning
		std::string fmtString = fmt::format(fmt::runtime(fmtMessage), as1, as2, as3, as4, as5, as6, as7, as8, as9);

		spdlog::debug("ShowMessage formatted = {}", fmtString);

		RE::SendHUDMessage::ShowHUDMessage(fmtString.c_str(), nullptr, false, false);
	}

	std::string Fmt (
		std::monostate mono,
		std::string_view fmtStr,
		std::string_view as1,
		std::string_view as2,
		std::string_view as3,
		std::string_view as4,
		std::string_view as5,
		std::string_view as6,
		std::string_view as7,
		std::string_view as8,
		std::string_view as9
	) {
		spdlog::debug("Fmt called with message={}, as1={}", fmtStr, as1);

		return fmt::format(fmt::runtime(fmtStr), as1, as2, as3, as4, as5, as6, as7, as8, as9);
	}

	inline void LogInfo (
		RE::BSScript::IVirtualMachine& a_vm,
		std::uint32_t a_stackID,
		std::monostate mono,
		std::string_view fmtStr,
		std::string_view as1,
		std::string_view as2,
		std::string_view as3,
		std::string_view as4,
		std::string_view as5,
		std::string_view as6,
		std::string_view as7,
		std::string_view as8,
		std::string_view as9
	) {
		auto scriptName = Internal::GetCallerName(a_vm, a_stackID);
		auto logger = Util::GetLogger(scriptName);

		if (logger && logger->level() > spdlog::level::info) {
			logger->info(fmt::runtime(fmtStr), as1, as2, as3, as4, as5, as6, as7, as8, as9);
		}
	}

	inline void LogWarn (
		RE::BSScript::IVirtualMachine& a_vm,
		std::uint32_t a_stackID,
		std::monostate mono,
		std::string_view fmtStr,
		std::string_view as1,
		std::string_view as2,
		std::string_view as3,
		std::string_view as4,
		std::string_view as5,
		std::string_view as6,
		std::string_view as7,
		std::string_view as8,
		std::string_view as9
	) {
		auto scriptName = Internal::GetCallerName(a_vm, a_stackID);
		auto logger = Util::GetLogger(scriptName);
		if (logger && logger->level() > spdlog::level::warn) {
			logger->warn(fmt::runtime(fmtStr), as1, as2, as3, as4, as5, as6, as7, as8, as9);
		}
	}

	inline void LogDebug (
		RE::BSScript::IVirtualMachine& a_vm,
		std::uint32_t a_stackID,
		std::monostate mono,
		std::string_view fmtStr,
		std::string_view as1,
		std::string_view as2,
		std::string_view as3,
		std::string_view as4,
		std::string_view as5,
		std::string_view as6,
		std::string_view as7,
		std::string_view as8,
		std::string_view as9
	) {
		auto scriptName = Internal::GetCallerName(a_vm, a_stackID);
		auto logger = Util::GetLogger(scriptName);
		if (logger && logger->level() > spdlog::level::debug) {
			logger->debug(fmt::runtime(fmtStr), as1, as2, as3, as4, as5, as6, as7, as8, as9);
		}
	}

	inline void LogError (
		RE::BSScript::IVirtualMachine& a_vm,
		std::uint32_t a_stackID,
		std::monostate mono,
		std::string_view fmtStr,
		std::string_view as1,
		std::string_view as2,
		std::string_view as3,
		std::string_view as4,
		std::string_view as5,
		std::string_view as6,
		std::string_view as7,
		std::string_view as8,
		std::string_view as9
	) {
		auto scriptName = Internal::GetCallerName(a_vm, a_stackID);
		auto logger = Util::GetLogger(scriptName);
		if (logger && logger->level() > spdlog::level::err) {
			logger->error(fmt::runtime(fmtStr), as1, as2, as3, as4, as5, as6, as7, as8, as9);
		}
	}

	inline std::string GetPlayerId (std::monostate mono) {
		auto playerId = Internal::GetPlayerId();
		if (playerId == 0) {
			playerId = Util::RandomUInt32();
			Internal::SetPlayerId(playerId);
		}

		return fmt::format("{:08X}", playerId);
	}

	inline std::string SetPlayerId (std::monostate mono, std::string_view playerIdHex) {
		std::uint32_t playerId = 0;
		if (!playerIdHex.empty()) {
			playerId = Util::Uint32FromHex(playerIdHex);
		}

		if (playerId == 0) playerId = Util::RandomUInt32();

		Internal::SetPlayerId(playerId);
		spdlog::debug("SetPlayerId: playerId={:08X}", playerId);

		return fmt::format("{:08X}", playerId);
	}

	// bool IsSavingAllowed (std::monostate mono) {
	// 	auto bslm = BSLM::GetSingleton();
	// 	logger->debug("IsSaveAllowed called : {}", bslm->savingAllowed);
	// 	return bslm->savingAllowed;
	// }

	inline void QueueSave (std::monostate mono) {
		spdlog::debug("QueueSave called");

		auto* bslm = BSLM::GetSingleton();

		SaveBackport::GetBufferSceneScreenShot()(reinterpret_cast<void*>(bslm));
		bslm->QueueSaveLoadTask(BSLM::QUEUED_TASK::kForceSave);
	}

	inline void LoadGame (std::monostate mono, std::string_view file_name) {
		spdlog::debug("LoadGame called : {}", file_name);

		auto* bslm = BSLM::GetSingleton();

		g_load_name = std::string { file_name };
		g_load_entry.fileName = g_load_name.c_str();

		bslm->queuedEntryToLoad = reinterpret_cast<RE::BGSSaveLoadFileEntry*>(&g_load_entry);
		bslm->QueueSaveLoadTask(BSLM::QUEUED_TASK::kLoadGame);
	}

	inline void LoadLastSave (std::monostate mono) {
		spdlog::debug("LoadLastSave called");

		auto* bslm = BSLM::GetSingleton();
		bslm->QueueSaveLoadTask(BSLM::QUEUED_TASK::kLoadMostRecentSave);
	}

	inline void OpenCustomMenu (std::monostate mono) {
		spdlog::debug("OpenCustomMenu called");

		if (auto* UIMessageQueue = RE::UIMessageQueue::GetSingleton()) {
			UIMessageQueue->AddMessage("CustomMenu", RE::UI_MESSAGE_TYPE::kShow);
		}
	}

	inline std::string PadStart (std::monostate mono, std::string_view input, std::uint16_t maxLen, std::string_view padChar) {
		return Util::PadStart(input, maxLen, padChar);
	}

	inline std::string PadEnd (std::monostate mono, std::string_view input, std::uint16_t maxLen, std::string_view padChar) {
		return Util::PadEnd(input, maxLen, padChar);
	}

	inline bool SaveGame (
		RE::BSScript::IVirtualMachine& a_vm,
		std::uint32_t a_stackID,
		std::monostate mono,
		std::string_view file_name
	) {
		spdlog::debug("SaveGame (latent) called : {}", file_name);

		const auto stackID = a_stackID;
		auto* vmPtr = &a_vm;

		std::string localName = Util::SanitizeFileName(file_name, true);

		F4SE::GetTaskInterface()->AddTask([stackID, vmPtr, localName] () mutable {
			auto* bslm = BSLM::GetSingleton();
			SaveBackport::GetBufferSceneScreenShot()(reinterpret_cast<void*>(bslm));

			auto playerId = Internal::GetPlayerId();
			if (playerId == 0) {
				playerId = Util::RandomUInt32();
				Internal::SetPlayerId(playerId);
			}

			std::string finalName = fmt::format("{} [_{:08X}]", localName, playerId);

			const bool res = SaveBackport::GetSaveGame()(reinterpret_cast<void*>(bslm), finalName.c_str(), 0, 0, false);
			spdlog::debug("SaveGame task finished: result={}", res);

			RE::BSScript::Variable ret;
			RE::BSScript::PackVariable(ret, res);
			if (vmPtr) {
				vmPtr->ReturnFromLatent(stackID, ret);
			}
		});

		return true;
	}

	inline void ListSaves (std::monostate mono) {
		auto* bslm = BSLM::GetSingleton();
		bslm->QueueSaveLoadTask(BSLM::QUEUED_TASK::kBuildSaveGameList);

		if (!bslm->isSaveListBuilt) {
			spdlog::debug("Save list not built yet (queued rebuild).");
			return;
		}

		const std::uint32_t count = bslm->saveGameCount;
		spdlog::debug("Save list has {} entries (build id {}).", count, bslm->saveGameListBuildID);

		auto GetEntryFileName = [] (RE::BGSSaveLoadFileEntry* a_entry) -> const char* {
			if (!a_entry) {
				return nullptr;
			}
			const auto* bslfe = reinterpret_cast<const SaveBackport::BGSSaveLoadFileEntry*>(a_entry);
			return bslfe->fileName;
		};

		for (std::uint32_t i = 0; i < count; ++i) {
			auto* entry = bslm->saveGameList[i];
			if (!entry) {
				spdlog::debug("[{}] <null entry>", i);
				continue;
			}

			const char* name = GetEntryFileName(entry);
			spdlog::debug("[{}] entry ptr={} name={}", i, fmt::ptr(entry), name ? name : "<no-name>");
		}
	}

	inline std::string ReadIni (
		RE::BSScript::IVirtualMachine& a_vm,
		std::uint32_t a_stackID,
		std::monostate mono,
		std::string_view section,
		std::string_view key
	) {
		auto name = Internal::GetCallerName(a_vm, a_stackID);
		spdlog::debug("ReadIni ({}): section={}, key={}", name, section, key);

		auto ini = Util::LoadIni(name);
		return Util::ReadIni(ini, section, key);
	}

	inline bool ReadIniBool (
		RE::BSScript::IVirtualMachine& a_vm,
		std::uint32_t a_stackID,
		std::monostate mono,
		std::string_view section,
		std::string_view key
	) {
		auto name = Internal::GetCallerName(a_vm, a_stackID);
		spdlog::debug("ReadIniBool ({}): section={}, key={}", name, section, key);

		auto ini = Util::LoadIni(name);
		return Util::ReadIniBool(ini, section, key, false);
	}

	inline void WriteIniBool (
		RE::BSScript::IVirtualMachine& a_vm,
		std::uint32_t a_stackID,
		std::monostate mono,
		std::string_view section,
		std::string_view key,
		bool value
	) {
		auto name = Internal::GetCallerName(a_vm, a_stackID);
		spdlog::debug("WriteIniBool ({}): section={}, key={}, value={}", name, section, key, value);
		Util::WriteIniBool(name, section, key, value);
	}

	inline void WriteIni (
		RE::BSScript::IVirtualMachine& a_vm,
		std::uint32_t a_stackID,
		std::monostate mono,
		std::string_view section,
		std::string_view key,
		std::string_view value
	) {
		auto name = Internal::GetCallerName(a_vm, a_stackID);
		spdlog::debug("WriteIni ({}): section={}, key={}, value={}", name, section, key, value);
		Util::WriteIni(name, section, key, value);
	}
}
