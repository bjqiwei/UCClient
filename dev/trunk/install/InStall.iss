; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

[Setup]
; ע��: AppId ֵ����Ψһʶ���Ӧ�ó���
; ��ֹ������Ӧ�ó���İ�װ��ʹ����ͬ�� AppId ֵ��
; (��Ҫ����һ���µ� GUID����ѡ�񡰹��� | ���� GUID����)
#define AppName "WebCall"
#define AppVersion "4.0.1.16"
AppId={{3E59331C-D6E2-4475-8678-467C583ADE01}}
AppName={#AppName}
AppVersion={#AppVersion}
AppCopyright=Copyright (C)ytx Inc.
AppComments=ytx
VersionInfoVersion={#AppVersion}
VersionInfoDescription={#AppName}��װ��
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
SetupMutex=Global\04B48F80-4AE4-4C90-AE5C-63BDB81110CE

ArchitecturesInstallIn64BitMode=x64 ia64

[Languages]
Name: "chinese"; MessagesFile: "ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone

[Components]
Name: main; Description:"������(��ѡ)";Types:full compact custom;Flags: fixed
Name: IE8;Description:"֧��IE8�����";Types:full
Name: help;Description:"�����ļ�";Types:full

[Files]
Source: "..\build\Win32\bin\Register.bat"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\UnRegister.bat"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\npBrowserPlugin.dll"; DestDir: "{app}\x86"; Components:main; Flags: regserver replacesameversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\npWebSocketPlugin.dll"; DestDir: "{app}\x86"; Components:IE8; Flags: regserver replacesameversion restartreplace uninsrestartdelete 32bit 
;Source: "..\build\Win32\bin\WebCallService.exe"; DestDir: "{app}\x86"; Components:main; Flags: replacesameversion restartreplace uninsrestartdelete 32bit
;Source: "..\build\Win32\bin\Daemon.exe"; DestDir: "{app}\x86"; Components:main; Flags: replacesameversion restartreplace uninsrestartdelete 32bit
;Source: "..\build\Win32\bin\WebCallCtrl.exe"; DestDir: "{app}\x86"; Components:main; Flags: replacesameversion restartreplace uninsrestartdelete 32bit  
Source: "..\build\Win32\bin\WebCall.exe"; DestDir: "{app}\x86"; Components:main; Flags: replacesameversion restartreplace uninsrestartdelete 32bit  ;Source: "..\build\Win32\bin\libx264-148.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit;Source: "..\build\Win32\bin\Ecmedia.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\libx278.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\libx279.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\ring.wav"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit 
;Source: "..\build\Win32\bin\ringback.wav"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: "..\build\Win32\bin\DuiLib.dll"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
Source: ".\default.zip"; DestDir: "{app}\x86"; Components:main; Flags: ignoreversion restartreplace uninsrestartdelete 32bit
; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��

[Icons]
;Name: "{group}\WebCall"; Filename: "{app}\x86\WebCallCtrl.exe"
Name: "{group}\WebCall"; Filename: "{app}\x86\WebCall.exe"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
;Name: "{commondesktop}\{#AppName}"; Filename:"{app}\x86\WebCallCtrl.exe"; WorkingDir: "{app}"
Name: "{commondesktop}\{#AppName}"; Filename:"{app}\x86\WebCall.exe"; WorkingDir: "{app}"
;Name: "{commondesktop}\CloopenClientPlugin"; Filename: "{app}\ClientPlugin.htm"; Tasks: desktopicon
;Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\CloopenClientPlugin"; Filename: "{app}\ClientPlugin.htm"; Tasks: quicklaunchicon
[Run]
;Filename:"{app}\x86\WebCallService.exe"; Parameters:"-install"
;Filename:"{app}\x86\Daemon.exe"; Parameters:"-install" 
;Filename: "net.exe"; Parameters: "start WebCall" 
;Filename: "net.exe"; Parameters: "start WebCallDaemon"Filename: "{app}\x86\WebCall.exe"; Description: "{cm:LaunchProgram,WebCall}"; Flags: nowait postinstall skipifsilent

[UninstallRun]
;Filename: "net.exe"; Parameters: "stop WebCallDaemon"
;Filename: "net.exe"; Parameters: "stop WebCall"
;Filename:"{app}\x86\WebCallService.exe"; Parameters:"-uninstall"; Flags:skipifdoesntexist
;Filename:"{app}\x86\Daemon.exe"; Parameters:"-uninstall"; Flags:skipifdoesntexist

[UninstallDelete]
Type:filesandordirs;Name:{app};
Type:dirifempty;Name:{pf32}\ytx;
Type:dirifempty;Name:{group};

[Registry] 
Root: HKLM32; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: {#AppName}; ValueData: "{app}\x86\WebCall.exe"; Flags: uninsdeletekeyRoot: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\WebCall"; ValueType: dword; ValueName:"WebSocketPort"; ValueData: 19996; Flags: createvalueifdoesntexist
Root: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\WebCall"; ValueType: expandsz; ValueName:"LogPath"; ValueData:{code:getLogDir}; Flags: createvalueifdoesntexist
Root: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\WebCall"; ValueType: dword; ValueName:"SpeakerVolume"; ValueData:100; Flags: createvalueifdoesntexist
Root: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\WebCall"; ValueType: dword; ValueName:"MicroVolume"; ValueData:100; Flags: createvalueifdoesntexist
Root: HKU32; Subkey:".DEFAULT\SOFTWARE\ytx\WebCall"; ValueType: dword; ValueName:"LogLevel"; ValueData:20000; Flags: createvalueifdoesntexist

[code]var selectLogDirPage:TwizardPage;
var selectDirBtn:TButton;
var logPath:TEdit;
var logLabel:TLabel;


procedure ChoseLogFoldersClick(Sender: TObject);
var choicedDIR:String;
begin
choicedDIR:=logPath.Text;

if BrowseForFolder('��ѡ����־Ŀ¼',choicedDIR, True) then
  logPath.Text:=choicedDIR;
end; 

procedure InitializeWizard();
begin
  selectLogDirPage:=CreateCustomPage(wpSelectDir, '��־Ŀ¼', '����־��¼���δ�');

  logLabel:=TLabel.Create(selectLogDirPage);
  logLabel.Parent := selectLogDirPage.Surface;
  //logLabel.AutoSize:=True;
  //logLabel.WordWrap:=True;
  logLabel.Caption := 'ѡ����־�ļ�Ŀ¼���벻Ҫѡ��ϵͳĿ¼���߳���İ�װĿ¼��';

  logPath:=TEdit.Create(selectLogDirPage);
  logPath.Parent:=selectLogDirPage.Surface;
  logPath.Width:=logPath.Width*3;
  logPath.Text:=ExpandConstant('{userappdata}')+'\ytx\WebCall';
  logPath.Top:=logLabel.Top+logLabel.Height+12;

  selectDirBtn:=TButton.Create(nil);
  selectDirBtn.Parent:=selectLogDirPage.Surface;
  selectDirBtn.Caption:='���(&R)...';
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

    if MsgBox('���Ƿ�Ҫɾ��������Ϣ��', mbConfirmation, MB_YESNO) = IDYES then
    begin
      RegDeleteKeyIncludingSubkeys(HKEY_CURRENT_USER, 'SOFTWARE\ytx\WebCall');
      RegDeleteKeyIncludingSubkeys(HKEY_USERS, '.DEFAULT\SOFTWARE\ytx\WebCall');
    end

end;

