: *********************************
: *** �{�̂� CAB �t�@�C���̎��W ***
: *********************************

: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat

: �R�s�[��/��� CAB �t�@�C��
@set cab_from=..\src\MZ3cab\Release\MZ3.CAB
@set cab_to=.\MZ3.%MZ3_VERSION%.CAB

:----- CAB �t�@�C���̎��W -----
@echo �� src �t�@�C�������W���܂�
@echo %cab_from% �� %cab_to%
@copy %cab_from% %cab_to%
