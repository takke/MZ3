@echo off

: *.luac �t�@�C���폜
echo *** delete luac files... ***
for /r %%F in (*.luac*) do (
  echo delete %%F
  @del "%%F"
)

pause
