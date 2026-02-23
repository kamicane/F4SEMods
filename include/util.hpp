#pragma once
#include <SimpleIni.h>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>

namespace Util {

	std::string Trim (std::string_view input) {
		size_t first = 0;
		size_t last = input.size();

		while (first < last && std::isspace(static_cast<unsigned char>(input[first]))) {
			++first;
		}
		while (last > first && std::isspace(static_cast<unsigned char>(input[last - 1]))) {
			--last;
		}
		return std::string(input.substr(first, last - first));
	}

	std::string ToLower (std::string_view input) {
		auto result = std::string(input);
		for (char& chr : result) {
			chr = static_cast<char>(std::tolower(static_cast<unsigned char>(chr)));
		}
		return result;
	}

	std::string PadStart (std::string_view input, std::uint16_t maxLength, std::string_view padChar) {
		auto inputStr = std::string(input);

		int padCount = maxLength - static_cast<int>(input.size());
		if (padCount <= 0) return inputStr;

		std::string padding;
		for (int i = 0; i < padCount; ++i) padding += padChar;

		return padding + inputStr;
	}

	std::string PadEnd (std::string_view input, std::uint16_t maxLength, std::string_view padChar) {
		auto inputStr = std::string(input);

		int padCount = maxLength - static_cast<int>(input.size());
		if (padCount <= 0) return inputStr;

		std::string padding;
		for (int i = 0; i < padCount; ++i) padding += padChar;

		return inputStr + padding;
	}

	std::uint32_t RandomUInt32 () {
		static std::mt19937 gen(std::random_device {}());
		static std::uniform_int_distribution<std::uint32_t> dist(1U, 0xFFFFFFFFU);
		return dist(gen);
	}

	std::uint32_t Uint32FromHex (std::string_view uint32Hex) {
		if (uint32Hex.empty()) {
			return 0;
		}

		try {
			unsigned long val = std::stoul(std::string(uint32Hex), nullptr, 16);
			return static_cast<std::uint32_t>(val);
		} catch (...) {
			return 0;
		}
	}

	std::filesystem::path GetGamePath () {
		static std::filesystem::path cachedDir;

		if (cachedDir.empty()) {
			std::wstring buf(MAX_PATH, L'\0');
			const DWORD len = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
			if (len != 0) {
				buf.resize(len);
				cachedDir = (std::filesystem::path(buf).parent_path()).lexically_normal();
			} else {
				cachedDir = std::filesystem::temp_directory_path();
			}
		}

		return cachedDir;
	}

