// Kakanu Portfolio Project


#include "Animation/AnimInstances/PUBGLinkedAnimLayer.h"
#include "Animation/AnimInstances/PUBGCharacterAnimInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGCharacterAnimInstance)

UPUBGCharacterAnimInstance* UPUBGLinkedAnimLayer::GetCharacterAnimInstance() const
{
	return Cast<UPUBGCharacterAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
