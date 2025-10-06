#pragma once

UENUM()
enum class EBluehorseConfirmType : uint8
{
	Yes,
	No
};

UENUM()
enum class EBluehorseValidType : uint8
{
	Valid,
	Invalid
};

UENUM(BlueprintType)
enum class EToggleDamageType : uint8
{
	EquippedLeftHandWeapon,
	EquippedRightHandWeapon,
	LeftHand,
	RightHand
};

UENUM(BlueprintType)
enum class EBluehorseSuccessType : uint8
{
	Successful,
	Failed
};

UENUM(BlueprintType)
enum class ELocomotionType : uint8
{
	Default,
	Blocking,
	TargetLocking,
	BlockingAndTargetLocking,
};

UENUM(BlueprintType)
enum class ERollDirection : uint8
{
	Forward,
	ForwardRight,
	ForwardLeft,
	BackRight,
	BackLeft,
	Right,
	Left,
	Back
};

UENUM(BlueprintType)
enum class EBluehorseInputMode : uint8
{
	GameOnly,
	UIOnly,
	GameAndUI
};