: ************************************
: *** Win32 �t�@�C���̃A�[�J�C�u�� ***
: ************************************

: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat

@set dist_dir=%CD%

:----- �t�@�C���̃R�s�[ -----
@echo �� �t�@�C�������W���܂�
copy ..\src\MZ3\Release_Win32\MZ4.exe    MZ3_win32\
copy ..\src\MZ3\Release_Win32_c\MZ4.exe  MZ3_win32\MZ4_c.exe
copy ..\src\JRegexDLL\Release\jregex.dll MZ3_win32\
rmdir /Q /S MZ3_win32\scripts
mkdir MZ3_win32\scripts
mkdir MZ3_win32\scripts\lib
mkdir MZ3_win32\scripts\mixi
copy ..\src\MZ3\scripts\*                MZ3_win32\scripts\
del MZ3_win32\scripts\_old.*
copy ..\src\MZ3\scripts\lib\*            MZ3_win32\scripts\lib\
copy ..\src\MZ3\scripts\mixi\*           MZ3_win32\scripts\mixi\
copy ..\src\MZ3\user_scripts\*           MZ3_win32\user_scripts\
copy ..\doc\Readme.txt                   MZ3_win32\
copy ..\ChangeLog.txt                    MZ3_win32\
copy toppage_group_sample.ini            MZ3_win32\

:----- �t�@�C���̈��k -----
@set archive=MZ3_win32.zip
@echo �� MZ3 �t�@�C�������k���܂�
@cd MZ3_win32
@del %archive%
@%_7Z_EXE% a -tzip -mx=9 -r %archive% *

:----- �A�[�J�C�u�t�@�C���̈ړ� -----
@echo �� �A�[�J�C�u�t�@�C�����ړ����܂�
@set archive_to=..\MZ4.%MZ3_VERSION%.zip
@echo %archive% �� %archive_to%
@move %archive% %archive_to%
@cd %dist_dir%
:pause
