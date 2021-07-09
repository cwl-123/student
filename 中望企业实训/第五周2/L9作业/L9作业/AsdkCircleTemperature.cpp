#include "pch.h"
#include "AsdkCircleTemperature.h"

AsdkCircleTemperature::AsdkCircleTemperature(void)
{
}


AsdkCircleTemperature::~AsdkCircleTemperature(void)
{
}


double AsdkCircleTemperature::reflectedEnergy(AcDbEntity* pEnt) const
{
	acutPrintf(_T("\n not implement AsdkCircleTemperature::reflectedEnergy"));

	return -1.0;
}

// ��ȡԲ������
ZcGePoint3d AsdkCircleTemperature::getCenter(AcDbEntity* pEnt) const {
	acutPrintf(_T("\n AsdkCircleTemperature::getCenter"));

	AcDbCircle* pCircle = AcDbCircle::cast(pEnt);
	if (pCircle == NULL) {
		acutPrintf(_T("\n��ѡ��һ��Բ"));
		return ZcGePoint3d();
	}
	ZcGePoint3d center = pCircle->center();
	pCircle->close();
	return center;
}
