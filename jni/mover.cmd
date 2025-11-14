@echo off
set SRC=C:\Users\maria\Downloads\source-neon\jni\libs\armeabi-v7a\libSAMP.so
set DEST=C:\Users\maria\Downloads\source-neon\app\src\main\jniLibs\armeabi-v7a\

echo Copiando %SRC% para %DEST%
xcopy "%SRC%" "%DEST%" /Y /I

echo Concluido!
pause