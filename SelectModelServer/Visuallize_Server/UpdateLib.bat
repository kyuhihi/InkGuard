set InPath=%1
if "%InPath%"=="" (
	set InPath=.
)

:: Reference
xcopy /y/s %InPath%\Engine\Public\*.* %InPath%\Reference\Headers\
xcopy /y/s %InPath%\Engine\Public\*.hpp %InPath%\Reference\Headers\
xcopy /y/s %InPath%\Engine\Public\*.cs %InPath%\Reference\Headers\
xcopy /y %InPath%\Engine\Bin\*.lib %InPath%\Reference\Librarys\

xcopy /y %InPath%\Engine\ThirdParty\x64\*.* %InPath%\Reference\Librarys\x64\
xcopy /y %InPath%\Engine\ThirdParty\x86\*.* %InPath%\Reference\Librarys\x86\

:: Client
xcopy /y %InPath%\Engine\Bin\*.dll %InPath%\Client\Bin\
xcopy /y %InPath%\Engine\Bin\ShaderFiles\*.hlsl %InPath%\Client\Bin\ShaderFiles\

:: Tool
xcopy /y %InPath%\Engine\Bin\*.dll %InPath%\Tool\Bin\
xcopy /y %InPath%\Engine\Bin\ShaderFiles\*.hlsl %InPath%\Tool\Bin\ShaderFiles\