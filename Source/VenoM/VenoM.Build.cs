// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class VenoM : ModuleRules
{
    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }
 
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }
 
    private string DLLPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Binaries/")); }
    }
    
	public VenoM(TargetInfo Target)
	{
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "Sockets",
            "Networking", "UMG", "Slate", "SlateCore", "AssetRegistry" });
        LoadIrrKlang(Target);
	}
    
    public bool LoadIrrKlang(TargetInfo Target)
    {
        bool isLibrarySupported = false;
 
        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;
 
            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, "irrKlang", "Libraries", PlatformString);
 
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "irrKlang.lib")); 
            //System.IO.File.Copy(Path.Combine(LibrariesPath, "irrKlang.dll"), Path.Combine(DLLPath, ThirdPartyPath, "irrKlang.dll"), true);
            //System.IO.File.Copy(Path.Combine(LibrariesPath, "irrKlang.dll"), "C:/Windows/System32/irrKlang.dll", true);
        }
 
        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "irrKlang", "Includes"));
        }
 
        Definitions.Add(string.Format("WITH_IRR_KLANG_BINDING={0}", isLibrarySupported ? 1 : 0));
 
        return isLibrarySupported;
    }
}
