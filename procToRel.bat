@echo off
title CopyShaderFiles

rem �t�@�C����, �t�H���_��
set v32=..\32bit-Win
set v64=..\64bit-Win
set exeNm=on4d_GLr3.exe
set newExeNm=s3sim.exe

cd on4d_GLr2\
rem �I�u�W�F�N�g�t�H���_�R�s�[
echo �I�u�W�F�N�g�t�H���_�R�s�[�J�n..
if exist "%v32%\obj" rmdir /s /q "%v32%\obj"
if exist "%v64%\obj" rmdir /s /q "%v64%\obj"
xcopy /i "obj" "%v32%\obj"
xcopy /i "obj" "%v64%\obj"

rem �V�F�[�_�t�@�C���R�s�[
echo �V�F�[�_�t�@�C���R�s�[�R�s�[�J�n..
for %%A in ( *.c ) do ( 
	copy /y "%%A" "%v32%\%%A" 
	copy /y "%%A" "%v64%\%%A" 
)

rem .exe�R�s�[
echo exe�t�@�C���R�s�[�J�n..
if exist "..\Release\%exeNm%" (copy /y "..\Release\%exeNm%" "%v32%\%newExeNm%") else (echo 32bit�ł�exe�͑��݂��Ȃ��̂Ŗ��R�s�[)
if exist "..\x64\Release\%exeNm%" (copy /y "..\x64\Release\%exeNm%" "%v64%\%newExeNm%") else (echo 64bit�ł�exe�͑��݂��Ȃ��̂Ŗ��R�s�[)

rem ���ԃt�@�C�������폜
echo ���ԃt�@�C�������폜
rmdir /s /q Debug
rmdir /s /q Release
rmdir /s /q x64

echo -�����I��-
pause >nul