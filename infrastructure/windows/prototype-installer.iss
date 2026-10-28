; Flow DaVinci (prototip): fișierele de rulare rămân împreună; Resolve și setările utilizatorului nu se modifică. Netestat pe Windows real.
#ifndef SourceRoot
  #error SourceRoot is required
#endif
#ifndef BuildVersion
  #error BuildVersion is required
#endif
#ifndef BuildRevision
  #error BuildRevision is required
#endif
[Setup]
AppId=FlowDaVinci-GDC
AppName=Flow DaVinci
AppVersion={#BuildVersion}
AppVerName=Flow DaVinci {#BuildVersion} prototip.{#BuildRevision}
AppPublisher=Cristi Gordas (GDC); bazat pe OpenGate de CineWaltz și Unbound de Wolfgang Woehl
DefaultDirName={autopf}\FlowDaVinci
DefaultGroupName=Flow DaVinci
; Resolve discovers native workflow shortcut targets in the shared ProgramData
; directory. Standard installer elevation writes only OpenGate-owned files there.
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
OutputDir={#SourceRoot}\release\packages
OutputBaseFilename=FlowDaVinci-{#BuildVersion}-Prototype.{#BuildRevision}-Windows-x64
Compression=lzma2
SolidCompression=yes
SetupLogging=yes
CloseApplications=yes
RestartApplications=no
UninstallDisplayIcon={app}\Unbound.exe
[Files]
Source: "{#SourceRoot}\release\win\{#BuildVersion}\FlowDaVinci.dvcp"; DestDir: "{commonappdata}\Blackmagic Design\DaVinci Resolve\Support\IOPlugins\FlowDaVinci.dvcp.bundle\Contents\Win64"; Flags: ignoreversion
Source: "{#SourceRoot}\release\win\{#BuildVersion}\Unbound.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceRoot}\release\win\{#BuildVersion}\Unbound.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceRoot}\LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceRoot}\infrastructure\windows\prototype-notes.txt"; DestDir: "{app}"; DestName: "README.txt"; Flags: ignoreversion
Source: "{#SourceRoot}\release\win\{#BuildVersion}\release-manifest.json"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceRoot}\release\win\{#BuildVersion}\workflow-commands\*.py"; DestDir: "{commonappdata}\Blackmagic Design\DaVinci Resolve\Support\Workflow Integration Plugins"; Flags: ignoreversion
Source: "{#SourceRoot}\release\win\{#BuildVersion}\workflow-commands\manifest.json"; DestDir: "{app}"; DestName: "workflow-commands-manifest.json"; Flags: ignoreversion
Source: "{#SourceRoot}\infrastructure\windows\export-support.ps1"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceRoot}\infrastructure\windows\export-support.cmd"; DestDir: "{app}"; DestName: "Exporta jurnale suport Flow DaVinci.cmd"; Flags: ignoreversion
[Icons]
Name: "{group}\Flow DaVinci"; Filename: "{app}\Unbound.exe"
Name: "{group}\Exportă jurnale de suport Flow DaVinci"; Filename: "{app}\Exporta jurnale suport Flow DaVinci.cmd"
Name: "{group}\Limitări ale prototipului"; Filename: "{app}\README.txt"
Name: "{group}\Dezinstalează Flow DaVinci"; Filename: "{uninstallexe}"
[Code]
function ResolveStopReason(): String;
var Locator, Services, Processes: Variant;
begin
  Result := '';
  try
    Locator := CreateOleObject('WbemScripting.SWbemLocator');
    Services := Locator.ConnectServer('', 'root\CIMV2');
    Processes := Services.ExecQuery('SELECT ProcessId FROM Win32_Process WHERE Name="Resolve.exe"');
    if Processes.Count > 0 then
      Result := 'Salvează lucrul și închide DaVinci Resolve înainte de a instala sau dezinstala Flow DaVinci.';
  except
    Result := 'Nu s-a putut verifica dacă Resolve este închis. Închide Resolve și reîncearcă.';
  end;
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
begin
  Result := ResolveStopReason();
end;
function InitializeUninstall(): Boolean;
var Reason: String;
begin
  Reason := ResolveStopReason();
  Result := Reason = '';
  if not Result then MsgBox(Reason, mbInformation, MB_OK);
end;
