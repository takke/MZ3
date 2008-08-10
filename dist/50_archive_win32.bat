: ************************************
: *** Win32 ファイルのアーカイブ化 ***
: ************************************

: ビルド用環境変数の設定
@call 00_set_env.bat

@set dist_dir=%CD%

:----- ファイルのコピー -----
@echo ■ ファイルを収集します
copy ..\src\MZ3\Release_Win32\MZ4.exe MZ3_win32\
copy "..\doc\Help Files\MZ4\MZ4.chm" MZ3_win32\
copy ..\doc\Readme.txt MZ3_win32\
copy ..\ChangeLog.txt MZ3_win32\
copy toppage_group_sample.ini MZ3_win32\
copy ..\src\MZ3GroupFileEditor\Release\MZ3GroupFileEditor.exe MZ3_win32\

:----- ファイルの圧縮 -----
@set archive=MZ3_win32.lzh
@echo ■ MZ3 ファイルを圧縮します
@cd MZ3_win32
@del %archive%
%UNLHA_EXE% @..\response_win32.txt

:----- アーカイブファイルの移動 -----
@echo ■ アーカイブファイルを移動します
@set archive_to=..\MZ4.%MZ3_VERSION%.lzh
@echo %archive% ⇒ %archive_to%
@move %archive% %archive_to%
@cd %dist_dir%
pause
