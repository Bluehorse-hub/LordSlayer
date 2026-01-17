// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"

// Pawn に付与される拡張コンポーネントの基底クラス

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLUEHORSE_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	// OwnerがAPawnから派生している型であることを確認し安全に取得するテンプレート関数
	template <class T>
	T* GetOwningPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' Template Parameter to GetPawn must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}

	// APawn型としてOwnerを取得する関数
	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	// OwnerのControllerを指定されたAContorollerの派生型として取得するテンプレート関数
	template <class T>
	T* GetOwningController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T' Template Parameter to GetController must be derived from AController");
		return GetOwningPawn<APawn>()->GetController<T>();
	}
};
