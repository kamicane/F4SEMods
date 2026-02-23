#pragma once

#include "constants.hpp"
#include "console.hpp"

inline void InitializeConsoleInterface () {
	auto* ui = RE::UI::GetSingleton();
	if (!ui) {
		spdlog::error("Could not initialize interface");
		return;
	}

	auto isConsoleOpen = ui->GetMenuOpen(RE::Console::MENU_NAME);

	if (!isConsoleOpen) {
		spdlog::debug("Console was not open on kGameDataReady: Using OnMenuOpenCloseEvent");
		auto* sink = new Console::OnMenuOpen();
		ui->RegisterSink<RE::MenuOpenCloseEvent>(sink);
	} else {
		spdlog::debug("Console was open on kGameDataReady: Registering");
		Console::RegisterHistoryHandlers();
	}
}

namespace Project {
	inline void MessageHandler (F4SE::MessagingInterface::Message* a_message) {
		if (a_message->type == F4SE::MessagingInterface::kGameDataReady) {
			spdlog::debug("kGameDataReady");
			InitializeConsoleInterface();
		}
	}

	inline bool RegisterPapyrusFunctions (RE::BSScript::IVirtualMachine* a_VM) {
		return false;
	}
}
