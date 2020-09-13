// juaxix - 2020 - xixgames

using UnrealBuildTool;
using System.Collections.Generic;

public class AI4AnimationEditorTarget : TargetRules
{
	public AI4AnimationEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "AI4Animation" } );
	}
}
