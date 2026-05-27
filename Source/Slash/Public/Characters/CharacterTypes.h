#pragma once

// A normal enum is declared with the "enum" keyword followed by the enum name, e.g. enum CharacterState.
// In Unreal Engine, typically enums are scoped, and the enum name is prefixed with a capital "E", e.g.
// enum class ECharacterState. By scoping an enum what happens is that when you're referencing it you
// have to fully qualify it with the scope resolution operator, e.g. ECharacterState::Unequipped. This
// is intended to be a safeguard that protects against having multiple enums that may have the same
// enum constant, e.g. if two different enums had the "Unequipped" constant.
//
// Enums are just "integer constants" in that each constant is associated with an integer value. The
// first enum constant is 0 by default, although you can reassign the integer values, e.g.
// Unequipped = 3, EquippedOneHandedWeapon = 5, etc.
//
// You can also set the type of the enum value via : uint8, which is an unsigned 8-bit integer.
//
// UENUM() exposes the enum to the reflection system, and also allows us to use it as a type in
// blueprints.
//
// UMETA() allows us to change the display name in Blueprints, e.g. UMETA(DisplayName = "Unequipped")

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "EquippingWeapon")
};

UENUM(BlueprintType)
enum class EDeathPose: uint8
{
	EDP_Alive UMETA(DisplayName = "Alive"),
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking")
};
