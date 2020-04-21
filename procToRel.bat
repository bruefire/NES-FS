@echo off
title CopyShaderFiles

rem ファイル名, フォルダ名
set v32=..\32bit-Win
set v64=..\64bit-Win
set exeNm=on4d_GLr3.exe
set newExeNm=s3sim.exe

cd on4d_GLr2\
rem オブジェクトフォルダコピー
echo オブジェクトフォルダコピー開始..
if exist "%v32%\obj" rmdir /s /q "%v32%\obj"
if exist "%v64%\obj" rmdir /s /q "%v64%\obj"
xcopy /i "obj" "%v32%\obj"
xcopy /i "obj" "%v64%\obj"

rem シェーダファイルコピー
echo シェーダファイルコピーコピー開始..
for %%A in ( *.c ) do ( 
	copy /y "%%A" "%v32%\%%A" 
	copy /y "%%A" "%v64%\%%A" 
)

rem .exeコピー
echo exeファイルコピー開始..
if exist "..\Release\%exeNm%" (copy /y "..\Release\%exeNm%" "%v32%\%newExeNm%") else (echo 32bit版のexeは存在しないので未コピー)
if exist "..\x64\Release\%exeNm%" (copy /y "..\x64\Release\%exeNm%" "%v64%\%newExeNm%") else (echo 64bit版のexeは存在しないので未コピー)

rem 中間ファイル等を削除
echo 中間ファイル等を削除
rmdir /s /q Debug
rmdir /s /q Release
rmdir /s /q x64

echo -処理終了-
pause >nul