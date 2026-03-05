#pragma once

#include "util.hpp"
#include "backports.hpp"

namespace Internal {
	using BSLM = RE::BGSSaveLoadManager;

	inline std::unordered_map<std::string, std::string> g_caller_name_map {};

	inline void ShowHUDNotification (std::string_view message) {
		if (message.empty()) return;

		std::string localMessage = std::string { message };
		RE::SendHUDMessage::ShowHUDMessage(localMessage.c_str(), nullptr, false, false);
	}

	inline std::string RegisterCallerName (std::string_view scriptName, std::string_view mapName) {
		auto sanitized = Util::SanitizeFileName(mapName);
		g_caller_name_map[std::string(scriptName)] = sanitized;
		return sanitized;
	}

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

	inline std::uint32_t EnsurePlayerId () {
		auto playerId = Internal::GetPlayerId();
		if (playerId == 0) {
			playerId = Util::RandomUInt32();
			Internal::SetPlayerId(playerId);
			spdlog::warn("Player Id was 0. New Player Id: {}", playerId);
		}

		return playerId;
	}

	inline std::string GetCallerNameRaw (RE::BSScript::IVirtualMachine& vm, std::uint32_t stackId) {
		RE::BSTSmartPointer<RE::BSScript::Stack> stack;
		static std::string fallback = "Unknown";

		auto* vmInternal = reinterpret_cast<RE::BSScript::Internal::VirtualMachine*>(&vm);
		if (!vmInternal) {
			return fallback;
		}

		if (!vmInternal->GetStackByID(stackId, stack) || !stack || !stack->top) {
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

	inline std::string GetCallerName (RE::BSScript::IVirtualMachine& vm, std::uint32_t stackId) {
		auto scriptName = GetCallerNameRaw(vm, stackId);
		auto it = g_caller_name_map.find(scriptName);
		if (it == g_caller_name_map.end()) {
			it = g_caller_name_map.emplace(scriptName, Util::SanitizeFileName(scriptName)).first;
		}
		return it->second;
	}

	inline std::string ToString (const RE::BSScript::Variable& var) {
		auto rawType = var.GetType().GetRawType();

		switch (rawType) {
			case RE::BSScript::TypeInfo::RawType::kString:
				return RE::BSScript::get<RE::BSFixedString>(var).c_str();
			case RE::BSScript::TypeInfo::RawType::kFloat:
				return std::to_string(RE::BSScript::get<float>(var));
			case RE::BSScript::TypeInfo::RawType::kInt:
				return std::to_string(RE::BSScript::get<std::int32_t>(var));
			case RE::BSScript::TypeInfo::RawType::kNone:
				return "<None>";
			case RE::BSScript::TypeInfo::RawType::kBool:
				return RE::BSScript::get<bool>(var) ? "true" : "false";
			case RE::BSScript::TypeInfo::RawType::kObject:
				return "<Object>";
			case RE::BSScript::TypeInfo::RawType::kStruct:
				return "<Struct>";
			case RE::BSScript::TypeInfo::RawType::kVar:
				return "<Var>";
			case RE::BSScript::TypeInfo::RawType::kArrayObject:
				return "<Array[object]>";
			case RE::BSScript::TypeInfo::RawType::kArrayString:
				return "<Array[string]>";
			case RE::BSScript::TypeInfo::RawType::kArrayInt:
				return "<Array[int]>";
			case RE::BSScript::TypeInfo::RawType::kArrayFloat:
				return "<Array[float]>";
			case RE::BSScript::TypeInfo::RawType::kArrayBool:
				return "<Array[bool]>";
			case RE::BSScript::TypeInfo::RawType::kArrayVar:
				return "<Array[var]>";
			case RE::BSScript::TypeInfo::RawType::kArrayStruct:
				return "<Array[struct]>";
			default:
				return "<Unknown>";
		}
	}

	template<class... VarPtrs>
	inline std::string Format (std::string_view fmtString, VarPtrs... vars) {
		fmt::dynamic_format_arg_store<fmt::format_context> store;
		const std::array<const RE::BSScript::Variable*, sizeof...(vars)> args { vars... };
		for (const auto* varPtr : args) {
			RE::BSScript::Variable none;
			const auto& varVal = varPtr ? *varPtr : none;

			switch (varVal.GetType().GetRawType()) {
				case RE::BSScript::TypeInfo::RawType::kInt:
					store.push_back(RE::BSScript::get<std::int32_t>(varVal));
					break;
				case RE::BSScript::TypeInfo::RawType::kString:
					store.push_back(std::string(RE::BSScript::get<RE::BSFixedString>(varVal).c_str()));
					break;
				case RE::BSScript::TypeInfo::RawType::kBool:
					store.push_back(RE::BSScript::get<bool>(varVal));
					break;
				case RE::BSScript::TypeInfo::RawType::kFloat:
					store.push_back(RE::BSScript::get<float>(varVal));
					break;
				default:
					store.push_back(ToString(varVal));
					break;
			}
		}

		return fmt::vformat(fmtString, fmt::basic_format_args<fmt::format_context>(store));
	}

	inline bool SaveGame (
		std::string_view saveName,
		std::string_view successMessage,
		std::string_view failureMessage,
		std::function<void(bool)> callback = nullptr
	) {
		auto playerId = Internal::EnsurePlayerId();

		auto sanitized = Util::SanitizeSaveName(saveName);
		auto finalName = fmt::format("{} [_{:08X}]", sanitized, playerId);

		F4SE::GetTaskInterface()->AddTask(
			[saveName = std::string(saveName),
			 successMessage = std::string(successMessage),
			 failureMessage = std::string(failureMessage),
			 finalName = std::move(finalName),
			 callback = std::move(callback)] () mutable {
				auto* bslm = BGSSaveLoadManagerEx::GetSingleton();
				bslm->BufferSceneScreenShot();

				const bool wasSaved = bslm->SaveGame(finalName.c_str(), 0, 0, false);

				if (wasSaved && !successMessage.empty()) {
					auto fmtSuccessMessage = fmt::format(fmt::runtime(successMessage), saveName);
					ShowHUDNotification(fmtSuccessMessage);
				}

				if (!wasSaved && !failureMessage.empty()) {
					auto fmtFailureMessage = fmt::format(fmt::runtime(failureMessage), saveName);
					ShowHUDNotification(fmtFailureMessage);
				}

				if (callback) callback(wasSaved);
			}
		);

		return true;
	}

	inline std::string GetCurrentLocationName () {
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		if (!player) {
			return {};
		}

		auto* cell = player->GetParentCell();

		if (cell && cell->IsInterior()) {
			auto cellStr = RE::TESFullName::GetFullName(*cell);
			if (!cellStr.empty()) {
				spdlog::debug("Cell name is {}", cellStr);
				return std::string(cellStr);
			}
		}

		auto* loc = player->GetCurrentLocation();

		if (loc) {
			auto locStr = RE::TESFullName::GetFullName(*loc);
			if (!locStr.empty()) {
				spdlog::debug("Location name is {}", locStr);
				return std::string(locStr);
			}
		}

		return {};
	}

	inline std::string GetPlayerName () {
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		if (!player) {
			spdlog::warn("GetPlayerName: PlayerCharacter::GetSingleton returned null");
			return {};
		}

		auto* npc = player->GetNPC();
		if (!npc) {
			spdlog::warn("GetPlayerName: GetNPC returned null");
			return {};
		}

		auto npcName = RE::TESFullName::GetFullName(*npc);
		if (npcName.empty()) {
			spdlog::warn("GetPlayerName: GetFullName returned empty string");
			return {};
		}

		return std::string(npcName);
	}
}
