: ****************************************
: *** ソースドキュメントのアーカイブ化 ***
: ****************************************

: ビルド用環境変数の設定
@call 00_set_env.bat

@set dist_dir=%CD%
@set DOC=doc.zip
@set DOC_TO=..\dist\MZ3.%MZ3_VERSION%.srcdoc.zip

@echo ■ ソースドキュメントファイルを圧縮します
@cd ..\doxygen
@del %DOC%
@%_7Z_EXE% a -r -tzip %DOC% *

@echo ■ 圧縮したソースドキュメントファイルを移動します
@echo %DOC% ⇒ %DOC_TO%
@move %DOC% %DOC_TO%
@cd %dist_dir%
@pause
