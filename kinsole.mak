# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Install - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Install - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "KINSole - Win32 Release" && "$(CFG)" !=\
 "KINSole - Win32 Debug" && "$(CFG)" != "Install - Win32 Release" && "$(CFG)" !=\
 "Install - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "kinsole.mak" CFG="Install - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KINSole - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "KINSole - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Install - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Install - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Install - Win32 Debug"

!IF  "$(CFG)" == "KINSole - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
TargetName=kinsole
# End Custom Macros

ALL : "$(OUTDIR)\kinsole.exe" "$(OUTDIR)\kinsole.ex_" "$(OUTDIR)\kinsole.hlp"\
 "$(OUTDIR)\kinsole.cnt" "$(OUTDIR)\kinsole.hl_" "$(OUTDIR)\kinsole.cn_"

CLEAN : 
	-@erase "$(INTDIR)\kinsole.cn_"
	-@erase "$(INTDIR)\kinsole.cnt"
	-@erase "$(INTDIR)\kinsole.hl_"
	-@erase "$(INTDIR)\kinsole.hlp"
	-@erase "$(INTDIR)\KINSole.obj"
	-@erase "$(INTDIR)\KINSole.res"
	-@erase "$(OUTDIR)\kinsole.ex_"
	-@erase "$(OUTDIR)\kinsole.exe"
	-@erase "$(OUTDIR)\kinsole.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /ML /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/KINSole.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/kinsole.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /map /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/kinsole.pdb" /map:"$(INTDIR)/kinsole.map" /machine:I386\
 /out:"$(OUTDIR)/kinsole.exe" 
LINK32_OBJS= \
	"$(INTDIR)\KINSole.obj" \
	"$(INTDIR)\KINSole.res"

"$(OUTDIR)\kinsole.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Performing Custom Build Step
OutDir=.\Release
TargetName=kinsole
InputPath=.\Release\kinsole.exe
SOURCE=$(InputPath)

"$(OutDir)\$(TargetName).ex_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   compress $(OutDir)\$(TargetName).exe $(OutDir)\$(TargetName).ex_

# End Custom Build

!ELSEIF  "$(CFG)" == "KINSole - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
TargetName=kinsole
# End Custom Macros

ALL : "$(OUTDIR)\kinsole.exe" "$(OUTDIR)\kinsole.ex_" "$(OUTDIR)\kinsole.hlp"\
 "$(OUTDIR)\kinsole.cnt" "$(OUTDIR)\kinsole.hl_" "$(OUTDIR)\kinsole.cn_"

CLEAN : 
	-@erase "$(INTDIR)\kinsole.cn_"
	-@erase "$(INTDIR)\kinsole.cnt"
	-@erase "$(INTDIR)\kinsole.hl_"
	-@erase "$(INTDIR)\kinsole.hlp"
	-@erase "$(INTDIR)\KINSole.obj"
	-@erase "$(INTDIR)\KINSole.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\kinsole.ex_"
	-@erase "$(OUTDIR)\kinsole.exe"
	-@erase "$(OUTDIR)\kinsole.ilk"
	-@erase "$(OUTDIR)\kinsole.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_AFXDLL" /D "_MBCS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/KINSole.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/kinsole.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=wsock32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/kinsole.pdb" /debug /machine:I386 /out:"$(OUTDIR)/kinsole.exe" 
LINK32_OBJS= \
	"$(INTDIR)\KINSole.obj" \
	"$(INTDIR)\KINSole.res"

"$(OUTDIR)\kinsole.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build
OutDir=.\Debug
TargetName=kinsole
InputPath=.\Debug\kinsole.exe
SOURCE=$(InputPath)

"$(OutDir)\$(TargetName).ex_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   compress $(OutDir)\$(TargetName).exe $(OutDir)\$(TargetName).ex_

# End Custom Build

!ELSEIF  "$(CFG)" == "Install - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Install\Release"
# PROP BASE Intermediate_Dir "Install\Release"
# PROP BASE Target_Dir "Install"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Install\Release"
# PROP Intermediate_Dir "Install\Release"
# PROP Target_Dir "Install"
OUTDIR=.\Install\Release
INTDIR=.\Install\Release

ALL : "KINSole - Win32 Release" "$(OUTDIR)\Install.exe"

CLEAN : 
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\Install.res"
	-@erase "$(OUTDIR)\Install.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/Install.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Install\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Install.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Install.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Install.pdb" /machine:I386 /out:"$(OUTDIR)/Install.exe" 
