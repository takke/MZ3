@echo off

: *.luac ファイル削除
echo *** delete luac files... ***
for /r %%F in (*.luac*) do (
  echo delete %%F
  @del "%%F"
)

: ファイル名一覧生成
echo *** generate target file list... ***
set FILE_LIST=_compile_target_files.txt
del %FILE_LIST%
for /r %%F in (*.lua) do (
  echo %%F>> %FILE_LIST%
)

: ファイル名一覧を対象に luac 実行
echo *** compile lua files... ***
for /F "delims=," %%F in (%FILE_LIST%) do (
  echo luac -o %%Fc %%F
  ..\..\..\src_lib\lua\bin\luac5.1.exe -o "%%Fc" "%%F"
)

: ファイル名一覧削除
del %FILE_LIST%

pause
