: *********************************
: *** 本体の CAB ファイルの収集 ***
: *********************************

: ビルド用環境変数の設定
@call 00_set_env.bat

: コピー元/先の CAB ファイル
@set cab_from=..\src\MZ3cab\Release\MZ3.CAB
@set cab_to=.\MZ3.%MZ3_VERSION%.CAB

:----- CAB ファイルの収集 -----
@echo ■ src ファイルを収集します
@echo %cab_from% ⇒ %cab_to%
@copy %cab_from% %cab_to%
