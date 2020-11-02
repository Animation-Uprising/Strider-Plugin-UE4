// Copyright 2020 Kenneth Claassen, All Rights Reserved.

using UnrealBuildTool;

public class StriderEditor : ModuleRules
{
	public StriderEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;	
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Strider",
				"AnimGraph",
				"BlueprintGraph",
				"AnimGraphRuntime",
				// ... add private dependencies that you statically link with here ...	
			}
			);
			
		// PrivateInlcludePathModuleNames.AddRange(
			// new string[] {

			// }
			// );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				//"AssetTools",
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
