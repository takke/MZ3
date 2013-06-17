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
rmdir /Q /S TkTweets_win32\scripts
mkdir TkTweets_win32\scripts
mkdir TkTweets_win32\scripts\lib
copy ..\src\MZ3\scripts\*                TkTweets_win32\scripts\
del MZ3_win32\scripts\_old.*
copy ..\src\MZ3\scripts\lib\*            TkTweets_win32\scripts\lib
copy ..\src\MZ3\user_scripts\*           TkTweets_win32\user_scripts\
copy ..\doc\Readme.txt                   TkTweets_win32\
copy ..\ChangeLog.txt                    TkTweets_win32\

:----- ファイルの圧縮 -----
@set archive=TkTweets_win32.zip
@echo ■ TkTweets ファイルを圧縮します
@cd TkTweets_win32
@del %archive%
@%_7Z_EXE% a -r -tzip %archive% *

:----- アーカイブファイルの移動 -----
@echo ■ アーカイブファイルを移動します
@set archive_to=..\TkTweets.%MZ3_VERSION%.zip
@echo %archive% ⇒ %archive_to%
@move %archive% %archive_to%
@cd %dist_dir%
:pause
