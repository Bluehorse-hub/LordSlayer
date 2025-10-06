// Shungen All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLUEHORSE_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Owner��APawn����h�����Ă���^�ł��邱�Ƃ��m�F�����S�Ɏ擾����e���v���[�g�֐�
	template <class T>
	T* GetOwningPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' Template Parameter to GetPawn must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}

	// APawn�^�Ƃ���Owner���擾����֐�
	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	// Owner��Controller���w�肳�ꂽAContoroller�̔h���^�Ƃ��Ď擾����e���v���[�g�֐�
	template <class T>
	T* GetOwningController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T' Template Parameter to GetController must be derived from AController");
		return GetOwningPawn<APawn>()->GetController<T>();
	}
};
