: *************************************************
: *** TkTweets for Win32 �t�@�C���̃A�[�J�C�u�� ***
: *************************************************

: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat

@set dist_dir=%CD%

:----- �t�@�C���̃R�s�[ -----
@echo �� �t�@�C�������W���܂�
copy ..\src\MZ3\TkTw_R_W32\TkTweets.exe  TkTweets_win32\
copy ..\src\MZ3\TkTw_R_W32c\TkTweets.exe TkTweets_win32\TkTweets_c.exe
copy ..\src\JRegexDLL\Release\jregex.dll TkTweets_win32\
rmdir /Q /S TkTweets_win32\scripts
mkdir TkTweets_win32\scripts
mkdir TkTweets_win32\scripts\lib
copy ..\src\MZ3\scripts\*                TkTweets_win32\scripts\
del MZ3_win32\scripts\_old.*
copy ..\src\MZ3\scripts\lib\*            TkTweets_win32\scripts\lib
copy ..\src\MZ3\user_scripts\*           TkTweets_win32\user_scripts\
copy ..\doc\Readme.txt                   TkTweets_win32\
copy ..\ChangeLog.txt                    TkTweets_win32\

:----- �t�@�C���̈��k -----
@set archive=TkTweets_win32.zip
@echo �� TkTweets �t�@�C�������k���܂�
@cd TkTweets_win32
@del %archive%
@%_7Z_EXE% a -r -tzip %archive% *

:----- �A�[�J�C�u�t�@�C���̈ړ� -----
@echo �� �A�[�J�C�u�t�@�C�����ړ����܂�
@set archive_to=..\TkTweets.%MZ3_VERSION%.zip
@echo %archive% �� %archive_to%
@move %archive% %archive_to%
@cd %dist_dir%
:pause
