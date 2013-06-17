: ビルド用環境変数の設定
@call 00_set_env.bat
@set DAT_FILE=99_upload_private_files.dat

: スクリプトファイルの生成
@echo cd /data/www/mz3/public/private         > %DAT_FILE%

@echo put MZ3.%MZ3_VERSION%.CAB x.CAB         >> %DAT_FILE%
@echo rm MZ3.%MZ3_VERSION%.CAB                >> %DAT_FILE%
@echo rename x.CAB MZ3.%MZ3_VERSION%.CAB      >> %DAT_FILE%
@echo chmod 744 MZ3.%MZ3_VERSION%.CAB         >> %DAT_FILE%

@echo put MZ4.%MZ3_VERSION%.zip x.zip         >> %DAT_FILE%
@echo rm MZ4.%MZ3_VERSION%.zip                >> %DAT_FILE%
@echo rename x.zip MZ4.%MZ3_VERSION%.zip      >> %DAT_FILE%
@echo chmod 744 MZ4.%MZ3_VERSION%.zip         >> %DAT_FILE%

@echo put TkTweets.%MZ3_VERSION%.CAB x.CAB    >> %DAT_FILE%
@echo rm TkTweets.%MZ3_VERSION%.CAB           >> %DAT_FILE%
@echo rename x.CAB TkTweets.%MZ3_VERSION%.CAB >> %DAT_FILE%
@echo chmod 744 TkTweets.%MZ3_VERSION%.CAB    >> %DAT_FILE%

@echo put TkTweets.%MZ3_VERSION%.zip x.zip    >> %DAT_FILE%
@echo rm TkTweets.%MZ3_VERSION%.zip           >> %DAT_FILE%
@echo rename x.zip TkTweets.%MZ3_VERSION%.zip >> %DAT_FILE%
@echo chmod 744 TkTweets.%MZ3_VERSION%.zip    >> %DAT_FILE%

@echo bye                                     >> %DAT_FILE%

: スクリプト確認
type %DAT_FILE%

: sftp 実行
type %DAT_FILE% | sftp -C takeuchi@mz3.jp

: スクリプトファイル削除
del %DAT_FILE%
pause
