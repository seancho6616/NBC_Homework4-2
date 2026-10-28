// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_TitleLayout.generated.h"

class UButton;
class UEditableText;

UCLASS()
class CX_API UUW_TitleLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_TitleLayout(const FObjectInitializer& ObjcetInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> EndButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> ServerIPEditableText;

};
