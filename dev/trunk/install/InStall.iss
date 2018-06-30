; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

[Setup]
; 注意: AppId 值用于唯一识别该应用程序。
; 禁止对其他应用程序的安装器使用相同的 AppId 值！
; (若要生成一个新的 GUID，请选择“工具 | 生成 GUID”。)
#define AppName "UCClient"
#define AppVersion "1.0.0.4"
AppId={{DF7797D1-6B99-4DB3-B78B-E47429FF3BD5}}
AppName={#AppName}
AppVersion={#AppVersion}
AppCopyright=Copyright (C)ytx Inc.
AppComments=ytx
VersionInfoVersion={#AppVersion}
VersionInfoDescription={#AppName}安装包
AppPublisher=ytx;AppPublisherURL=http://www.avaya.com/;AppSupportURL=http://www.avaya.com/;AppUpdatesURL=http://www.avaya.com/
DefaultDirName={pf32}\ytx\{#AppName}
DefaultGroupName=ytx\{#AppName}
AllowNoIcons=yes
AlwaysShowDirOnReadyPage=yes
AlwaysShowGroupOnReadyPage=yes
DisableWelcomePage=no
OutputDir=.\
OutputBaseFilename={#AppName}.{#AppVersion}
Compression=lzma2/ultra
SolidCompression=yes
PrivilegesRequired=admin
AlwaysRestart=no
UninstallRestartComputer=no
MinVersion=5.01
SetupMutex=Global\B2D8B39A-94D2-434B-AA18-3524C219FCFE

ArchitecturesInstallIn64BitMode=x64 ia64

[Languages]
Name: "chinese"; MessagesFile: "ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone

[Components]
Name: main; Description:"主程序(必选)";Types:full compact custom;Flags: fixed
;Name: IE8;Description:"支持IE8浏览器";Types:full
Name: help;Description:"帮助文件";Types:full

[Files]
;Source: "..\build\Win32\bin\Register.bat"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
;Source: "..\build\Win32\bin\UnRegister.bat"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
;Source: "..\build\Win32\bin\npWebSocketPlugin.dll"; DestDir: "{app}\x86"; Components:IE8; Flags: regserver replacesameversion restartreplace uninsrestartdelete 32bit 
Source: "..\build\Win32\bin\UCClient.exe"; DestDir: "{app}\x86"; Components:main; Flags: replacesameversion restartreplace uninsrestartdelete 32bit  
Source: "..\build\Win32\bin\libx264-148.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\Ecmedia.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\libx278.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\libx279.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
;Source: "..\build\Win32\bin\ring.wav"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit 
;Source: "..\build\Win32\bin\ringback.wav"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\DuiLib.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\default.zip"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\assets\*"; DestDir: "{app}\x86\assets"; Components:main; Flags: recursesubdirs ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\*.js"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\*.html"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]Name: "{group}\Demo"; Filename: "{app}\x86\Demo.html"
Name: "{group}\UCClient"; Filename: "{app}\x86\UCClient.exe"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}"; Filename:"{app}\x86\UCClient.exe"; WorkingDir: "{app}"Name: "{commondesktop}\Demo"; Filename: "{app}\x86\Demo.html"; Tasks: desktopicon
;Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\CloopenClientPlugin"; Filename: "{app}\ClientPlugin.htm"; Tasks: quicklaunchicon
[Run]
;Filename: "net.exe"; Parameters: "start UCClient" 
;Filename: "net.exe"; Parameters: "start UCClientDaemon"Filename: "{app}\x86\UCClient.exe"; Description: "{cm:LaunchProgram,UCClient}"; Flags: nowait postinstall skipifsilent

[UninstallRun]
;Filename: "net.exe"; Parameters: "stop UCClient"
;Filename:"{app}\x86\UCClientService.exe"; Parameters:"-uninstall"; Flags:skipifdoesntexist
;Filename:"{app}\x86\Daemon.exe"; Parameters:"-uninstall"; Flags:skipifdoesntexist

[UninstallDelete]
Type:filesandordirs;Name:{app};
Type:dirifempty;Name:{pf32}\ytx;
Type:dirifempty;Name:{group};

[Registry] 
Root: HKLM32; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: {#AppName}; ValueData: "{app}\x86\UCClient.exe"; Flags: uninsdeletekeyRoot: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\UCClient"; ValueType: dword; ValueName:"WebSocketPort"; ValueData: 19998; Flags: createvalueifdoesntexist
Root: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\UCClient"; ValueType: expandsz; ValueName:"LogPath"; ValueData:{code:getLogDir}; Flags: createvalueifdoesntexist
Root: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\UCClient"; ValueType: dword; ValueName:"SpeakerVolume"; ValueData:100; Flags: createvalueifdoesntexist
Root: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\UCClient"; ValueType: dword; ValueName:"MicroVolume"; ValueData:100; Flags: createvalueifdoesntexist
Root: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\UCClient"; ValueType: dword; ValueName:"LogLevel"; ValueData:20000; Flags: createvalueifdoesntexist

[code]var selectLogDirPage:TwizardPage;
var selectDirBtn:TButton;
var logPath:TEdit;
var logLabel:TLabel;


procedure ChoseLogFoldersClick(Sender: TObject);
var choicedDIR:String;
begin
choicedDIR:=logPath.Text;

if BrowseForFolder('请选择日志目录',choicedDIR, True) then
  logPath.Text:=choicedDIR;
end; 

procedure InitializeWizard();
begin
  selectLogDirPage:=CreateCustomPage(wpSelectDir, '日志目录', '将日志记录到何处');

  logLabel:=TLabel.Create(selectLogDirPage);
  logLabel.Parent := selectLogDirPage.Surface;
  //logLabel.AutoSize:=True;
  //logLabel.WordWrap:=True;
  logLabel.Caption := '选择日志文件目录，请不要选择系统目录或者程序的安装目录。';

  logPath:=TEdit.Create(selectLogDirPage);
  logPath.Parent:=selectLogDirPage.Surface;
  logPath.Width:=logPath.Width*3;
  logPath.Text:=ExpandConstant('{userappdata}')+'\ytx\UCClient';
  logPath.Top:=logLabel.Top+logLabel.Height+12;

  selectDirBtn:=TButton.Create(nil);
  selectDirBtn.Parent:=selectLogDirPage.Surface;
  selectDirBtn.Caption:='浏览(&R)...';
  selectDirBtn.OnClick:= @ChoseLogFoldersClick; 
  selectDirBtn.Left:= logPath.Left + logPath.Width + 10;
  selectDirBtn.Top:=logPath.Top;
  selectDirBtn.Width:=100;
end;


function getLogDir(Param: String):String;
begin
  Result := logPath.Text;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin

  if CurUninstallStep = usUninstall then

    if MsgBox('您是否要删除配置信息？', mbConfirmation, MB_YESNO) = IDYES then
    begin
      RegDeleteKeyIncludingSubkeys(HKEY_CURRENT_USER, 'SOFTWARE\ytx\UCClient');
      RegDeleteKeyIncludingSubkeys(HKEY_USERS, '.DEFAULT\SOFTWARE\ytx\UCClient');
    end

end;

