: ********************************
: *** ソースドキュメントの生成 ***
: ********************************

: ビルド用環境変数の設定
@call 00_set_env.bat

@set dist_dir=%CD%

:----- doc ファイルの生成 -----
@echo ■ doc ファイルを生成します
@cd ..\doxygen
doxygen
@cd %dist_dir%
pause
