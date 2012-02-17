# Microsoft Developer Studio Generated NMAKE File, Based on Mandlebrot.dsp
!IF "$(CFG)" == ""
CFG=Mandlebrot - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Mandlebrot - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Mandlebrot - Win32 Release" && "$(CFG)" != "Mandlebrot - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mandlebrot.mak" CFG="Mandlebrot - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mandlebrot - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Mandlebrot - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Mandlebrot - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Mandlebrot.exe"


CLEAN :
	-@erase "$(INTDIR)\mandlebrot.obj"
	-@erase "$(INTDIR)\Mandlebrot.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Mandlebrot.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\Mandlebrot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Mandlebrot.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mandlebrot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Mandlebrot.pdb" /machine:I386 /out:"$(OUTDIR)\Mandlebrot.exe" 
LINK32_OBJS= \
	"$(INTDIR)\mandlebrot.obj" \
	"$(INTDIR)\Mandlebrot.res"

"$(OUTDIR)\Mandlebrot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Mandlebrot - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Mandlebrot.exe" "$(OUTDIR)\Mandlebrot.bsc"


CLEAN :
	-@erase "$(INTDIR)\mandlebrot.obj"
	-@erase "$(INTDIR)\Mandlebrot.res"
	-@erase "$(INTDIR)\mandlebrot.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Mandlebrot.bsc"
	-@erase "$(OUTDIR)\Mandlebrot.exe"
	-@erase "$(OUTDIR)\Mandlebrot.ilk"
	-@erase "$(OUTDIR)\Mandlebrot.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Mandlebrot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Mandlebrot.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mandlebrot.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\mandlebrot.sbr"

"$(OUTDIR)\Mandlebrot.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Mandlebrot.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Mandlebrot.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\mandlebrot.obj" \
	"$(INTDIR)\Mandlebrot.res"

"$(OUTDIR)\Mandlebrot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Mandlebrot.dep")
!INCLUDE "Mandlebrot.dep"
!ELSE 
!MESSAGE Warning: cannot find "Mandlebrot.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Mandlebrot - Win32 Release" || "$(CFG)" == "Mandlebrot - Win32 Debug"
SOURCE=.\mandlebrot.cpp

!IF  "$(CFG)" == "Mandlebrot - Win32 Release"


"$(INTDIR)\mandlebrot.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Mandlebrot - Win32 Debug"


"$(INTDIR)\mandlebrot.obj"	"$(INTDIR)\mandlebrot.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Mandlebrot.rc

"$(INTDIR)\Mandlebrot.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

