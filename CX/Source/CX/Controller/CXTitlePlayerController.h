// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CXTitlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CX_API ACXTitlePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void JoinServer(const FString& InIPAddress);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ASUIPlayerContrroller, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UIWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ASUIPlayerContrroller, Meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> UIWidgetInstance;
};
