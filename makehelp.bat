@echo off
echo Building RTF file
xsltproc -o help/kinsole.rtf shared-code/kinhelp.xsl help/kinsole.xml
echo Building Win32 Help files
start /wait hcrtf -x "help\kinsole.hpj"
echo.
if exist Debug\nul copy "help\kinsole.hlp" Debug
if exist Debug\nul copy "help\kinsole.cnt" Debug
if exist Release\nul copy "help\kinsole.hlp" Release
if exist Release\nul copy "help\kinsole.cnt" Release
echo.
