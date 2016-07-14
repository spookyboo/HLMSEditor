[Setup]
AppName=HLMS Editor
AppVersion=1.0
DefaultDirName={pf}\HLMSEditor
; Since no icons will be created in "{group}", we don't need the wizard
; to ask for a Start Menu folder name:
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\HLMSEditor.exe
OutputBaseFilename=setup
OutputDir=userdocs:Setup 
Compression=lzma
SolidCompression=yes
DefaultGroupName=HLMS Editor

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\Henry\Documents\Visual Studio 2015\Projects\HLMSEditor_bin\HLMSEditor\*.*"; DestDir: "{app}\"; Flags: ignoreversion recursesubdirs createallsubdirs

[Dirs]
Name: "{app}"; 
Name: "{app}\common\thumbs"; Permissions: everyone-full
Name: "{app}\bin"; Permissions: everyone-full

[Icons]
Name: "{group}\HLMS Editor"; Filename: "{app}\bin\HLMSEditor.exe"; IconFilename: "C:\Users\Henry\Documents\Visual Studio 2015\Projects\HLMSEditor\common\icons\HLMSEditor.ico"
Name: "{commondesktop}\HLMS Editor"; Tasks: desktopicon; Filename: "{app}\bin\HLMSEditor.exe"; IconFilename: "C:\Users\Henry\Documents\Visual Studio 2015\Projects\HLMSEditor\common\icons\HLMSEditor.ico"
Name: "{group}\HLMS Editor\{cm:UninstallProgram,HLMS Editor}"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\bin\HLMSEditor.exe"; Description: "{cm:LaunchProgram,HLMS Editor}"; Flags: nowait postinstall skipifsilent
