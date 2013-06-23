: *********************************
: *** 本体の CAB ファイルの収集 ***
: *********************************

: ビルド用環境変数の設定
@call 00_set_env.bat


:
:--- MZ3 ---
:
: コピー元/先の CAB ファイル
@set cab_from=..\src\MZ3cab\Release\MZ3.CAB
@set cab_to=.\MZ3.%MZ3_VERSION%.CAB

:----- CAB ファイルの収集 -----
@echo ■ src ファイルを収集します
@echo %cab_from% ⇒ %cab_to%
@copy %cab_from% %cab_to%


:
:--- MZ3 Plus ---
:
: コピー元/先の CAB ファイル
@set cab_from=..\src\MZ3cab\Release_MZ3Plus\MZ3.CAB
@set cab_to=.\MZ3Plus.%MZ3_VERSION%.CAB

:----- CAB ファイルの収集 -----
:@echo ■ src ファイルを収集します
:@echo %cab_from% ⇒ %cab_to%
:@copy %cab_from% %cab_to%


:
:--- TkTweets ---
:
: コピー元/先の CAB ファイル
@set cab_from=..\src\MZ3cab\TkTw_R_WM\TkTweets.CAB
@set cab_to=.\TkTweets.%MZ3_VERSION%.CAB

:----- CAB ファイルの収集 -----
@echo ■ src ファイルを収集します
@echo %cab_from% ⇒ %cab_to%
@copy %cab_from% %cab_to%
