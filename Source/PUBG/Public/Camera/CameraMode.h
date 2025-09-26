// Kakanu Portfolio Project

#pragma once

#include "UObject/NoExportTypes.h"
#include "CameraMode.generated.h"

DECLARE_DYNAMIC_DELEGATE(FTimelineEnd);

class UCurveFloat;
class USpringArmComponent;
class UCameraComponent;
class APUBGBaseCharacter;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PUBG_API UCameraMode : public UObject
{
	GENERATED_BODY()
	
public:
	UCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintNativeEvent)
	void Init(APUBGBaseCharacter* InCharacter);

public:
	UFUNCTION()
	UCurveFloat* GetCurveFloat();

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnUpdate(float Output);

	UFUNCTION(BlueprintNativeEvent)
	void OnEnd();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* CurveFloat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<APUBGBaseCharacter> Character;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AController> Controller;

	// ��ġ, ȸ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<USpringArmComponent> SpringArmComponent;

	// ��/�ƿ�/FOV,ī�޶� ��鸲/PostProcess ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<UCameraComponent> CameraComponent;
};
