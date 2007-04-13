: ************************************
: *** ソースファイルのアーカイブ化 ***
: ************************************

: ビルド用環境変数の設定
@call 00_set_env.bat

@set dist_dir=%CD%

:----- src ファイルのcleanup -----
@echo ■ src ファイルをcleanupします
@cd ..\src
@call clean.bat
@cd %dist_dir%

:----- src ファイルの圧縮 -----
@set archive=src.lzh
@echo ■ src ファイルを圧縮します
@cd ..
@del %archive%
%UNLHA_EXE% @%dist_dir%\response_src.txt

:----- アーカイブファイルの移動 -----
@echo ■ アーカイブファイルを移動します
@set archive_to=dist\MZ3.%MZ3_VERSION%.src.lzh
@echo %archive% ⇒ %archive_to%
@move %archive% %archive_to%
@cd %dist_dir%
