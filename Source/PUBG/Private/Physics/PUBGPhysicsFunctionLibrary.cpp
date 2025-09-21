// Kakanu Portfolio Project


#include "Physics/PUBGPhysicsFunctionLibrary.h"

bool UPUBGPhysicsFunctionLibrary::SphereIntersectsBox(const FVector& SphereCenter, float SphereRadius, const FBox& Box)
{
	if (Box.IsValid == false)
	{
		return false;
	}

	// ���� �߽ɰ� ���尡��� ���͸� ��ȯ
	FVector ClosestPoint = Box.GetClosestPointTo(SphereCenter);

	// ���� �߽ɰ��� �����Ÿ��� ���Ѵ�.
	float DistanceSquared = FVector::DistSquared(SphereCenter, ClosestPoint);

	// ���������� ���ų� ������ �浹�߻�
	return DistanceSquared <= (SphereRadius * SphereRadius);
}
