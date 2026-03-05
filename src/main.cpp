#include "project.h"
#include "util.hpp"
#include "project-main.hpp"
#include "events.hpp"

#define DLLEXPORT __declspec(dllexport)

#if GAME_VERSION == 1

// old CommonLib has no Version pack function
constexpr std::uint32_t PackVersion (const REL::Version& version) noexcept {
	return static_cast<std::uint32_t>(
		(version[0] & 0x0FF) << 24U | (version[1] & 0x0FF) << 16U | (version[2] & 0xFFF) << 4U | (version[3] & 0x00F) << 0U
	);
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query (const F4SE::QueryInterface* f4se, F4SE::PluginInfo* info) {
	info->infoVersion = F4SE::PluginInfo::kVersion;
	info->name = Project::ID.data();
	info->version = PackVersion(Project::Version);

	const auto ver = f4se->RuntimeVersion();
	if (ver != F4SE::RUNTIME_1_10_163) {
		F4SE::log::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}
#endif

#if GAME_VERSION == 3
extern "C" DLLEXPORT constinit auto F4SEPlugin_Version = [] () noexcept {
	F4SE::PluginVersionData data {};

	data.PluginVersion(Project::Version);
	data.PluginName(Project::ID);
	data.AuthorName("kamicane");
	data.UsesAddressLibrary(true);
	data.UsesSigScanning(false);
	data.IsLayoutDependent(true);
	data.HasNoStructUse(false);
	data.CompatibleVersions({ F4SE::RUNTIME_1_11_169 });

	return data;
}();
#endif

void MessageHandler (F4SE::MessagingInterface::Message* message) {
	Events::Messaging.dispatch(message->type);
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load (const F4SE::LoadInterface* f4se) {
#if GAME_VERSION == 3
	F4SE::Init(f4se, false);
#endif

#if GAME_VERSION == 1
	F4SE::Init(f4se);
#endif

	Util::LoadIni(Project::ID);

	auto logger = Util::GetLogger(Project::ID);
	spdlog::set_default_logger(logger);

	spdlog::info("Plugin {} v{} loaded", Project::ID, Project::Version);

	const auto* messaging = F4SE::GetMessagingInterface();
	if (!messaging) {
		spdlog::error("Fatal error: could not get F4SE messaging interface");
		return false;
	}

	messaging->RegisterListener(MessageHandler);

	Project::main();

	return true;
}
