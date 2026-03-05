#pragma once

namespace MenuUtil {
	template<class T>
	constexpr bool always_false = false;

	template<typename T>
	RE::Scaleform::GFx::Value MakeValue (RE::Scaleform::Ptr<RE::IMenu> menu, T&& val) {
		RE::Scaleform::GFx::Value out;
		using U = std::decay_t<T>;
		if constexpr (std::is_same_v<U, bool>) {
			out = val;
		} else if constexpr (std::is_integral_v<U> || std::is_floating_point_v<U>) {
			out = static_cast<double>(val);
		} else if constexpr (std::is_same_v<U, std::string>) {
			auto uiMovie = menu ? menu->uiMovie : RE::Scaleform::Ptr<RE::Scaleform::GFx::Movie> {};
			if (uiMovie && uiMovie->asMovieRoot) {
				uiMovie->asMovieRoot->CreateString(&out, val.c_str());
			}
		} else if constexpr (std::is_convertible_v<U, const char*>) {
			auto uiMovie = menu ? menu->uiMovie : RE::Scaleform::Ptr<RE::Scaleform::GFx::Movie> {};
			if (uiMovie && uiMovie->asMovieRoot) {
				uiMovie->asMovieRoot->CreateString(&out, static_cast<const char*>(val));
			}
		} else if constexpr (std::is_same_v<U, std::string_view>) {
			const std::string tmp { std::forward<T>(val) };
			auto uiMovie = menu ? menu->uiMovie : RE::Scaleform::Ptr<RE::Scaleform::GFx::Movie> {};
			if (uiMovie && uiMovie->asMovieRoot) {
				uiMovie->asMovieRoot->CreateString(&out, tmp.c_str());
			}
		} else {
			static_assert(always_false<T>, "Unsupported arg type");
		}
		return out;
	}

	template<typename T>
	RE::Scaleform::GFx::Value MakeValue (RE::GameMenuBase* menu, T&& val) {
		return MakeValue(RE::Scaleform::Ptr<RE::IMenu>{ menu }, std::forward<T>(val));
	}

	bool SetMember (RE::Scaleform::Ptr<RE::IMenu> menu, std::string_view name, std::string_view str) {
		if (!menu) return false;
		auto& menuObj = menu->menuObj;
		if (!menuObj.IsObject()) return false;
		auto val = MakeValue(menu, str);
		return menuObj.SetMember(name, val);
	}

	bool SetMember (RE::GameMenuBase* menu, std::string_view name, std::string_view str) {
		return SetMember(RE::Scaleform::Ptr<RE::IMenu>{ menu }, name, str);
	}

	bool SetMember (RE::Scaleform::Ptr<RE::IMenu> menu, std::string_view name, double value) {
		if (!menu) return false;
		auto& menuObj = menu->menuObj;
		if (!menuObj.IsObject()) return false;
		auto val = MakeValue(menu, value);
		return menuObj.SetMember(name, val);
	}

	bool SetMember (RE::GameMenuBase* menu, std::string_view name, double value) {
		return SetMember(RE::Scaleform::Ptr<RE::IMenu>{ menu }, name, value);
	}

	bool SetMember (RE::Scaleform::Ptr<RE::IMenu> menu, std::string_view name, bool value) {
		if (!menu) return false;
		auto& menuObj = menu->menuObj;
		if (!menuObj.IsObject()) return false;
		auto val = MakeValue(menu, value);
		return menuObj.SetMember(name, val);
	}

	bool SetMember (RE::GameMenuBase* menu, std::string_view name, bool value) {
		return SetMember(RE::Scaleform::Ptr<RE::IMenu>{ menu }, name, value);
	}

	bool AttachObjectMember (RE::Scaleform::Ptr<RE::IMenu> menu, std::string_view name, RE::Scaleform::GFx::Value& obj) {
		if (!menu) return false;
		auto& menuObj = menu->menuObj;
		if (!menuObj.IsObject()) return false;
		return menuObj.SetMember(name, obj);
	}

	bool AttachObjectMember (RE::GameMenuBase* menu, std::string_view name, RE::Scaleform::GFx::Value& obj) {
		return AttachObjectMember(RE::Scaleform::Ptr<RE::IMenu>{ menu }, name, obj);
	}

	template<typename... Args>
	bool InvokeMethod (RE::Scaleform::Ptr<RE::IMenu> menu, std::string_view methodName, Args&&... args) {
		if (!menu) return false;
		auto& menuObj = menu->menuObj;
		if (!menuObj.IsObject()) return false;
		const std::string name { methodName };
		if constexpr (sizeof...(Args) == 0) {
			return menuObj.Invoke(name.c_str(), nullptr, nullptr, 0);
		} else {
			std::array<RE::Scaleform::GFx::Value, sizeof...(Args)> argv { MakeValue(menu, std::forward<Args>(args))... };
			return menuObj.Invoke(name.c_str(), nullptr, argv.data(), static_cast<std::uint32_t>(argv.size()));
		}
	}

	template<typename... Args>
	bool InvokeMethod (RE::GameMenuBase* menu, std::string_view methodName, Args&&... args) {
		return InvokeMethod(RE::Scaleform::Ptr<RE::IMenu>{ menu }, methodName, std::forward<Args>(args)...);
	}
}
