; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Clavis"
#define MyAppVersion "1.2.2"
#define MyAppPublisher "Manel Castillo Gimenez"
#define MyAppURL "https://github.com/ManelCG/Clavis"
#define MyAppExeName "clavis.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{E9F0B3A4-EB99-42BD-BE5F-E64D768583B2}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputDir=G:\clavis\releases\installers
OutputBaseFilename=clavis_setup_win64
SetupIconFile=G:\clavis\assets\app_icon\256.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "G:\clavis\build\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libatk-1.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libbrotlicommon.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libbrotlidec.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libbz2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libcairo-2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libcairo-gobject-2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libcrypto-1_1-x64.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libdatrie-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libepoxy-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libexpat-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libffi-8.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libfontconfig-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libfreetype-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libfribidi-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libgdk_pixbuf-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libgdk-3-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libgio-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libglib-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libgmodule-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libgobject-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libgraphite2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libgtk-3-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libharfbuzz-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libiconv-2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libintl-8.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libpango-1.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libpangocairo-1.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libpangoft2-1.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libpangowin32-1.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libpcre-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libpixman-1-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libpng16-16.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libthai-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\build\zlib1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "G:\clavis\__windows__\windows_assets\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs
Source: "G:\clavis\assets\*"; DestDir: "{app}\assets\"; Flags: ignoreversion recursesubdirs
Source: "G:\clavis\__windows__\windows_configs\*"; DestDir: "{userappdata}\Clavis\"; Flags: ignoreversion recursesubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files


[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

