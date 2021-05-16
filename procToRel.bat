@echo off
title CopyShaderFiles

rem ファイル名, フォルダ名
set v32=..\32bit-Win
set v64=..\64bit-Win
set exeNm=nes-fs.exe
set newExeNm=nes-fs.exe

cd on4d_GLr2\
rem オブジェクトフォルダコピー
echo 3dオブジェクトフォルダコピー開始..
if exist "%v32%\obj" rmdir /s /q "%v32%\obj"
if exist "%v64%\obj" rmdir /s /q "%v64%\obj"
xcopy /i "obj" "%v32%\obj"
xcopy /i "obj" "%v64%\obj"
rem スクリプトフォルダコピー
echo スクリプトフォルダコピー開始..
if exist "%v32%\sample_script" rmdir /s /q "%v32%\sample_script"
if exist "%v64%\sample_script" rmdir /s /q "%v64%\sample_script"
xcopy /i "sample_script" "%v32%\sample_script"
xcopy /i "sample_script" "%v64%\sample_script"
echo シェーダーフォルダコピー開始..
if exist "%v32%\shader" rmdir /s /q "%v32%\shader"
if exist "%v64%\shader" rmdir /s /q "%v64%\shader"
xcopy /i "shader" "%v32%\shader"
xcopy /i "shader" "%v64%\shader"

echo pyファイルコピー開始..
for %%A in ( *.py ) do ( 
	copy /y "%%A" "%v32%\%%A" 
	copy /y "%%A" "%v64%\%%A" 
)

rem .exeコピー
echo exeファイルコピー開始..
if exist "..\Release\%exeNm%" (copy /y "..\Release\%exeNm%" "%v32%\%newExeNm%") else (echo 32bit版のexeは存在しないので未コピー)
if exist "..\x64\Release\%exeNm%" (copy /y "..\x64\Release\%exeNm%" "%v64%\%newExeNm%") else (echo 64bit版のexeは存在しないので未コピー)

rem 中間ファイル等を削除
REM echo 中間ファイル, 結果ファイル, vsファイル等を削除
REM rmdir /s /q Debug
REM rmdir /s /q Release
REM rmdir /s /q x64
cd ..
REM rmdir /s /q Debug
REM rmdir /s /q Release
REM rmdir /s /q x64
REM rmdir /s /q .vs
REM rmdir /s /q on4d_GLr3.sdf


echo -処理終了-
pause >nul