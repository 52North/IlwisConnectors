@Echo Off

echo save old PATH
set OLD=%PATH%

echo add Qt- and ILWIS-path to PATH
call prepare_PATH.bat

echo ==========start test.py==============
python test.py
echo ==========test.py ended==============

echo reset PATH
set PATH=%OLD%
