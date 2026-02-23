#pragma once

#include "constants.hpp"
#include "util.hpp"
#include <filesystem>

namespace Console {

	inline std::shared_ptr<std::fstream> histFile;
	inline std::string lastCommand;

	// names are confusing because names in the action script are confusing
	// Actionscript has a method called AddHistory, which just prints stuff to the console (adds to buffer)
	// there *was no method to add a command to the actual history
	// I added AddCommandToHistory that does just that

	inline void AddCommandToHistory (RE::Scaleform::Ptr<RE::IMenu> console, const std::string& thisCommand) {
		if (thisCommand.empty()) return;

		RE::Scaleform::GFx::Value strVal;
		console->uiMovie->asMovieRoot->CreateString(std::addressof(strVal), thisCommand.c_str());
		console->menuObj.Invoke("AddCommandToHistory", nullptr, std::addressof(strVal), 1);

		lastCommand = thisCommand;
	}

	struct AddHistoryHandler : RE::Scaleform::GFx::FunctionHandler {
		AddHistoryHandler () = default;

		void Call (const RE::Scaleform::GFx::FunctionHandler::Params& a_params) override {
			if (a_params.argCount < 1 || !a_params.args) return;
			const auto& val = *(a_params.args);

			if (!val.IsString()) return;
			std::string thisCommand = val.GetString() ? val.GetString() : std::string();

			if (thisCommand.empty()) return;
			if (thisCommand == "qqq") return; // don't save qqq when typed from the console
			if (lastCommand == thisCommand)
				return; // don't save same commands when typed consecutively (or grabbed from history)

			auto* ui = RE::UI::GetSingleton();
			if (ui) {
				auto console = ui->GetMenu(RE::Console::MENU_NAME);
				if (console) {
					AddCommandToHistory(console, thisCommand);
				}
			}

			if (!histFile || !histFile->is_open()) {
				spdlog::error("AddHistoryHandler: file stream is not available");
				return;
			}

			(*histFile) << thisCommand << '\n';
			histFile->flush();
		}
	};

	inline bool RegisterHistoryHandlers () {
		// Validate UI and console
		auto* ui = RE::UI::GetSingleton();
		if (!ui) {
			spdlog::error("UI singleton is null");
			return false;
		}

		auto console = ui->GetMenu(RE::Console::MENU_NAME);
		if (!console) {
			spdlog::error("Console menu is null");
			return false;
		}

		auto uiMovie = console->uiMovie;

		if (!uiMovie) {
			spdlog::error("uiMovie is null");
			return false;
		}

		auto menuObj = console->menuObj;

		if (!menuObj.IsObject()) {
			spdlog::error("menuObj is not an object");
			return false;
		}

		auto root = uiMovie->asMovieRoot;

		if (!root) {
			spdlog::error("asMovieRoot is null");
			return false;
		}

		RE::Scaleform::GFx::Value consoleHistoryObj;

		root->CreateObject(&consoleHistoryObj);

		spdlog::debug("RegisterHistoryHandlers: Created ConsoleHistoryObj");

		auto ini = Util::LoadIni(Project::Name);

		std::string histFileDefaultName = "console_history.txt";

		std::filesystem::path histFilePath = Util::ReadIni(ini, "Main", "HistoryFile", histFileDefaultName);
		if (std::filesystem::is_directory(histFilePath)) {
			histFilePath /= histFileDefaultName;
		}

		histFilePath = Util::GetMyDocumentsPath() / histFilePath;

		spdlog::info("History file is {}", histFilePath.string());

		auto historySize = Util::ReadIniInt(ini, "Main", "HistorySize", 128, 32, 1024);

		RE::Scaleform::GFx::Value historySizeVal;
		historySizeVal = static_cast<double>(historySize);
		menuObj.Invoke("SetHistorySize", nullptr, std::addressof(historySizeVal), 1);

		std::deque<std::string> lastLines;

		histFile = std::make_shared<std::fstream>();

		histFile->open(histFilePath, std::ios::in | std::ios::out | std::ios::app);

		if (!histFile->is_open()) {
			spdlog::error("RegisterHistoryHandlers: failed to open {}", histFilePath.string());
			return false;
		}

		// Read last lines
		histFile->seekg(0);
		std::string line;
		while (std::getline(*histFile, line)) {
			lastLines.push_back(line);
			if (lastLines.size() > historySize) lastLines.pop_front();
		}

		// Truncate and rewrite
		histFile->close();
		histFile->open(histFilePath, std::ios::out | std::ios::trunc);
		for (const auto& line : lastLines) {
			*histFile << line << '\n';
			AddCommandToHistory(console, line);
		}

		// Keep file open for future append
		histFile->close();
		histFile->open(histFilePath, std::ios::out | std::ios::app);

		// Register as function to save command to our history

		RE::Scaleform::GFx::Value saveCommandFunc;
		root->CreateFunction(&saveCommandFunc, new AddHistoryHandler());
		consoleHistoryObj.SetMember("saveCommand", saveCommandFunc);
		spdlog::debug("RegisterHistoryHandlers: Registered saveCommand on ConsoleHistoryObj");

		if (!menuObj.SetMember("ConsoleHistoryObj", consoleHistoryObj)) {
			spdlog::error("Failed to set ConsoleHistoryObj as member of menuObj");
			return false;
		}

		spdlog::debug("RegisterHistoryHandlers: Successfully attached ConsoleHistoryObj to menuObj");

		auto welcomeMessage = Util::ReadIni(ini, "Main", "WelcomeMessage", "");

		if (!welcomeMessage.empty()) {
			auto msgStr = fmt::format(fmt::runtime(welcomeMessage), histFilePath.string());
			auto msgStrNl = msgStr + "\n";

			RE::Scaleform::GFx::Value msgVal;
			root->CreateString(std::addressof(msgVal), msgStrNl.c_str());
			menuObj.Invoke("AddHistory", nullptr, std::addressof(msgVal), 1);

			spdlog::info(msgStr);
		}

		return true;
	}

	class OnMenuOpen : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
	public:
		RE::BSEventNotifyControl ProcessEvent (
			const RE::MenuOpenCloseEvent& a_event,
			RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_source
		) override {
			auto* ui = RE::UI::GetSingleton();
			if (!ui) return RE::BSEventNotifyControl::kContinue;

			std::string menuName = a_event.menuName.c_str();

			spdlog::debug("MenuOpenCloseEvent: menu={}, opening={}", menuName, a_event.opening);

			if (!a_event.opening || menuName != RE::Console::MENU_NAME) {
				return RE::BSEventNotifyControl::kContinue;
			}

			spdlog::debug("Accepting MenuOpenCloseEvent: menu={}, opening={}", menuName, a_event.opening);

			a_source->UnregisterSink(this);

			RegisterHistoryHandlers();

			// Note to SoleVaultBoy: if you use kStop, others won't get the events until you UnregisterSink
			return RE::BSEventNotifyControl::kContinue;
		}
	};

}
