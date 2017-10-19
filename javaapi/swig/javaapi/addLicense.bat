:: Adds license to every java file
:: Param1: license file
:: Param2: java folder
::addLicense.bat ..\LICENSE ..\ilwisjava\src\main\java\org\n52\ilwis\java\

@echo off
set "header=%1"
set "folder=%2"
set "tempFile=%folder%\temp.txt"
for %%F in ("%folder%\*.java") do (
  findstr /M /OFF /C:"GNU General Public License" %%F
  if errorlevel 1 (
    echo /* > "%tempFile%"
    type "%header%" >>"%tempFile%"
	echo. >>"%tempFile%"
    echo */ >> "%tempFile%"
    type "%%F" >>"%tempFile%"
    move /y "%tempFile%" "%%F" >nul
  ) 
)