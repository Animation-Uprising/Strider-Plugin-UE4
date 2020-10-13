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
				"AnimGraph",
				"AnimGraphRuntime",
				"Strider",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Strider",
				"AnimGraph",
				"BlueprintGraph",
				"AnimGraphRuntime",
				"Projects",
				
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
