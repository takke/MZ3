@echo off

: *.luac ファイル削除
echo *** delete luac files... ***
for /r %%F in (*.luac*) do (
  echo delete %%F
  @del "%%F"
)

pause
