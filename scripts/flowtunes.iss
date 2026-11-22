#define AppName "Flowtunes"
#define AppVersion "0.2.0"
[Setup]
AppId={{A826AABD-8D8D-47A2-90A7-AFA425F4711A}
AppName={#AppName}
AppVersion={#AppVersion}
DefaultDirName={autopf}\Flowtunes
DefaultGroupName=Flowtunes
OutputDir=..\dist
OutputBaseFilename=Flowtunes-Setup
Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=lowest
[Files]
Source: "..\dist\Flowtunes\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
[Icons]
Name: "{group}\Flowtunes"; Filename: "{app}\flowtunes.exe"
[Run]
Filename: "{app}\flowtunes.exe"; Description: "Launch Flowtunes"; Flags: nowait postinstall skipifsilent
