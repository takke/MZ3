: *****************************************************
: *** �S�z�z�t�@�C���̍쐬                          ***
: ***                                               ***
: *** MZ3 �{�̂� CAB �̃r���h�͎��O�ɍs���Ă������� ***
: *****************************************************
chcp 932

@PUSHD .

:----- CAB �t�@�C���̎��W -----
@call 10_gather_cab.bat

:----- doc �t�@�C���̐��� -----
:@call 30_make_doxygen.bat

:----- doc �t�@�C���̈��k -----
:@call 40_archive_doxygen.bat

:----- Win32, zip �t�@�C���̐��� -----
@call 50_archive_win32.bat
@call 51_archive_tktweets_win32.bat

:----- luadoc �t�@�C���̐��� -----
@call 60_make_mz3_luadoc.bat

@POPD

:----- CAB/LZH �t�@�C�����̕ύX/�A�b�v���[�h
@call 99_release.bat
