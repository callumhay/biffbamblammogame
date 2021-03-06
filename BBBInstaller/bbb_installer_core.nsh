
Name "Biff! Bam!! Blammo!?!"

RequestExecutionLevel admin

Function create_desktop_shortcut_action
SetOutPath $INSTDIR
CreateShortcut "$DESKTOP\Biff Bam Blammo.lnk" "$INSTDIR\BiffBamBlammo.exe" "${BBB_SHORTCUT_PARAMETERS}"
FunctionEnd
# General Symbol Definitions
!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 1.0
!define COMPANY "Callum Hay"
!define URL ""

# MUI Symbol Definitions
!define MUI_ICON "..\BiffBamBlammo_Install.ico"

!define MUI_LICENSEPAGE_CHECKBOX
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Biff Bam Blammo"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN "$INSTDIR\BiffBamBlammo.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Run $(^Name)"
!define MUI_FINISHPAGE_SHOWREADME ""
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION create_desktop_shortcut_action
!define MUI_UNICON "..\BiffBamBlammo_Uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI.nsh
!include Library.nsh

# Reserved Files
ReserveFile "${NSISDIR}\Plugins\AdvSplash.dll"

# Variables
Var StartMenuGroup
Var LibInstall

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE ../licenses/bbb_license.txt
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile "${BBB_INSTALLER_EXE}"
InstallDir $PROGRAMFILES\BiffBamBlammo
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion 1.0.0.0
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductName "Biff! Bam!! Blammo!?!"
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} CompanyName "${COMPANY}"
VIAddVersionKey /LANG=${LANG_ENGLISH} FileVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} FileDescription ""
VIAddVersionKey /LANG=${LANG_ENGLISH} LegalCopyright "Copyright (c) 2014"
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show

# Installer sections

# Prerequisite installations:
# - C++ Redistributable
#Section -Prerequisites
#    SetOutPath $INSTDIR
  
#    IfErrors continue
#    continue:
#    File vcredist_x86.exe
#    ExecWait '"$INSTDIR\vcredist_x86.exe"'

#    Delete "$INSTDIR\vcredist_x86.exe"
#SectionEnd


# Installation of BBB section
Section "Biff! Bam!! Blammo!?!" SEC0000
    SetOutPath $INSTDIR
    SetOverwrite on
    File ..\lib\cg.dll
    File ..\lib\cgGL.dll
    File ..\lib\DevIL.dll
    File ..\lib\ILU.dll
    File ..\lib\ILUT.dll
    File ..\lib\SDL.dll
    File ..\lib\SDL_mixer.dll
    File ..\lib\XInput9_1_0.dll
    File "..\lib\${BBB_MSVCR_FILE}"
    File "..\lib\${BBB_MSVCP_FILE}"
    File ..\BBBResources.zip
    File ..\BiffBamBlammoIcon.bmp
    File "..\BiffBamBlammo.ico"
    File /r "..\licenses"
    File "${BBB_EXE_DIR}\BiffBamBlammo.exe"
    WriteRegStr HKLM "${REGKEY}\Components" "Biff! Bam!! Blammo!?! Install" 1
SectionEnd

Section -post SEC0002
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\bbb_uninstaller.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $INSTDIR
	CreateDirectory $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk" $INSTDIR\bbb_uninstaller.exe
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^ExeLink).lnk" "$INSTDIR\BiffBamBlammo.exe"
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\bbb_uninstaller.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\bbb_uninstaller.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

Section /o "-un.Biff! Bam!! Blammo!?! Install" UNSEC0000
    Delete /REBOOTOK $INSTDIR\BiffBamBlammo.ico
    Delete /REBOOTOK $INSTDIR\BiffBamBlammo.exe
    Delete /REBOOTOK $INSTDIR\BBBResources.zip
    Delete /REBOOTOK $INSTDIR\SDL_mixer.dll
    Delete /REBOOTOK $INSTDIR\SDL.dll
    Delete /REBOOTOK $INSTDIR\ILUT.dll
    Delete /REBOOTOK $INSTDIR\ILU.dll
    Delete /REBOOTOK $INSTDIR\DevIL.dll
    Delete /REBOOTOK $INSTDIR\cgGL.dll
    Delete /REBOOTOK $INSTDIR\cg.dll
    Delete /REBOOTOK $INSTDIR\XInput9_1_0.dll
    Delete /REBOOTOK "$INSTDIR\${BBB_MSVCR_FILE}"
    Delete /REBOOTOK "$INSTDIR\${BBB_MSVCP_FILE}"
    RMDir /r /REBOOTOK "$INSTDIR\licenses"

    MessageBox MB_YESNO "Would you like to keep all game progress and configuration files? (Warning: By clicking 'No' you will delete all game progress you've made in Biff! Bam!! Blammo!?!)." IDNO delete_progress IDYES keep_progress
    delete_progress:
    Delete /REBOOTOK $INSTDIR\BiffBamBlammo.ini
    Delete /REBOOTOK $INSTDIR\bbb_progress.dat
    Delete /REBOOTOK $INSTDIR\blammopedia.dat
    keep_progress:

    Delete /REBOOTOK $INSTDIR\BiffBamBlammoIcon.bmp
    DeleteRegValue HKLM "${REGKEY}\Components" "Biff! Bam!! Blammo!?! Install"
SectionEnd

Section -un.post UNSEC0002
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^ExeLink).lnk"
    Delete /REBOOTOK "$DESKTOP\Biff Bam Blammo.lnk"
    Delete /REBOOTOK $INSTDIR\bbb_uninstaller.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RMDir /r /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RMDir /REBOOTOK $INSTDIR
    Push $R0
    StrCpy $R0 $StartMenuGroup 1
    StrCmp $R0 ">" no_smgroup
no_smgroup:
    Pop $R0
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
    Push $R1
    File /oname=$PLUGINSDIR\spltmp.bmp BBB_Logo.bmp
    advsplash::show 1100 600 500 -1 $PLUGINSDIR\spltmp
    Pop $R1
    Pop $R1
    Push $0
    ReadRegStr $0 HKLM "${REGKEY}" Path
    ClearErrors
    StrCmp $0 "" +2
    StrCpy $LibInstall 1
    Pop $0
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION "Biff! Bam!! Blammo!?! Install" ${UNSEC0000}
FunctionEnd

# Installer Language Strings
# TODO Update the Language Strings with the appropriate translations.

LangString ^UninstallLink ${LANG_ENGLISH} "Uninstall Biff Bam Blammo"
LangString ^ExeLink ${LANG_ENGLISH} "Biff Bam Blammo"