	std::filesystem::path GetMyDocumentsPath () {
		static std::filesystem::path cachedDir;

		if (cachedDir.empty()) {
			std::wstring docsPath(MAX_PATH, L'\0');
			if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_MYDOCUMENTS, nullptr, SHGFP_TYPE_CURRENT, docsPath.data()))) {
				const size_t len = std::wcslen(docsPath.c_str());
				docsPath.resize(len);
				cachedDir = (std::filesystem::path(docsPath) / L"My Games" / L"Fallout4").lexically_normal();
			} else {
				cachedDir = std::filesystem::temp_directory_path();
			}
		}

		return cachedDir;
	}

	std::filesystem::path GetMCMPath () {
		static std::filesystem::path cachedDir;

		if (cachedDir.empty()) {
			cachedDir = GetGamePath() / "Data/MCM";
		}

		return cachedDir;
	}

	std::filesystem::path GetSystemConfigPath (std::string_view stem) {
		auto mcmPath = GetMCMPath();
		return mcmPath / "Config" / stem / "settings.ini";
	}

	std::filesystem::path GetUserConfigPath (std::string_view stem) {
		auto mcmPath = GetMCMPath();

		return mcmPath / "Settings" / (std::string(stem) + ".ini");
	}

	std::filesystem::path GetUserLogPath (std::string_view stem) {
		static std::filesystem::path cachedDir;

		if (cachedDir.empty()) {
			cachedDir = GetMyDocumentsPath() / "F4SE";
		}

		std::string fileNameLog;
		fileNameLog += stem;
		fileNameLog += ".log";

		return cachedDir / fileNameLog;
	}

	spdlog::level::level_enum level_from_str (std::string_view level) {
		auto logLevel = spdlog::level::debug;
		if (level.empty()) return logLevel;

		try {
			logLevel = spdlog::level::from_str(std::string(level));
		} catch (...) {
			logLevel = spdlog::level::debug;
		}

		return logLevel;
	}

	std::shared_ptr<spdlog::logger> CreateLogger (
		const std::string_view userId,
		const std::filesystem::path& logFile,
		std::string_view level
	) {
		auto logger = spdlog::basic_logger_mt<spdlog::async_factory>(std::string(userId), logFile.string(), true);

		auto logLevel = level_from_str(level);

		logger->set_level(logLevel);
		logger->flush_on(logLevel);

		logger->set_pattern("[%H:%M:%S.%f] [%L] %v");

		return logger;
	}

	std::string SanitizeFileName (std::string_view input, bool replaceUnderscores) {
		static const std::regex invalid_re(R"([<>:"/\\|?*\x00-\x1F])");
		static const std::regex multi_space_re(R"(\s+)");
		static const std::regex trim_re(R"(^\s+|\s+$)");
		static const std::regex underscore_re(R"(_+)");

		auto safe = std::string(input);

		if (replaceUnderscores) safe = std::regex_replace(safe, underscore_re, std::string(" "));

		safe = std::regex_replace(safe, invalid_re, std::string(" "));
		safe = std::regex_replace(safe, multi_space_re, std::string(" "));
		safe = std::regex_replace(safe, trim_re, std::string(""));

		return safe;
	}

	std::shared_ptr<CSimpleIniA> LoadIni (std::string_view userId) {
		auto ini = std::make_shared<CSimpleIniA>();
		ini->SetUnicode(true);
		if (userId.empty()) return ini;

		auto systemPath = GetSystemConfigPath(userId);
		ini->LoadFile(systemPath.c_str());
		auto userPath = GetUserConfigPath(userId);
		ini->LoadFile(userPath.c_str());
		return ini;
	}

	std::string ReadIni (
		std::shared_ptr<CSimpleIniA> ini,
		std::string_view section,
		std::string_view key,
		std::string_view defaultValue = ""
	) {
		if (!ini) return Trim(defaultValue);

		const std::string section_s { section };
		const std::string key_s { key };

		if (const auto* val = ini->GetValue(section_s.c_str(), key_s.c_str())) return Trim(val);

		return Trim(defaultValue);
	}

	bool ReadIniBool (std::shared_ptr<CSimpleIniA> ini, std::string_view section, std::string_view key, bool defaultValue = false) {
		auto val = ReadIni(ini, section, key);
		if (val.empty()) return defaultValue;

		val = ToLower(val);

		if (val == "1" || val == "true" || val == "yes") return true;
		if (val == "0" || val == "false" || val == "no") return false;

		return defaultValue;
	}

	int32_t ReadIniInt (
		std::shared_ptr<CSimpleIniA> ini,
		std::string_view section,
		std::string_view key,
		int32_t defaultValue = 0,
		int32_t min = std::numeric_limits<int32_t>::min(),
		int32_t max = std::numeric_limits<int32_t>::max()
	) {
		auto raw = ReadIni(ini, section, key);
		if (raw.empty()) return std::clamp<int32_t>(defaultValue, min, max);

		auto val = ToLower(raw);

		// Handle boolean-like values
		if (val == "true" || val == "yes") return std::clamp<int32_t>(1, min, max);
		if (val == "false" || val == "no") return std::clamp<int32_t>(0, min, max);

		// Prepare numeric parse (support hex 0x...)
		int base = 10;
		std::string parseStr = raw;
		if (parseStr.size() > 2 && parseStr[0] == '0' && (parseStr[1] == 'x' || parseStr[1] == 'X')) {
			base = 16;
		}

		long long parsed = 0;
		try {
			parsed = std::stoll(parseStr, nullptr, base);
		} catch (...) {
			return std::clamp<int32_t>(defaultValue, min, max);
		}

		// Clamp to int32_t range first
		if (parsed < static_cast<long long>(std::numeric_limits<int32_t>::min()))
			parsed = std::numeric_limits<int32_t>::min();
		if (parsed > static_cast<long long>(std::numeric_limits<int32_t>::max()))
			parsed = std::numeric_limits<int32_t>::max();

		auto result = static_cast<int32_t>(parsed);
		if (result < min) return min;
		if (result > max) return max;
		return result;
	}

	void WriteIni (std::string_view userId, std::string_view section, std::string_view key, std::string_view value) {
		if (userId.empty()) return;

		const std::string section_s { section };
		const std::string key_s { key };
		const std::string value_s { value };

		auto userPath = GetUserConfigPath(userId);

		CSimpleIniA ini;
		ini.SetUnicode(true);
		ini.LoadFile(userPath.c_str());

		if (value_s.empty()) {
			ini.Delete(section_s.c_str(), key_s.c_str(), true);
		} else {
			ini.SetValue(section_s.c_str(), key_s.c_str(), value_s.c_str());
		}

		ini.SaveFile(userPath.c_str());
	}

	void WriteIniBool (std::string_view userId, std::string_view section, std::string_view key, bool value) {
		WriteIni(userId, section, key, value ? "true" : "false");
	}

	void WriteIniInt (std::string_view userId, std::string_view section, std::string_view key, int32_t value) {
		auto str = std::to_string(value);
		WriteIni(userId, section, key, str);
	}

	std::shared_ptr<spdlog::logger> GetLogger (std::string_view userId) {
		if (userId.empty()) return nullptr;

		const std::string userId_s { userId };

		auto logger = spdlog::get(userId_s);

		if (!logger) {
			auto ini = LoadIni(userId);
			auto logLevel = ToLower(ReadIni(ini, "Logging", "Level"));

			auto fullPath = GetUserLogPath(userId);
			logger = CreateLogger(userId_s, fullPath, logLevel);
		}

		return logger;
	}

	void SetLogPattern (std::string_view userId, std::string_view pattern) {
		auto logger = GetLogger(userId);
		if (!logger) return;

		logger->set_pattern(pattern.data()); // NOLINT
	}
}