LINK32_OBJS= \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\Install.res"

"$(OUTDIR)\Install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Install - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Install\Debug"
# PROP BASE Intermediate_Dir "Install\Debug"
# PROP BASE Target_Dir "Install"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Install\Debug"
# PROP Intermediate_Dir "Install\Debug"
# PROP Target_Dir "Install"
OUTDIR=.\Install\Debug
INTDIR=.\Install\Debug

ALL : "KINSole - Win32 Debug" "$(OUTDIR)\Install.exe"

CLEAN : 
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\Install.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Install.exe"
	-@erase "$(OUTDIR)\Install.ilk"
	-@erase "$(OUTDIR)\Install.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/Install.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Install\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Install.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Install.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Install.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Install.exe" 
LINK32_OBJS= \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\Install.res"

"$(OUTDIR)\Install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "KINSole - Win32 Release"
# Name "KINSole - Win32 Debug"

!IF  "$(CFG)" == "KINSole - Win32 Release"

!ELSEIF  "$(CFG)" == "KINSole - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\KINSole.cpp
DEP_CPP_KINSO=\
	".\NAWS.cpp"\
	".\NEW-ENVIRON.cpp"\
	".\options.cpp"\
	".\status.cpp"\
	".\terminal.cpp"\
	".\TIMING-MARK.cpp"\
	".\TTY.cpp"\
	".\VT100.cpp"\
	

"$(INTDIR)\KINSole.obj" : $(SOURCE) $(DEP_CPP_KINSO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\kinsole.rc
DEP_RSC_KINSOL=\
	".\res\kinsole.ico"\
	

"$(INTDIR)\KINSole.res" : $(SOURCE) $(DEP_RSC_KINSOL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\VT100.cpp
# PROP Exclude_From_Build 1
# End Source File
################################################################################
# Begin Source File

SOURCE=".\NEW-ENVIRON.cpp"
# PROP Exclude_From_Build 1
# End Source File
################################################################################
# Begin Source File

SOURCE=.\help\kinsole.hpj

!IF  "$(CFG)" == "KINSole - Win32 Release"

# Begin Custom Build - Building Help
OutDir=.\Release
TargetName=kinsole
InputPath=.\help\kinsole.hpj

BuildCmds= \
	makehelp \
	compress $(OutDir)\$(TargetName).hlp $(OutDir)\$(TargetName).hl_ \
	compress $(OutDir)\$(TargetName).cnt $(OutDir)\$(TargetName).cn_ \
	

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).hl_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cn_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "KINSole - Win32 Debug"

# Begin Custom Build - Building Help
OutDir=.\Debug
TargetName=kinsole
InputPath=.\help\kinsole.hpj

BuildCmds= \
	makehelp \
	compress $(OutDir)\$(TargetName).hlp $(OutDir)\$(TargetName).hl_ \
	compress $(OutDir)\$(TargetName).cnt $(OutDir)\$(TargetName).cn_ \
	

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).hl_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\$(TargetName).cn_" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Install - Win32 Release"
# Name "Install - Win32 Debug"

!IF  "$(CFG)" == "Install - Win32 Release"

!ELSEIF  "$(CFG)" == "Install - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Install\Install.rc
DEP_RSC_INSTA=\
	".\Install\Custom.rch"\
	".\shared-data\install-icon.ico"\
	

!IF  "$(CFG)" == "Install - Win32 Release"


"$(INTDIR)\Install.res" : $(SOURCE) $(DEP_RSC_INSTA) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Install.res" /i "Install" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Install - Win32 Debug"


"$(INTDIR)\Install.res" : $(SOURCE) $(DEP_RSC_INSTA) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/Install.res" /i "Install" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Install\install.cpp
DEP_CPP_INSTAL=\
	".\shared-code\install.h"\
	

"$(INTDIR)\install.obj" : $(SOURCE) $(DEP_CPP_INSTAL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "KINSole"

!IF  "$(CFG)" == "Install - Win32 Release"

"KINSole - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\kinsole.mak" CFG="KINSole - Win32 Release" 

!ELSEIF  "$(CFG)" == "Install - Win32 Debug"

"KINSole - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\kinsole.mak" CFG="KINSole - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\help\kinsole.hpj

!IF  "$(CFG)" == "Install - Win32 Release"

!ELSEIF  "$(CFG)" == "Install - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
