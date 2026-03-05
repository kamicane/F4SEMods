#pragma once

#include "events.hpp"
#include "menu-text-input.hpp"
#include "papyrus.hpp"
#include "load-handler.hpp"

namespace Project {

	inline void InitializeLoadEvent () {
		Events::SaveLoad.on(Events::SaveLoadType::Load, [] () {
			LoadHandler::RunLoadFunctions();
		});
	}

	inline bool RegisterPapyrusFunctions (RE::BSScript::IVirtualMachine* vm) {
		vm->BindNativeMethod(ID, "GetSettings", Papyrus::GetSettings, true);
		vm->BindNativeMethod(ID, "SetSettings", Papyrus::SetSettings, true);

		vm->BindNativeMethod(ID, "GetPlayerId", Papyrus::GetPlayerId, true);
		vm->BindNativeMethod(ID, "GetCallerName", Papyrus::GetCallerName, true);
		vm->BindNativeMethod(ID, "Register", Papyrus::Register, true);

		vm->BindNativeMethod(ID, "QueueSave", Papyrus::QueueForceSave, true);
		vm->BindNativeMethod(ID, "UpdatePowerArmor3d", Papyrus::UpdatePowerArmor3d, true);

		vm->BindNativeMethod(ID, "LoadGame", Papyrus::QueueLoadGame, true);
		vm->BindNativeMethod(ID, "SaveGame", Papyrus::SaveGame, std::nullopt, true);

		vm->BindNativeMethod(ID, "ShowMessage", Papyrus::ShowMessage, true);
		vm->BindNativeMethod(ID, "Format", Papyrus::Format, true);
		vm->BindNativeMethod(ID, "ConsoleLog", Papyrus::ConsoleLog, true);

		vm->BindNativeMethod(ID, "LoadSettings", Papyrus::LoadSettings, true);
		vm->BindNativeMethod(ID, "SaveSettings", Papyrus::SaveSettings, true);

		vm->BindNativeMethod(ID, "GetSettingBool", Papyrus::GetSettingBool, true);
		vm->BindNativeMethod(ID, "SetSettingBool", Papyrus::SetSettingBool, true);

		vm->BindNativeMethod(ID, "GetSettingString", Papyrus::GetSettingString, true);
		vm->BindNativeMethod(ID, "SetSettingString", Papyrus::SetSettingString, true);

		vm->BindNativeMethod(ID, "GetSettingInt", Papyrus::GetSettingInt, true);
		vm->BindNativeMethod(ID, "SetSettingInt", Papyrus::SetSettingInt, true);
		vm->BindNativeMethod(ID, "GetSettingFloat", Papyrus::GetSettingFloat, true);
		vm->BindNativeMethod(ID, "SetSettingFloat", Papyrus::SetSettingFloat, true);

		vm->BindNativeMethod(ID, "LoadLastSave", Papyrus::QueueLoadLastSave, true);

		vm->BindNativeMethod(ID, "LogD", Papyrus::LogD, true);
		vm->BindNativeMethod(ID, "LogI", Papyrus::LogI, true);
		vm->BindNativeMethod(ID, "LogW", Papyrus::LogW, true);
		vm->BindNativeMethod(ID, "LogE", Papyrus::LogE, true);

		// vm->BindNativeMethod(ID, "_OpenTextInputMenu", Papyrus::OpenTextInputMenu_, std::nullopt, true);
		// vm->BindNativeMethod(ID, "_GetLastTextInputResult", Papyrus::GetLastTextInputResult_, true);

		return true;
	}

	inline void main () {
		F4SE::GetPapyrusInterface()->Register(RegisterPapyrusFunctions);

		Events::Messaging.once(F4SE::MessagingInterface::kGameDataReady, [] () {
			LoadOrder::ComputePlugins();
			LoadHandler::LoadPluginIniFiles();

			Menu::TextInput::Register();
			InitializeLoadEvent();
			return true;
		});
	}
}
