// Shungen All Rights Reserved.


#include "DataAssets/Item/ItemData.h"

UNiagaraSystem* UItemData::GetUseEffect()
{
    if (UseEffect.IsNull())
    {
        return nullptr;
    }

    if (!UseEffect.IsValid())
    {
        return UseEffect.LoadSynchronous();
    }

    return UseEffect.Get();
}

USoundBase* UItemData::GetUseSound()
{
    if (UseSound.IsNull())
    {
        return nullptr;
    }

    if (!UseSound.IsValid())
    {
        return UseSound.LoadSynchronous();
    }

    return UseSound.Get();
}

