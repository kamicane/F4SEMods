#!/usr/bin/env node
import { readFileSync, writeFileSync, mkdirSync, existsSync } from "node:fs"
import { join } from "node:path"

const presetsPath = join(process.cwd(), "CMakePresets.json")
const vscodeDir = join(process.cwd(), ".vscode")
const tasksPath = join(vscodeDir, "tasks.json")

const presets = JSON.parse(readFileSync(presetsPath, "utf8"))
const nonHiddenConfigurePresets = (presets.configurePresets ?? []).filter((p) => !p.hidden)
const basePresets = (presets.configurePresets ?? []).filter((p) => p.hidden && p.displayName)

const commonPresentation = {
	echo: true,
	reveal: "always",
	focus: true,
	panel: "shared",
	showReuseMessage: true,
	clear: false
}

const tasks = []

tasks.push({
	type: "cmake",
	label: `[Default] Cmake Build`,
	command: "build",
	targets: ["all"],
	preset: "__defaultBuildPreset__",
	group: { kind: "build", isDefault: true },
	presentation: commonPresentation,
	problemMatcher: []
})

function findBasePreset(baseName) {
	for (const preset of presets.configurePresets) {
		if (preset.name == baseName) {
			return preset;
			// const version = preset.cacheVariables?.PROJECT_VERSION
			// if (version != null) return version
		}
	}
}

// pack version does this. keeping for ref.
function findBaseVersion(base) {
	for (const preset of presets.configurePresets) {
		if (preset.name == base) {
			const version = preset.cacheVariables?.PROJECT_VERSION
			if (version != null) return version
		}
	}
}

for (const preset of basePresets) {
	// Deploy task
	tasks.push({
		label: `[${preset.name}] Deploy`,
		type: "process",
		hide: false,
		command: "node",
		args: ["deploy-project.mjs", preset.name, "${env:USERPROFILE}/MO2Data/Fallout4/mods/" + preset.displayName],
		options: { cwd: "${workspaceFolder}" },
		presentation: commonPresentation,
		problemMatcher: []
	})

	// Compile Papyrus task
	tasks.push({
		label: `[${preset.name}] Compile Papyrus Scripts`,
		type: "process",
		hide: false,
		command: "${env:USERPROFILE}/bin/FO4/Papyrus_Compiler_1.10.163_Patched/PapyrusCompiler.exe",
		args: ["${workspaceFolder}/Projects/" + `${preset.displayName}/project.ppj`],
		options: { cwd: "${workspaceFolder}/Projects/" + preset.displayName },
		presentation: commonPresentation,
		problemMatcher: []
	})
}

for (const preset of nonHiddenConfigurePresets) {
	const id = preset.name
	const displayName = preset.displayName

	const baseName = preset.inherits
	const basePreset = findBasePreset(baseName)
	const baseDisplayName = basePreset.displayName

	console.log(baseName, id, displayName)

	// CMake build task
	tasks.push({
		type: "cmake",
		label: `[${id}] Build`,
		command: "build",
		targets: ["all"],
		preset: id,
		group: { kind: "build", isDefault: false },
		presentation: commonPresentation,
		problemMatcher: []
	})

	// Pack task
	tasks.push({
		label: `[${id}] Pack`,
		type: "process",
		hide: false,
		command: "bash",
		args: ["pack-project.sh", baseName, id],
		options: { cwd: "${workspaceFolder}" },
		presentation: commonPresentation,
		problemMatcher: []
	})

	// Meta task: build + deploy
	tasks.push({
		label: `[${id}] Build and Deploy`,
		dependsOn: [
			`[${id}] Build`,
			`[${baseName}] Deploy`
		],
		dependsOrder: "sequence",
		problemMatcher: []
	})

	// Meta task: build + compile papyrus + deploy
	tasks.push({
		label: `[${id}] Build, Compile Papyrus and Deploy`,
		dependsOn: [
			`[${id}] Build`,
			`[${baseName}] Compile Papyrus Scripts`,
			`[${baseName}] Deploy`
		],
		dependsOrder: "sequence",
		problemMatcher: []
	})
}

if (!existsSync(vscodeDir)) {
	mkdirSync(vscodeDir)
}

writeFileSync(tasksPath, JSON.stringify({ version: "2.0.0", tasks }, null, 2))
