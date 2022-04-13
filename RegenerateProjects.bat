call vendor\premake\premake5.exe vs2019
call vendor\premake\GBCFindAppend.exe GBC\GBC.vcxproj "</LanguageStandard>" "<UseStandardPreprocessor>true</UseStandardPreprocessor>"
call vendor\premake\GBCFindAppend.exe GBC\GBC.vcxproj "<UseStandardPreprocessor>" true
call vendor\premake\GBCFindAppend.exe EZ80IDE\EZ80IDE.vcxproj "</LanguageStandard>" "<UseStandardPreprocessor>true</UseStandardPreprocessor>"
call vendor\premake\GBCFindAppend.exe EZ80IDE\EZ80IDE.vcxproj "<UseStandardPreprocessor>" true
pause
