'====================================================================
' mz3.jp ���� MZ3 ���|�W�g���� trunk �̍ŐV���r�W�����ԍ����擾���A
' mz3_revision_in.h �𐶐�����
'====================================================================
Const URL = "http://mz3.jp/get_mz3_trunk_revision.php"
Const OUTPUT_FILENAME = "mz3_revision_in.h"

WScript.echo "mz3.jp ���� trunk �̃��r�W�������擾���Ă��܂�..."
Set http = CreateObject("microsoft.xmlhttp")
http.open "GET", URL, false
http.send
WScript.echo " => " & http.responsetext

Set Fs = WScript.CreateObject("Scripting.FileSystemObject")
Set f = Fs.CreateTextFile(OUTPUT_FILENAME, True)
f.WriteLine "#define MZ3_SVN_REVISION L""" & http.responsetext & """"
f.Close
WScript.echo OUTPUT_FILENAME & " �𐶐����܂���"
