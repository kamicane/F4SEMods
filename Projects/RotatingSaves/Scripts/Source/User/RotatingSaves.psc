ScriptName RotatingSaves native

; Saves next slot
function SaveNextSlot () global native

; Loads last save
function LoadLastSave () global native

; what do you think this does
function ShowSaveMenu () global native

; convenience functions

; cgf "RotatingSaves.SaveGame" "My amazing save". Will handle player id.
bool function SaveGame (string saveName, string successMessage = "", string failureMessage = "") global native

; cgf "RotatingSaves.LoadGame" "My amazing save".  Will handle player id. Note this works for the current player saves only.
function LoadGame (string saveName) global native

; Gets the player id (uint32) as zero-padded hex string
string function GetPlayerId () global native

; Sets a new player id from a hex string representing a uint32
; If input string is empty or invalid, a new player id will be generated
; returns zero-padded player id as uint32 hex string
; note: after setting a new player id, all save operations (including autosaves, manual saves, quicksaves) will use this new player id
string function SetPlayerId (string newPlayerId = "") global native

; Creates a normal save (like from the menu -> save)
function ForceSave () global native

; Creates an autosave
function AutoSave () global native
