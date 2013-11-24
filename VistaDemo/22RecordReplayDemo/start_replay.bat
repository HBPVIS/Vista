@ECHO OFF
REM In this file, we will first start the slaves, and then the master

SET CURRENT_PATH=%PATH%

CALL set_path_for_22RecordReplayDemo.bat

CALL 22RecordReplayDemo.exe -replay testrecord

set PATH=%CURRENT_PATH%