// Kakanu Portfolio Project


#include "Physics/PUBGPhysicsFunctionLibrary.h"

bool UPUBGPhysicsFunctionLibrary::SphereIntersectsBox(const FVector& SphereCenter, float SphereRadius, const FBox& Box)
{
	if (Box.IsValid == false)
	{
		return false;
	}

	// 원의 중심과 가장가까운 벡터를 반환
	FVector ClosestPoint = Box.GetClosestPointTo(SphereCenter);

	// 원의 중심과의 제곱거리를 구한다.
	float DistanceSquared = FVector::DistSquared(SphereCenter, ClosestPoint);

	// 반지름보다 같거나 작으면 충돌발생
	return DistanceSquared <= (SphereRadius * SphereRadius);
}
