: ビルド用環境変数の設定
@call 00_set_env.bat
@set DAT_FILE=99_upload_private_files.dat

: スクリプトファイルの生成
@echo cd /var/www/mz3/private   > %DAT_FILE%
@echo put MZ3.%MZ3_VERSION%.CAB >> %DAT_FILE%
@echo put MZ4.%MZ3_VERSION%.lzh >> %DAT_FILE%
@echo bye                       >> %DAT_FILE%

: スクリプト確認
type %DAT_FILE%

: sftp 実行
type %DAT_FILE% | sftp -C takeuchi@mz3.jp

: スクリプトファイル削除
del %DAT_FILE%
pause
