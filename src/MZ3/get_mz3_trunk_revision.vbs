'====================================================================
' mz3.jp から MZ3 リポジトリの trunk の最新リビジョン番号を取得し、
' mz3_revision_in.h を生成する
'====================================================================
Const URL = "http://mz3.jp/get_mz3_trunk_revision.php"
Const OUTPUT_FILENAME = "mz3_revision_in.h"

WScript.echo "mz3.jp から trunk のリビジョンを取得しています..."
Set http = CreateObject("microsoft.xmlhttp")
http.open "GET", URL, false
http.send
WScript.echo " => " & http.responsetext

Set Fs = WScript.CreateObject("Scripting.FileSystemObject")
Set f = Fs.CreateTextFile(OUTPUT_FILENAME, True)
f.WriteLine "#define MZ3_SVN_REVISION L""" & http.responsetext & """"
f.Close
WScript.echo OUTPUT_FILENAME & " を生成しました"
