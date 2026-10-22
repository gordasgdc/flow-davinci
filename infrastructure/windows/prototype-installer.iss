; OpenGate prototype: keep runtime files together; never modify Resolve or user settings.
#ifndef SourceRoot
  #error SourceRoot is required
#endif
#ifndef BuildVersion
  #error BuildVersion is required
#endif
[Setup]
AppId=OpenGate-CineWaltz-Prototype
AppName=OpenGate
AppVersion={#BuildVersion}
AppPublisher=CineWaltz; based on Unbound by Wolfgang Woehl
DefaultDirName={localappdata}\Programs\OpenGate
DefaultGroupName=OpenGate
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
OutputDir={#SourceRoot}\release\packages
OutputBaseFilename=OpenGate-{#BuildVersion}-Prototype-Windows-x64
Compression=lzma2
SolidCompression=yes
SetupLogging=yes
CloseApplications=yes
RestartApplications=no
UninstallDisplayIcon={app}\Unbound.exe
[Files]
Source: "{#SourceRoot}\release\win\{#BuildVersion}\Unbound.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceRoot}\release\win\{#BuildVersion}\Unbound.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceRoot}\LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceRoot}\infrastructure\windows\prototype-notes.txt"; DestDir: "{app}"; DestName: "README.txt"; Flags: ignoreversion
[Icons]
Name: "{group}\OpenGate"; Filename: "{app}\Unbound.exe"
Name: "{group}\Prototype limitations"; Filename: "{app}\README.txt"
Name: "{group}\Uninstall OpenGate"; Filename: "{uninstallexe}"
[Code]
function PrepareToInstall(var NeedsRestart: Boolean): String;
var Locator, Services, Processes: Variant;
begin
  Result := '';
  try
    Locator := CreateOleObject('WbemScripting.SWbemLocator');
    Services := Locator.ConnectServer('', 'root\CIMV2');
    Processes := Services.ExecQuery('SELECT ProcessId FROM Win32_Process WHERE Name="Resolve.exe"');
    if Processes.Count > 0 then
      Result := 'Save your work and quit DaVinci Resolve before installing OpenGate.';
  except
    Result := 'Could not check whether Resolve is closed. Close Resolve and retry.';
  end;
end;
