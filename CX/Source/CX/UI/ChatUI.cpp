// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatUI.h"
#include "CX/CXPlayerController.h"

void UChatUI::OnNewChatMessage(const FString& InSender, const FString& InMessage)
{
	FString NewMessage = InSender + TEXT(": ") + InMessage;

	FString OldMessage = MultilineEditableTexBox_ChatLog->GetText().ToString();
	OldMessage += "\n";
	FText Message = FText::FromString(OldMessage + NewMessage);

	MultilineEditableTexBox_ChatLog->SetText(Message);
}

void UChatUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &UChatUI::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &UChatUI::OnChatInputTextCommitted);
	}
}

void UChatUI::NativeDestruct()
{
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &UChatUI::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &UChatUI::OnChatInputTextCommitted);
	}
	Super::NativeDestruct();
}

void UChatUI::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (Text.IsEmpty() == false)
		{
			FString ChatMessage = Text.ToString();
			EditableTextBox_ChatInput->SetText(FText::GetEmpty());
			if (ACXPlayerController* PC = GetOwningPlayer<ACXPlayerController>())
			{
				if (PC->IsLocalController())
				{
					PC->CTSChatMessage(ChatMessage);
				}
			}
		}
	}
}
