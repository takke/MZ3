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
del /Q MZ3_win32\scripts\*
copy ..\src\MZ3\scripts\*                TkTweets_win32\scripts\
del /Q TkTweets_win32\scripts\mixi\*
del /Q TkTweets_win32\scripts\goohome\*
del /Q TkTweets_win32\user_scripts\*
copy ..\src\MZ3\user_scripts\*           TkTweets_win32\user_scripts\
copy ..\doc\Readme.txt                   TkTweets_win32\
copy ..\ChangeLog.txt                    TkTweets_win32\

:----- �t�@�C���̈��k -----
@set archive=MZ3_win32.LZH
@echo �� MZ3 �t�@�C�������k���܂�
@cd TkTweets_win32
@del %archive%
%UNLHA_EXE% @..\response_win32.txt

:----- �A�[�J�C�u�t�@�C���̈ړ� -----
@echo �� �A�[�J�C�u�t�@�C�����ړ����܂�
@set archive_to=..\TkTweets.%MZ3_VERSION%.lzh
@echo %archive% �� %archive_to%
@move %archive% %archive_to%
@cd %dist_dir%
:pause
