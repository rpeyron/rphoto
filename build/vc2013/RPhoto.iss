; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=RPhoto
AppVerName=RPhoto 0.4.3
AppPublisherURL=http://www.via.ecp.fr/~remi/soft/rphoto/
AppSupportURL=http://www.via.ecp.fr/~remi/soft/rphoto/
AppUpdatesURL=http://www.via.ecp.fr/~remi/soft/rphoto/
DefaultDirName={pf}\RPhoto
DefaultGroupName=RPhoto
AllowNoIcons=true
DisableProgramGroupPage=yes
OutputBaseFilename=rphoto_setup
OutputDir=..\..\distrib\
ShowLanguageDialog=yes

[Files]
Source: ..\..\bin\RPhoto.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\..\bin\jpegtran.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\..\bin\jhead.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\..\bin\rphoto.htb; DestDir: {app}; Flags: ignoreversion
Source: ..\..\bin\fr\RPhoto.mo; DestDir: {app}/fr; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: {userprograms}\RPhoto; Filename: {app}\RPhoto.exe; WorkingDir: {app}

Name: {sendto}\RPhoto; Filename: {app}\RPhoto.exe
[Run]
; NOTE: The following entry contains an English phrase ("Launch"). You are free to translate it into another language if required.
Filename: {app}\RPhoto.exe; Description: Launch RPhoto; Flags: nowait postinstall skipifsilent
[_ISTool]
Use7zip=false
[Registry]
Root: HKCU; Subkey: software/RPhoto; ValueType: string; ValueName: version; ValueData: -; Flags: uninsdeletekey
