@ECHO OFF

SET VCXPROJ=%1%

SET REGEX="<SourceProperty1.*Debug.*>SourceProperty1Value<\/SourceProperty1>"
FINDSTR %REGEX% %VCXPROJ%
if %ERRORLEVEL% NEQ 0 goto ERROR

SET REGEX="<TargetProperty1.*Debug.*>TargetProperty1ValueDebug<\/TargetProperty1>"
FINDSTR %REGEX% %VCXPROJ%
if %ERRORLEVEL% NEQ 0 goto ERROR

SET REGEX="<TargetProperty1.*Release.*>TargetProperty1ValueRelease<\/TargetProperty1>"
FINDSTR %REGEX% %VCXPROJ%
if %ERRORLEVEL% NEQ 0 goto ERROR

ECHO Success: Properties were set correctly
exit /b 0

:ERROR
ECHO Error: Properties not set correctly
exit /b %ERRORLEVEL%
