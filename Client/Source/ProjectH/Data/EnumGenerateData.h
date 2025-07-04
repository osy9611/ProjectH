# pragma once

#include "EnumGenerateData.generated.h"

UENUM(BlueprintType)
enum class EInteractionType:uint8
{
	NONE, 
	Talk, //대화
	Shop, //상점
};

UENUM(BlueprintType)
enum class ECharType:uint8
{
	Character, //캐릭터
	Monster, //몬스터
};

UENUM(BlueprintType)
enum class ESceneType:uint8
{
	Field, //필드 씬
	Battle, //배틀 씬
};

UENUM(BlueprintType)
enum class EMonsterType:uint8
{
	Normal, //일반
	Boss, //보스
};

UENUM(BlueprintType)
enum class EDamageType:uint8
{
	NONE, //없음(버프나 힐링 스킬등)
	Physical, //물리 데미지
	Magical, //마법 데미지
};

UENUM(BlueprintType)
enum class ESkillTargetType:uint8
{
	Mine, //물리 데미지
	TeamAll, //마법 데미지
	Enemy_One, //물리 데미지
	Enemy_All, //마법 데미지
	Random, //물리 데미지
};


