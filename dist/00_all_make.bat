: *****************************************************
: *** �S�z�z�t�@�C���̍쐬                          ***
: ***                                               ***
: *** MZ3 �{�̂� CAB �̃r���h�͎��O�ɍs���Ă������� ***
: *****************************************************

:----- CAB �t�@�C���̎��W -----
@call 10_gather_cab.bat

:----- src �t�@�C����cleanup&���k -----
:@call 20_archive_src.bat

:----- doc �t�@�C���̐��� -----
:@call 30_make_doxygen.bat

:----- doc �t�@�C���̈��k -----
:@call 40_archive_doxygen.bat

:----- Win32, lzh �t�@�C���̐��� -----
@call 50_archive_win32.bat

:----- luadoc �t�@�C���̐��� -----
@call 60_make_mz3_luadoc.bat

