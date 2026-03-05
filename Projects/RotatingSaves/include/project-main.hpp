#pragma once

#include "events.hpp"
#include "internal.hpp"
#include "project.h"
#include "papyrus.hpp"
#include "menu-text-input.hpp"
#include "util.hpp"

namespace Papyrus {
	inline void SaveNextSlot (std::monostate mono) {
		spdlog::debug("SaveNextSlot called");

		Util::LoadIni(Project::ID);

		auto savePrefixRaw = Util::GetIniString(Project::ID, "Strings", "sSavePrefix", "Rotating Save");
		auto savePrefix = Util::SanitizeSaveName(savePrefixRaw);

		auto useSaveNotifications = Util::GetIniBool(Project::ID, "Main", "bSaveNotifications");

		auto numSlots = Util::GetIniInt(Project::ID, "Main", "iNumSaves", 10, 1, 99);

		auto characterSection = fmt::format("Character:{:08X}", Internal::EnsurePlayerId());

		auto lastSavedSlot = Util::GetIniInt(Project::ID, characterSection, "iLastSavedSlot", 0, 0, numSlots);

		auto nextSlot = lastSavedSlot + 1;

		if (nextSlot > numSlots) nextSlot = 1;

		auto saveName = fmt::format("{} {:02}", savePrefix, nextSlot);

		std::string successMessage;
		std::string failureMessage;

		if (useSaveNotifications) {
			successMessage = Util::GetIniString(Project::ID, "Strings", "sSaveNotification");
			failureMessage = Util::GetIniString(Project::ID, "Strings", "sCantSaveNow");
		}

		Internal::SaveGame(
			saveName,
			successMessage,
			failureMessage,
			[characterSection = std::move(characterSection), nextSlot] (bool wasSaved) {
				if (wasSaved) {
					Util::SetIniInt(Project::ID, characterSection, "iLastSavedSlot", nextSlot);
					Util::SaveIni(Project::ID);
				}
			}
		);
	}

	inline void ShowSaveMenu (std::monostate mono) {
		spdlog::debug("[ShowSaveMenu] called");

		Util::LoadIni(Project::ID);
		auto useSaveNotifications = Util::GetIniBool(Project::ID, "Main", "bSaveNotifications");

		std::string successMessage;
		std::string failureMessage;

		if (useSaveNotifications) {
			successMessage = Util::GetIniString(Project::ID, "Strings", "sSaveNotification");
			failureMessage = Util::GetIniString(Project::ID, "Strings", "sCantSaveNow");
		}

		Events::TextInput.once("Close", [successMessage, failureMessage] (std::string_view text) {
			spdlog::debug("ShowSaveMenu -> kHide");

			if (!text.empty()) {
				Internal::SaveGame(text, successMessage, failureMessage);
			}
			return true;
		});

		auto locationName = Internal::GetCurrentLocationName();
		if (locationName.empty()) locationName = "New Save";

		Menu::TextInput::Open("Save Game", locationName);
	}
}

namespace Project {

	inline bool RegisterPapyrusFunctions (RE::BSScript::IVirtualMachine* vm) {
		vm->BindNativeMethod(Project::ID, "SaveGame", Papyrus::SaveGame, std::nullopt, true);

		vm->BindNativeMethod(Project::ID, "LoadGame", Papyrus::QueueLoadGame, true);
		vm->BindNativeMethod(Project::ID, "SaveAndQuitToDesktop", Papyrus::QueueSaveAndQuitToDesktop, std::nullopt, true);

		vm->BindNativeMethod(Project::ID, "ForceSave", Papyrus::QueueForceSave, std::nullopt, true);
		vm->BindNativeMethod(Project::ID, "AutoSave", Papyrus::QueueAutoSave, std::nullopt, true);

		vm->BindNativeMethod(Project::ID, "SaveNextSlot", Papyrus::SaveNextSlot, true);
		vm->BindNativeMethod(Project::ID, "ShowSaveMenu", Papyrus::ShowSaveMenu, true);
		vm->BindNativeMethod(Project::ID, "LoadLastSave", Papyrus::QueueLoadLastSave, true);

		return true;
	}

	inline void main () {
		F4SE::GetPapyrusInterface()->Register(RegisterPapyrusFunctions);

		Events::Messaging.once(F4SE::MessagingInterface::kGameDataReady, [] () {
			spdlog::debug("kGameDataReady");
			Menu::TextInput::Register();
			return true;
		});
	}

}
