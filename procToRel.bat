@echo off
title CopyShaderFiles

rem �t�@�C����, �t�H���_��
set v32=..\32bit-Win
set v64=..\64bit-Win
set exeNm=nes-fs.exe
set newExeNm=nes-fs.exe

cd on4d_GLr2\
rem �I�u�W�F�N�g�t�H���_�R�s�[
echo 3d�I�u�W�F�N�g�t�H���_�R�s�[�J�n..
if exist "%v32%\obj" rmdir /s /q "%v32%\obj"
if exist "%v64%\obj" rmdir /s /q "%v64%\obj"
xcopy /i "obj" "%v32%\obj"
xcopy /i "obj" "%v64%\obj"
rem �X�N���v�g�t�H���_�R�s�[
echo �X�N���v�g�t�H���_�R�s�[�J�n..
if exist "%v32%\sample_script" rmdir /s /q "%v32%\sample_script"
if exist "%v64%\sample_script" rmdir /s /q "%v64%\sample_script"
xcopy /i "sample_script" "%v32%\sample_script"
xcopy /i "sample_script" "%v64%\sample_script"
echo �V�F�[�_�[�t�H���_�R�s�[�J�n..
if exist "%v32%\shader" rmdir /s /q "%v32%\shader"
if exist "%v64%\shader" rmdir /s /q "%v64%\shader"
xcopy /i "shader" "%v32%\shader"
xcopy /i "shader" "%v64%\shader"

echo py�t�@�C���R�s�[�J�n..
for %%A in ( *.py ) do ( 
	copy /y "%%A" "%v32%\%%A" 
	copy /y "%%A" "%v64%\%%A" 
)

rem .exe�R�s�[
echo exe�t�@�C���R�s�[�J�n..
if exist "..\Release\%exeNm%" (copy /y "..\Release\%exeNm%" "%v32%\%newExeNm%") else (echo 32bit�ł�exe�͑��݂��Ȃ��̂Ŗ��R�s�[)
if exist "..\x64\Release\%exeNm%" (copy /y "..\x64\Release\%exeNm%" "%v64%\%newExeNm%") else (echo 64bit�ł�exe�͑��݂��Ȃ��̂Ŗ��R�s�[)

rem ���ԃt�@�C�������폜
REM echo ���ԃt�@�C��, ���ʃt�@�C��, vs�t�@�C�������폜
REM rmdir /s /q Debug
REM rmdir /s /q Release
REM rmdir /s /q x64
cd ..
REM rmdir /s /q Debug
REM rmdir /s /q Release
REM rmdir /s /q x64
REM rmdir /s /q .vs
REM rmdir /s /q on4d_GLr3.sdf


echo -�����I��-
pause >nul