: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat
@set DAT_FILE=99_upload_private_files.dat

: �X�N���v�g�t�@�C���̐���
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

: �X�N���v�g�m�F
type %DAT_FILE%

: sftp ���s
type %DAT_FILE% | sftp -C takeuchi@mz3.jp

: �X�N���v�g�t�@�C���폜
del %DAT_FILE%
pause
