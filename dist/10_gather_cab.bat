: *********************************
: *** �{�̂� CAB �t�@�C���̎��W ***
: *********************************

: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat


:
:--- MZ3 ---
:
: �R�s�[��/��� CAB �t�@�C��
@set cab_from=..\src\MZ3cab\Release\MZ3.CAB
@set cab_to=.\MZ3.%MZ3_VERSION%.CAB

:----- CAB �t�@�C���̎��W -----
@echo �� src �t�@�C�������W���܂�
@echo %cab_from% �� %cab_to%
@copy %cab_from% %cab_to%


:
:--- MZ3 Plus ---
:
: �R�s�[��/��� CAB �t�@�C��
@set cab_from=..\src\MZ3cab\Release_MZ3Plus\MZ3.CAB
@set cab_to=.\MZ3Plus.%MZ3_VERSION%.CAB

:----- CAB �t�@�C���̎��W -----
:@echo �� src �t�@�C�������W���܂�
:@echo %cab_from% �� %cab_to%
:@copy %cab_from% %cab_to%


:
:--- TkTweets ---
:
: �R�s�[��/��� CAB �t�@�C��
@set cab_from=..\src\MZ3cab\TkTw_R_WM\TkTweets.CAB
@set cab_to=.\TkTweets.%MZ3_VERSION%.CAB

:----- CAB �t�@�C���̎��W -----
@echo �� src �t�@�C�������W���܂�
@echo %cab_from% �� %cab_to%
@copy %cab_from% %cab_to%
