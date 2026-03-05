ScriptName PapyrusLib hidden native

; Create a new named save
; note: it will append [_PLAYERID] to correctly display the character name in the character selection screen in the main menu
; note: if the current player id is zero (game loaded from save game that had no player id embedded in the file name)
; a new player id will be generated and assigned
; Latent: returns true if it was able to save, false otherwise
bool function SaveGame (string aSaveName, string aSaveSuccessMessage = "", string aSaveFailureMessage = "") global native

; Load the last save game
; Note: this load the actual last save game, be it autosave, quicksave, manual save, console save, etc
function LoadLastSave () global native

; Gets the player id (uint32) as zero-padded hex string
; If the current player id is 0, a new player id will be generated, set, and returned
string function GetPlayerId () global native

; Sets a new player id from a hex string representing a uint32
; If input string is empty, a new player id will be generated
; returns zero-padded player id as uint32 hex string
; note: after setting a new player id, all saves (including autosaves, manual saves, quicksaves) will use this new player id
string function SetPlayerId (string aNewPlayerIdHex = "") global native

; Messages
; Supports fmt::format placeholders

function ShowMessage (string aFmtBase, var a1 = none, var a2 = none, var a3 = none, var a4 = none, var a5 = none, var a6 = none, var a7 = none, var a8 = none, var a9 = none) global native
function ConsoleLog (string aFmtBase, var a1 = none, var a2 = none, var a3 = none, var a4 = none, var a5 = none, var a6 = none, var a7 = none, var a8 = none, var a9 = none) global native
string function Format (string aFmtBase, var a1 = none, var a2 = none, var a3 = none, var a4 = none, var a5 = none, var a6 = none, var a7 = none, var a8 = none, var a9 = none) global native

; Register new name for $NAME
; must be called once per session, on every script using this
; otherwise will default to the name of the running script
string function Register (string aMapName) global native

; Logging
; Uses the current running script name as the log $NAME. File will be written to Documents/My Games/Fallout4/F4SE/$NAME.log
; Before logging, reads `[Logging] Level` from settings. If level >= chosen level, it won't log.

function LogI (string aFmtBase, var a1 = none, var a2 = none, var a3 = none, var a4 = none, var a5 = none, var a6 = none, var a7 = none, var a8 = none, var a9 = none) global native
function LogW (string aFmtBase, var a1 = none, var a2 = none, var a3 = none, var a4 = none, var a5 = none, var a6 = none, var a7 = none, var a8 = none, var a9 = none) global native
function LogE (string aFmtBase, var a1 = none, var a2 = none, var a3 = none, var a4 = none, var a5 = none, var a6 = none, var a7 = none, var a8 = none, var a9 = none) global native
function LogD (string aFmtBase, var a1 = none, var a2 = none, var a3 = none, var a4 = none, var a5 = none, var a6 = none, var a7 = none, var a8 = none, var a9 = none) global native

; Settings
; Uses the current running script name as the settings $NAME

; Loads settings from disk into memory cache
; Reads both MCM/Config/$NAME/settings.ini (the defaults) and MCM/Settings/$NAME.ini (the overrides)
bool function LoadSettings() global native

; Saves current in-memory settings to disk
; Reads defaults from MCM/Config and writes only to user MCM/Settings
; Values equal to defaults are removed from user settings
bool function SaveSettings() global native

; Returns string setting value from memory cache, or default if key is missing/invalid
string function GetSettingString (string aSection, string aKey, string aDefaultValue = "") global native
; Sets string setting value in memory cache
function SetSettingString (string aSection, string aKey, string aValue) global native

; Returns bool setting value from memory cache, or default if key is missing/invalid
bool function GetSettingBool (string aSection, string aKey, bool aDefaultValue = false) global native
; Sets bool setting value in memory cache
function SetSettingBool (string aSection, string aKey, bool aValue) global native

; Returns int setting value from memory cache, clamped to [aMinValue, aMaxValue], or default
int function GetSettingInt (string aSection, string aKey, int aDefaultValue = 0, int aMinValue = -2147483647, int aMaxValue = 2147483647) global native
; Sets int setting value in memory cache
function SetSettingInt (string aSection, string aKey, int aValue) global native

; Returns float setting value from memory cache, clamped to [aMinValue, aMaxValue], or default
float function GetSettingFloat (string aSection, string aKey, float aDefaultValue = 0.0, float aMinValue = -340282346638528860000000000000000000000.0, float aMaxValue = 340282346638528860000000000000000000000.0) global native
; Sets float setting value in memory cache
function SetSettingFloat (string aSection, string aKey, float aValue) global native

; Reads all supported struct fields from memory cache section into aStruct
function GetSettings (string aSection, var aStruct) global native
; Writes all supported struct fields from aStruct into section
function SetSettings (string aSection, var aStruct) global native

string function RequestText () global
	bool ok = _OpenTextInputMenu()
	if (!ok)
		return ""
	endif

	return _GetLastTextInputResult()
endFunction

bool function UpdatePowerArmor3d (ObjectReference aRef) global native

bool function _OpenTextInputMenu () global native
string function _GetLastTextInputResult () global native

; function LoadGame (string aSaveName) global native
; function QueueSave () global native
string function GetCallerName () global native
