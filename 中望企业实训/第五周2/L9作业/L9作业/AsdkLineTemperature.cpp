#include "pch.h"
#include "AsdkLineTemperature.h"

AsdkLineTemperature::AsdkLineTemperature(void)
{
}


AsdkLineTemperature::~AsdkLineTemperature(void)
{
}

double AsdkLineTemperature::reflectedEnergy(AcDbEntity* pEnt) const
{
	acutPrintf(_T("\n not implement AsdkLineTemperature::reflectedEnergy"));

	return -1.0;
}

// ��ȡֱ�ߵ�����
ZcGePoint3d AsdkLineTemperature::getCenter(AcDbEntity* pEnt) const {
	acutPrintf(_T("\n AsdkLineTemperature::getCenter"));

	AcDbLine* pLine = AcDbLine::cast(pEnt);
	if (pLine == NULL) {
		acutPrintf(_T("\n��ѡ��һ��ֱ��"));
		return ZcGePoint3d();
	}

	ZcGePoint3d start = pLine->startPoint();
	ZcGePoint3d end = pLine->endPoint();

	pLine->close();

	return ZcGePoint3d(
		(start.x + end.x) / 2,
		(start.y + end.y) / 2,
		(start.z + end.z) / 2
	);
}


