: ビルド用環境変数の設定
@call 00_set_env.bat

:----- CAB/LZH ファイル名の変更 -----
echo MZ3.%MZ3_VERSION%.CAB 等を別ファイル名に変更できます。
set NEW_VER=0
set /P NEW_VER="変更する場合は新バージョン番号を入力して下さい(例:1.2.3) > "
:echo %NEW_VER%

IF %NEW_VER%==0 GOTO END

CD

echo "MZ3.%MZ3_VERSION%.CAB      => MZ3.%NEW_VER%.CAB"
move  MZ3.%MZ3_VERSION%.CAB         MZ3.%NEW_VER%.CAB

echo "MZ3Plus.%MZ3_VERSION%.CAB  => MZ3Plus.%NEW_VER%.CAB"
move  MZ3Plus.%MZ3_VERSION%.CAB     MZ3Plus.%NEW_VER%.CAB

echo "MZ4.%MZ3_VERSION%.lzh      => MZ4.%NEW_VER%.lzh"
move  MZ4.%MZ3_VERSION%.lzh         MZ4.%NEW_VER%.lzh

echo "TkTweets.%MZ3_VERSION%.CAB => TkTweets.%NEW_VER%.CAB"
move  TkTweets.%MZ3_VERSION%.CAB    TkTweets.%NEW_VER%.CAB

:----- アップロード -----
@set DAT_FILE=99_upload_bin_files.dat
set MZ3_VERSION=%NEW_VER%

: スクリプトファイルの生成
@echo cd /data/www/mz3/public/bin             > %DAT_FILE%
@echo put MZ3.%MZ3_VERSION%.CAB x.CAB         >> %DAT_FILE%
@echo rm MZ3.%MZ3_VERSION%.CAB                >> %DAT_FILE%
@echo rename x.CAB MZ3.%MZ3_VERSION%.CAB      >> %DAT_FILE%
@echo chmod 744 MZ3.%MZ3_VERSION%.CAB         >> %DAT_FILE%
@echo put MZ4.%MZ3_VERSION%.lzh x.lzh         >> %DAT_FILE%
@echo rm MZ4.%MZ3_VERSION%.lzh                >> %DAT_FILE%
@echo rename x.lzh MZ4.%MZ3_VERSION%.lzh      >> %DAT_FILE%
@echo chmod 744 MZ4.%MZ3_VERSION%.lzh         >> %DAT_FILE%
@echo put TkTweets.%MZ3_VERSION%.CAB x.CAB    >> %DAT_FILE%
@echo rm TkTweets.%MZ3_VERSION%.CAB           >> %DAT_FILE%
@echo rename x.CAB TkTweets.%MZ3_VERSION%.CAB >> %DAT_FILE%
@echo chmod 744 TkTweets.%MZ3_VERSION%.CAB    >> %DAT_FILE%
@echo bye                                     >> %DAT_FILE%

: スクリプト確認
type %DAT_FILE%

: sftp 実行
type %DAT_FILE% | sftp -C takeuchi@mz3.jp

: スクリプトファイル削除
del %DAT_FILE%

:END
