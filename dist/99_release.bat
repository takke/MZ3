: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat

:----- CAB/LZH �t�@�C�����̕ύX -----
echo MZ3.%MZ3_VERSION%.CAB ����ʃt�@�C�����ɕύX�ł��܂��B
set NEW_VER=0
set /P NEW_VER="�ύX����ꍇ�͐V�o�[�W�����ԍ�����͂��ĉ�����(��:1.2.3) > "
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

:----- �A�b�v���[�h -----
@set DAT_FILE=99_upload_bin_files.dat
set MZ3_VERSION=%NEW_VER%

: �X�N���v�g�t�@�C���̐���
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

: �X�N���v�g�m�F
type %DAT_FILE%

: sftp ���s
type %DAT_FILE% | sftp -C takeuchi@mz3.jp

: �X�N���v�g�t�@�C���폜
del %DAT_FILE%

:END
