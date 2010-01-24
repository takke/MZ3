: *************************************************
: *** TkTweets for Win32 ファイルのアーカイブ化 ***
: *************************************************

: ビルド用環境変数の設定
@call 00_set_env.bat

@set dist_dir=%CD%

:----- ファイルのコピー -----
@echo ■ ファイルを収集します
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

:----- ファイルの圧縮 -----
@set archive=MZ3_win32.LZH
@echo ■ MZ3 ファイルを圧縮します
@cd TkTweets_win32
@del %archive%
%UNLHA_EXE% @..\response_win32.txt

:----- アーカイブファイルの移動 -----
@echo ■ アーカイブファイルを移動します
@set archive_to=..\TkTweets.%MZ3_VERSION%.lzh
@echo %archive% ⇒ %archive_to%
@move %archive% %archive_to%
@cd %dist_dir%
:pause
