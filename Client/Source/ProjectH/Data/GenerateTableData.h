# pragma once

#include "Engine/DataTable.h"
#include "EnumGenerateData.h"
#include "GenerateTableData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StatusNo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> SkillIDs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> SkillNames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BattleResource;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ABP_Path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Filpbook_Path;
};
USTRUCT(BlueprintType)
struct FCharacterStatusData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PDEF;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EDEF;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CRI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
};
USTRUCT(BlueprintType)
struct FCommonString : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ko;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString en;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString jp;
};
USTRUCT(BlueprintType)
struct FDialogData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NextDailogNo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Speaker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ko;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString en;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString jp;
};
USTRUCT(BlueprintType)
struct FEventData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString eventName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString eventDesc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString openType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString openValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString bannerPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString toggleImgPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString openPageImgPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> rewardItems;
};
USTRUCT(BlueprintType)
struct FMonsterData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MonsterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMonsterType MonsterType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StatusNo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> SkillNos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ResourcePath;
};
USTRUCT(BlueprintType)
struct FMonsterGroupData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> MonsterIDs;
};
USTRUCT(BlueprintType)
struct FMonsterStatusData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PDEF;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EDEF;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CRI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Toughness;
};
USTRUCT(BlueprintType)
struct FNPCData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInteractionType InteractionType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractionValue;
};
USTRUCT(BlueprintType)
struct FSceneData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DataPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESceneType SceneType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SceneValue;
};
USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillTargetType SkillTargetType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageType DamageType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AddDamageRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseCritical;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HitCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostSP;
};
USTRUCT(BlueprintType)
struct FSkillString : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ko;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString en;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString jp;
};
USTRUCT(BlueprintType)
struct FStatusData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DEF;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
};

