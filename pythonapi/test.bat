@Echo Off
echo save old PATH
set OLD=%PATH%
echo add Qt- and ILWIS-path to PATH
set PATH=%PATH%;C:\Qt\Qt5.1.1\5.1.1\mingw48_32\bin;C:\Users\Poku\dev\Ilwis4\projects\output\win32debug\bin
echo ==========start test.py==============
python test.py
echo ==========test.py ended==============
echo reset PATH
set PATH=%OLD%
