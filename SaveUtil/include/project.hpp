#pragma once

#include "menus.hpp"
#include "papyrus.hpp"

inline void InitializeInterface () {
	Menus::CustomMenu::Install();
}

namespace Project {
	constexpr std::string_view Name = "SaveUtil";
	constexpr REL::Version Version = { 1, 0, 0 };
	constexpr bool HasPapyrusFunctions = true;

	inline void MessageHandler (F4SE::MessagingInterface::Message* a_message) {
		switch (a_message->type) {
			case F4SE::MessagingInterface::kInputLoaded: {
				spdlog::debug("kInputLoaded");
				break;
			}
			case F4SE::MessagingInterface::kGameLoaded: {
				spdlog::debug("kGameLoaded");
				break;
			}
			case F4SE::MessagingInterface::kPostLoad: {
				spdlog::debug("kPostLoad");
				break;
			}
			case F4SE::MessagingInterface::kGameDataReady: {
				spdlog::debug("kGameDataReady");
				InitializeInterface();
				break;
			}

			case F4SE::MessagingInterface::kPostSaveGame: {
				spdlog::debug("kPostSaveGame");
				break;
			}
			case F4SE::MessagingInterface::kPostLoadGame: {
				spdlog::debug("kPostLoadGame");
				break;
			}
			default: {
				break;
			}
		}
	}

	inline bool RegisterPapyrusFunctions (RE::BSScript::IVirtualMachine* a_VM) {
		// a_VM->BindNativeMethod(SCRIPT_NAME, "IsSavingAllowed", IsSavingAllowed, true);
		// a_VM->BindNativeMethod(SCRIPT_NAME, "ListSaves", ListSaves, true);

		a_VM->BindNativeMethod(Name, "Test", Papyrus::Test, true);

		a_VM->BindNativeMethod(Name, "GetPlayerId", Papyrus::GetPlayerId, true);

		a_VM->BindNativeMethod(Name, "QueueSave", Papyrus::QueueSave, true);

		a_VM->BindNativeMethod(Name, "LoadGame", Papyrus::LoadGame, true);
		a_VM->BindNativeMethod(Name, "SaveGame", Papyrus::SaveGame, std::nullopt, true);

		a_VM->BindNativeMethod(Name, "ShowMessage", Papyrus::ShowMessage, true);
		a_VM->BindNativeMethod(Name, "Fmt", Papyrus::Fmt, true);

		a_VM->BindNativeMethod(Name, "WriteIni", Papyrus::WriteIni, true);
		a_VM->BindNativeMethod(Name, "ReadIni", Papyrus::ReadIni, true);

		a_VM->BindNativeMethod(Name, "WriteIniBool", Papyrus::WriteIniBool, true);
		a_VM->BindNativeMethod(Name, "ReadIniBool", Papyrus::ReadIniBool, true);

		a_VM->BindNativeMethod(Name, "PadStart", Papyrus::PadStart, true);
		a_VM->BindNativeMethod(Name, "PadEnd", Papyrus::PadEnd, true);

		a_VM->BindNativeMethod(Name, "LoadLastSave", Papyrus::LoadLastSave, true);

		a_VM->BindNativeMethod(Name, "OpenCustomMenu", Papyrus::OpenCustomMenu, true);

		a_VM->BindNativeMethod(Name, "LogInfo", Papyrus::LogInfo, true);
		a_VM->BindNativeMethod(Name, "LogWarn", Papyrus::LogWarn, true);
		a_VM->BindNativeMethod(Name, "LogError", Papyrus::LogError, true);
		a_VM->BindNativeMethod(Name, "LogDebug", Papyrus::LogDebug, true);

		return true;
	}
}
