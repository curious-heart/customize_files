@echo off
set usage_str=用法：deploy_release.bat 目标文件夹 版本号 [定制内容文件夹]
set count=0
for %%i in (%*) do (
  set /a count+=1
)

if %count% LSS 2 (
    @echo %usage_str%
    pause
    exit /B
)

set dest_folder=%1
set ver_str=%2
if %count% EQU 3 (
    set cust_files_folder=%3
) else (
    set cust_files_folder=
)

set winrar_exe="C:\Program Files\WinRAR\winrar.exe"
set QT_DEPLOY_EXE=D:\01.Prog\Qt5.15\5.15.2\msvc2019_64\bin\windeployqt.exe

set exe_file_base_name=customize_files
set exe_folder=..\build-customize_files-Desktop_Qt_5_15_2_MSVC2019_64bit-Release\release
if "%cust_files_folder%" == "" (
    echo no cust files!
    echo
    set exe_dest_folder=%dest_folder%
) else (
    echo there are cust files!
    echo
    set exe_dest_folder=%dest_folder%\%exe_file_base_name%
    xcopy %cust_files_folder%\* %dest_folder%\%cust_files_folder% /Y /I /E
)

xcopy .\configs %exe_dest_folder%\configs /Y /E /I
xcopy .\*.docx %dest_folder%\ /Y /I
copy %exe_folder%\%exe_file_base_name%.exe %exe_dest_folder%\
%QT_DEPLOY_EXE% %exe_dest_folder%\%exe_file_base_name%.exe
%winrar_exe% a -df -r -ep1 -tk %dest_folder%\%exe_file_base_name%-%ver_str%.zip %dest_folder%\*

@echo 完成！
pause
