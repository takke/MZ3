: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat
@set DAT_FILE=99_upload_private_files.dat

: �X�N���v�g�t�@�C���̐���
@echo cd /var/www/mz3/private   > %DAT_FILE%
@echo put MZ3.%MZ3_VERSION%.CAB >> %DAT_FILE%
@echo put MZ4.%MZ3_VERSION%.lzh >> %DAT_FILE%
@echo bye                       >> %DAT_FILE%

: �X�N���v�g�m�F
type %DAT_FILE%

: sftp ���s
type %DAT_FILE% | sftp -C takeuchi@mz3.jp

: �X�N���v�g�t�@�C���폜
del %DAT_FILE%
pause
