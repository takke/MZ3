@echo off

: *.luac ƒtƒ@ƒCƒ‹íœ
echo *** delete luac files... ***
for /r %%F in (*.luac*) do (
  echo delete %%F
  @del "%%F"
)

pause
