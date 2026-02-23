#pragma once

namespace Menus {
	class CustomMenu : public RE::GameMenuBase {
	public:
		static constexpr const char* MENU_NAME = "CustomMenu";

		static void Install () {
			if (auto* ui = RE::UI::GetSingleton()) {
				ui->RegisterMenu(MENU_NAME, Create, nullptr);
				spdlog::debug("Registered menu");
			}
		}

		CustomMenu () {
			menuFlags.set(
				RE::UI_MENU_FLAGS::kPausesGame,
				RE::UI_MENU_FLAGS::kUsesCursor,
				RE::UI_MENU_FLAGS::kDisablePauseMenu,
				RE::UI_MENU_FLAGS::kUpdateUsesCursor,
				RE::UI_MENU_FLAGS::kCustomRendering,
				RE::UI_MENU_FLAGS::kUsesMenuContext,
				RE::UI_MENU_FLAGS::kUsesBlurredBackground
			);
			menuHUDMode = "SpecialMode";
			depthPriority = RE::UI_DEPTH_PRIORITY::kTerminal;

			auto* const ScaleformManager = RE::BSScaleformManager::GetSingleton();
			spdlog::debug("BSScaleformManager = {}", fmt::ptr(ScaleformManager));
			if (!ScaleformManager) {
				spdlog::error("BSScaleformManager singleton is null - skipping GFx init");
				return;
			}

			const auto LoadMovieSuccess = ScaleformManager->LoadMovieEx(*this, "Interface/TextField2.swf", "root.Menu_mc");
			spdlog::debug("LoadMovieEx returned {}", LoadMovieSuccess);
			if (!LoadMovieSuccess) {
				spdlog::error("LoadMovieEx failed - skipping GFx initialization to avoid crash");
				return;
			}

			spdlog::debug("uiMovie = {}", fmt::ptr(uiMovie.get()));
			if (!uiMovie) {
				spdlog::error("uiMovie is null after LoadMovieEx - skipping GFx init");
				return;
			}

			filterHolder = std::make_unique<RE::BSGFxShaderFXTarget>(*uiMovie, "root.Menu_mc");
			if (filterHolder) {
				spdlog::debug("filterHolder created = {}", fmt::ptr(filterHolder.get()));
				filterHolder->CreateAndSetFiltersToHUD(RE::HUDColorTypes::kPlayerSetColor);
				shaderFXObjects.push_back(filterHolder.get());
			} else {
				spdlog::error("filterHolder is null after construction");
				return;
			}

			Background_mc = std::make_unique<RE::BSGFxShaderFXTarget>(*uiMovie, "root.Menu_mc.Background_mc");
			if (Background_mc) {
				Background_mc->EnableShadedBackground(RE::HUDColorTypes::kMenuNoColorBackground);
				shaderFXObjects.push_back(Background_mc.get());
			} else {
				spdlog::warn("Background_mc not found - continuing without background shader");
			}

			SetUpButtonBar(*filterHolder, "ButtonHintBar_mc", RE::HUDColorTypes::kPlayerSetColor);
			spdlog::debug("GFx initialization complete");
		}

		~CustomMenu () = default;

		void Call (const Params& a_params) override {
			switch (*((std::uint32_t*)&(a_params.userData))) {
				case 0: {
					auto *ControlMap = RE::ControlMap::GetSingleton();
					ControlMap->SetTextEntryMode(false);
					CloseMenu();
					break;
				}

				case 1: {
					if (a_params.argCount == 1 && a_params.args[0].IsString()) {
						RE::UIUtils::PlayMenuSound(a_params.args[0].GetString());
					}
					break;
				}

				case 2: {
					NotifyLoaded();
					break;
				}

				case 3: {
					if (a_params.argCount == 1 && a_params.args[0].IsBoolean()) {
						auto* ControlMap = RE::ControlMap::GetSingleton();
						ControlMap->SetTextEntryMode(a_params.args[0].GetBoolean());
					}
					break;
				}

				default: {
					break;
				}
			}
		}

		void MapCodeObjectFunctions () override {
			MapCodeMethodToASFunction("CloseMenu", 0);
			MapCodeMethodToASFunction("PlaySound", 1);
			MapCodeMethodToASFunction("NotifyLoaded", 2);
			MapCodeMethodToASFunction("SetTextEntry", 3);
		}

		RE::UI_MESSAGE_RESULTS ProcessMessage (RE::UIMessage& a_message) override {
			switch (*a_message.type) {
				case RE::UI_MESSAGE_TYPE::kShow: {
					if (auto* ControlMap = RE::ControlMap::GetSingleton()) {
						ControlMap->PushInputContext(RE::UserEvents::INPUT_CONTEXT_ID::kBasicMenuNav);
						ControlMap->PushInputContext(RE::UserEvents::INPUT_CONTEXT_ID::kThumbNav);
						ControlMap->PushInputContext(RE::UserEvents::INPUT_CONTEXT_ID::kVirtualController);
					}

					if (CustomMenu::IsLoaded) {
						NotifyLoaded();
						auto* ControlMap = RE::ControlMap::GetSingleton();
						ControlMap->SetTextEntryMode(true);
					}

					return RE::UI_MESSAGE_RESULTS::kHandled;
				}

				case RE::UI_MESSAGE_TYPE::kHide: {
					if (auto* ControlMap = RE::ControlMap::GetSingleton()) {
						ControlMap->PopInputContext(RE::UserEvents::INPUT_CONTEXT_ID::kBasicMenuNav);
						ControlMap->PopInputContext(RE::UserEvents::INPUT_CONTEXT_ID::kThumbNav);
						ControlMap->PopInputContext(RE::UserEvents::INPUT_CONTEXT_ID::kVirtualController);
					}

					return RE::UI_MESSAGE_RESULTS::kHandled;
				}

				default:
					return RE::IMenu::ProcessMessage(a_message);
			}
		}

	private:
		static RE::IMenu* Create (const RE::UIMessage& msg) {
			spdlog::debug("calling create");
			return new CustomMenu();
		}

		static void CloseMenu () {
			if (auto* UIMessageQueue = RE::UIMessageQueue::GetSingleton()) {
				UIMessageQueue->AddMessage("CustomMenu", RE::UI_MESSAGE_TYPE::kHide);
			}
		}

		void NotifyLoaded () {
			CustomMenu::IsLoaded = true;
			menuObj.Invoke("RefreshDisplay");
		}

		std::unique_ptr<RE::BSGFxShaderFXTarget> Background_mc { nullptr };
		std::unique_ptr<RE::BSGFxShaderFXTarget> Menu_mc { nullptr };
		inline static std::string HeaderText;
		inline static bool IsLoaded { false };
	};
}
