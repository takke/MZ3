@echo off

: *.luac �t�@�C���폜
echo *** delete luac files... ***
for /r %%F in (*.luac*) do (
  echo delete %%F
  @del "%%F"
)

: �t�@�C�����ꗗ����
echo *** generate target file list... ***
set FILE_LIST=_compile_target_files.txt
del %FILE_LIST%
for /r %%F in (*.lua) do (
  echo %%F>> %FILE_LIST%
)

: �t�@�C�����ꗗ��Ώۂ� luac ���s
echo *** compile lua files... ***
for /F "delims=," %%F in (%FILE_LIST%) do (
  echo luac -o %%Fc %%F
  ..\..\..\src_lib\lua\bin\luac5.1.exe -o "%%Fc" "%%F"
)

: �t�@�C�����ꗗ�폜
del %FILE_LIST%

pause
