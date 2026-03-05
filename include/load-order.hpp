#pragma once

#include "util.hpp"

class LoadOrder {
private:
	using IDNameFileMap_t = std::map<std::uint32_t, std::string>;
	using NameIDFileMap_t = std::map<std::string, std::uint32_t>;

	inline static IDNameFileMap_t idNameFileMap;
	inline static NameIDFileMap_t nameIDFileMap;

	// Returns true when the form ID belongs to the virtual form space.
	[[nodiscard]] static bool IsVirtualForm (std::uint32_t formID) {
		return (formID >> 24) == 0xFF;
	}

	// Computes the load-order file ID prefix for a TES file.
	[[nodiscard]] static std::uint32_t ComputeFileID (const RE::TESFile* file) {
		return (file->compileIndex << 24) + (file->smallFileCompileIndex << 12);
	}

public:
	// Builds both mod-name and file-ID lookup maps from current load order.
	static void ComputePlugins () {
		idNameFileMap.clear();
		nameIDFileMap.clear();

		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) {
			return;
		}

		for (auto* file : dataHandler->files) {
			if (!file || file->compileIndex == 0xFF) continue;

			auto fileID = ComputeFileID(file);
			auto fileName = std::string(file->GetFilename());

			nameIDFileMap[fileName] = fileID;
			idNameFileMap[fileID] = fileName;
		}
	}

	// Returns the map of mod filename to file ID.
	static NameIDFileMap_t* GetNameIDFileMap () {
		return &nameIDFileMap;
	}

	// Returns the map of file ID to mod filename.
	static IDNameFileMap_t* GetIDNameFileMap () {
		return &idNameFileMap;
	}

	class FormKey {
	public:
		std::uint32_t id;
		std::string modKey;

		// Builds a FormKey from local ID and mod key.
		[[nodiscard]] static FormKey From (std::uint32_t id, std::string_view modKey) {
			return FormKey { .id = id, .modKey = std::string(modKey) };
		}

		// Parses a FormKey from "LOCAL_ID:MOD_NAME".
		[[nodiscard]] static FormKey From (std::string_view formKey) {
			auto separator = formKey.find(':');
			if (separator == std::string_view::npos) return { .id = 0, .modKey = "" };

			auto localIdHex = formKey.substr(0, separator);
			auto mod = formKey.substr(separator + 1);
			if (localIdHex.empty() || mod.empty()) return { .id = 0, .modKey = "" };

			auto localId = Util::Uint32FromHex(localIdHex);
			if (localId == 0) return { .id = 0, .modKey = "" };

			return FormKey { .id = localId, .modKey = std::string(mod) };
		}

		// Resolves this key into a full game form ID using current load order.
		[[nodiscard]] std::uint32_t GetFormID () const {
			auto* nameIDMap = GetNameIDFileMap();
			if (!nameIDMap->contains(modKey)) return 0;

			return nameIDMap->at(modKey) + id;
		}

		// Resolves this key to a TESForm pointer, or null if unresolved.
		[[nodiscard]] RE::TESForm* Resolve () const {
			auto formID = GetFormID();
			return formID > 0 ? RE::TESForm::GetFormByID(formID) : nullptr;
		}

		// Resolves this key to a typed TESForm pointer, or null if unresolved.
		template<class T>
		[[nodiscard]] T* Resolve () const {
			auto formID = GetFormID();
			return formID > 0 ? RE::TESForm::GetFormByID<T>(formID) : nullptr;
		}

		// Formats this key as "LOCAL_ID:MOD_NAME".
		operator std::string () const {
			return fmt::format("{:X}:{:s}", id, modKey);
		}

		explicit operator bool () const {
			return id != 0 && !modKey.empty();
		}
	};

	// Extracts the load-order file ID component from a full form ID.
	[[nodiscard]] static std::uint32_t GetFileID (std::uint32_t formID) {
		return (formID >> 24) == 0xFE ? formID & 0xFFFFF000 : formID & 0xFF000000;
	}

	// Resolves the owning mod filename for a form ID, if available.
	[[nodiscard]] static std::optional<std::string> GetModKey (std::uint32_t formID) {
		if (IsVirtualForm(formID)) {
			return "Virtual";
		}

		auto* idNameMap = GetIDNameFileMap();
		auto fileID = GetFileID(formID);
		if (idNameMap->contains(fileID)) return idNameMap->at(fileID);

		return std::nullopt;
	}

	// Converts a full form ID to its local (plugin-relative) ID.
	[[nodiscard]] static std::uint32_t Localize (std::uint32_t formID) {
		return (formID >> 24) == 0xFE ? formID & 0x00000FFF : formID & 0x00FFFFFF;
	}

	// Converts a full form ID to a FormKey of local ID plus owning mod key.
	[[nodiscard]] static FormKey GetFormKey (std::uint32_t formID) {
		auto localizedFormID = Localize(formID);
		if (IsVirtualForm(formID)) {
			return FormKey { .id = localizedFormID, .modKey = "Virtual" };
		}

		auto* idNameMap = GetIDNameFileMap();
		auto fileID = GetFileID(formID);

		if (idNameMap->contains(fileID)) {
			return FormKey { .id = localizedFormID, .modKey = idNameMap->at(fileID) };
		}

		return FormKey { .id = localizedFormID, .modKey = fmt::format("{:x}", fileID) };
	}
};
