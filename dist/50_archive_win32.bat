: ************************************
: *** Win32 �t�@�C���̃A�[�J�C�u�� ***
: ************************************

: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat

@set dist_dir=%CD%

:----- �t�@�C���̃R�s�[ -----
@echo �� �t�@�C�������W���܂�
copy ..\src\MZ3\Release_Win32\MZ3_win32.exe MZ3_win32\
copy "..\doc\Help Files\MZ3 for Windows\MZ3 for Windows.chm" MZ3_win32\
copy ..\doc\Readme.txt MZ3_win32\
copy ..\ChangeLog.txt MZ3_win32\
copy toppage_group_sample.ini MZ3_win32\

:----- �t�@�C���̈��k -----
@set archive=MZ3_win32.lzh
@echo �� MZ3 �t�@�C�������k���܂�
@cd MZ3_win32
@del %archive%
%UNLHA_EXE% @..\response_win32.txt

:----- �A�[�J�C�u�t�@�C���̈ړ� -----
@echo �� �A�[�J�C�u�t�@�C�����ړ����܂�
@set archive_to=..\MZ3.%MZ3_VERSION%.win32.lzh
@echo %archive% �� %archive_to%
@move %archive% %archive_to%
@cd %dist_dir%
pause