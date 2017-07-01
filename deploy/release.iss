; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "TTM25"
#define MyAppVersion "0.1"
#define MyAppPublisher "�� ����������"
#define MyAppExeName "ttm25.exe"
#define LinkName "������������� ����� ���25"
#define MetranExeName "test.exe"
#define MetranLinkName "��������� ������"
#define BUILD_DIR_main "\\VBOXSVR\Work\build-ttm25-Desktop_Qt_5_5_0_MinGW_32bit-Release\release\"
#define BUILD_DIR_test "\\VBOXSVR\Work\build-test-Desktop_Qt_5_5_0_MinGW_32bit-Release\release\"
#define QT_DIR "C:\Qt\5.5\mingw492_32\"
[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{F517A171-F620-4A4E-BD0A-E5514C2B03E1}
AppName={#LinkName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#LinkName}
AllowNoIcons=yes
OutputDir=\\VBOXSVR\Work\kb\ttm25\deploy
OutputBaseFilename=TTM25_Win_32
Compression=lzma
SolidCompression=yes
SourceDir = \\VBOXSVR\Work\kb\ttm25\


[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "{#BUILD_DIR_main}ttm25.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BUILD_DIR_test}test.exe"; DestDir: "{app}"; Flags: ignoreversion

Source: "img\vershina_small.png"; DestDir: "{app}/img"; Flags: ignoreversion

Source: "data\kkt.json"; DestDir: "{app}"; Flags: ignoreversion
Source: "data\marks.json"; DestDir: "{app}"; Flags: ignoreversion
Source: "data\models.json"; DestDir: "{app}"; Flags: ignoreversion
Source: "data\tires_mark.json"; DestDir: "{app}"; Flags: ignoreversion

Source: "{#QT_DIR}bin\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT_DIR}bin\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT_DIR}bin\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT_DIR}bin\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT_DIR}bin\Qt5PrintSupport.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT_DIR}bin\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT_DIR}bin\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT_DIR}bin\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion


Source: "{#QT_DIR}plugins\platforms\qwindows.dll"; DestDir: "{app}\plugins\platforms"; Flags: ignoreversion
Source: "{#QT_DIR}plugins\printsupport\windowsprintersupport.dll"; DestDir: "{app}\plugins\printsupport"; Flags: ignoreversion
;Source: "C:\Qt\5.5\mingw492_32\plugins\platforms\qwindowsd.dll"; DestDir: "{app}\plugins\platforms"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#LinkName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{#MetranLinkName}"; Filename: "{app}\{#MetranExeName}"
Name: "{group}\{cm:UninstallProgram,{#LinkName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#LinkName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{commondesktop}\{#MetranLinkName}"; Filename: "{app}\{#MetranExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(LinkName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

