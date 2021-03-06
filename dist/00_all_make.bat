: *****************************************************
: *** 全配布ファイルの作成                          ***
: ***                                               ***
: *** MZ3 本体と CAB のビルドは事前に行っておくこと ***
: *****************************************************
chcp 932

@PUSHD .

:----- CAB ファイルの収集 -----
@call 10_gather_cab.bat

:----- doc ファイルの生成 -----
:@call 30_make_doxygen.bat

:----- doc ファイルの圧縮 -----
:@call 40_archive_doxygen.bat

:----- Win32, zip ファイルの生成 -----
@call 50_archive_win32.bat
@call 51_archive_tktweets_win32.bat

:----- luadoc ファイルの生成 -----
@call 60_make_mz3_luadoc.bat

@POPD

:----- CAB/LZH ファイル名の変更/アップロード
@call 99_release.bat
