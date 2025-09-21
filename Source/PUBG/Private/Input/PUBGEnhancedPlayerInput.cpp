// Kakanu Portfolio Project


#include "Input/PUBGEnhancedPlayerInput.h"
#include "EnhancedActionKeyMapping.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGEnhancedPlayerInput)

UPUBGEnhancedPlayerInput::UPUBGEnhancedPlayerInput()
{
}

void UPUBGEnhancedPlayerInput::FlushPressedInput(UInputAction* InputAction)
{
	const TArray<FEnhancedActionKeyMapping>& KeyMappings = GetEnhancedActionMappings();
	for (const FEnhancedActionKeyMapping& KeyMapping : KeyMappings)
	{
		if (KeyMapping.Action == InputAction)
		{
			APlayerController* PlayerController = GetOuterAPlayerController();
			ULocalPlayer* LocalPlayer = PlayerController ? Cast<ULocalPlayer>(PlayerController->Player) : nullptr;

			if (LocalPlayer)
			{
				// 눌린키 해제 처리
				if (FKeyState* KeyState = GetKeyStateMap().Find(KeyMapping.Key))
				{
					// 파라미터로 제거
					if (KeyState->bDown)
					{
						FInputKeyEventArgs Params;
						Params.Viewport = GEngine->GameViewport->Viewport;
						Params.ControllerId = 0;
						Params.Key = KeyMapping.Key;
						Params.Event = IE_Released;
						Params.InputDevice = FInputDeviceId::CreateFromInternalId(0);
						Params.NumSamples = 1;
						Params.AmountDepressed = 0.0f;
						Params.bIsTouchEvent = false;
						InputKey(Params);
					}
				}
			}

			UWorld* World = GetWorld();
			check(World);
			float TimeSeconds = World->GetRealTimeSeconds();

			// 키 상태 초기화
			if (FKeyState* KeyState = GetKeyStateMap().Find(KeyMapping.Key))
			{
				KeyState->RawValue = FVector(0.f, 0.f, 0.f);
				KeyState->bDown = false;
				KeyState->bDownPrevious = false;
				KeyState->LastUpDownTransitionTime = TimeSeconds;
			}

			bIsFlushingInputThisFrame = true;
		}
	}
}
