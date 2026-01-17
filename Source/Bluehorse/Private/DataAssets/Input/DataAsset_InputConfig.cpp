// Shungen All Rights Reserved.


#include "DataAssets/Input/DataAsset_InputConfig.h"

// w’è‚³‚ê‚½ GameplayTag ‚É‘Î‰‚·‚é Native InputAction ‚ğŒŸõ‚µ‚Ä•Ô‚·
UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	// “o˜^‚³‚ê‚Ä‚¢‚é NativeInputActions ‚©‚ç Tag ‚ªˆê’v‚·‚é‚à‚Ì‚ğ’T‚·
	for (const FBluehorseInputActionConfig& InputActionConfig : NativeInputActions)
	{
		// Tag ‚ªˆê’v‚µA‚©‚Â InputAction ‚ª—LŒø‚Èê‡‚Ì‚İ•Ô‚·
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}

	// –¢“o˜^A‚à‚µ‚­‚Í InputAction ‚ª–¢İ’è‚Ìê‡
	return nullptr;
}
