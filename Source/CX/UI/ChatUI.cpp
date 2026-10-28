// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatUI.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/MultiLineEditableTextBox.h"
#include "CX/CXPlayerController.h"

void UChatUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (!EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &UChatUI::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &UChatUI::OnChatInputTextCommitted);
	}
}

void UChatUI::NativeDestruct()
{
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &UChatUI::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &UChatUI::OnChatInputTextCommitted);
	}

	Super::NativeDestruct();
}

void UChatUI::OnNewChatMessage(const FString& InSender, const FString& InMessage)
{
	FString NewMessage = InSender + TEXT(": ") + InMessage;
	FString OldMessage = MultiLineEditableTextBox_ChatLog->GetText().ToString();
	OldMessage += '\n';
	FText Message = FText::FromString(OldMessage + NewMessage);


	MultiLineEditableTextBox_ChatLog->SetText(Message);
}

void UChatUI::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsValid(EditableTextBox_ChatInput))
	{
		return;
	}

	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (!Text.IsEmpty())
		{
			if (ACXPlayerController* PC = GetOwningPlayer<ACXPlayerController>())
			{
				// Send chat message to server
				FString ChatMessage = Text.ToString();
				PC->CTSChatMessage(ChatMessage);
				EditableTextBox_ChatInput->SetText(FText::GetEmpty());
			}
		}
	}
}
