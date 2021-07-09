#include "pch.h"

#include "base.h"

#include"AsdkCircleTemperature.h"
#include"AsdkDefaultTemperature.h"
#include"AsdkEntTemperature.h"
#include"AsdkRegionTemperature.h"
#include"AsdkLineTemperature.h"
#include"AsdkPolylineTemperature.h"


AsdkDefaultTemperature* pDefaultTemp;
AsdkRegionTemperature* pRegionTemp;
AsdkCircleTemperature* pCircleTemp;
AsdkLineTemperature* pLineTemp;
AsdkPolylineTemperature* pPolylineTemp;

void energy()
{
	AcDbEntity* pEnt;
	AcDbObjectId pEntId;
	ads_name en;
	ads_point pt;

	if (acedEntSel(_T("\n��ѡ��һ��ʵ�壺"), en, pt) != RTNORM)
	{
		acutPrintf(_T("\nδѡ��"));
		return;
	}

	acdbGetObjectId(pEntId, en);
	acdbOpenObject(pEnt, pEntId, AcDb::kForRead);


	// ����Э����չ��ĺ���
	double eTemp = ACRX_PE_PTR(pEnt, AsdkEntTemperature)->reflectedEnergy(pEnt);

	acutPrintf(L"\nEnergy=%f\n", eTemp);

	return;
}

void lineCenter() {
	ZcGePoint3d center;
	AcDbEntity* pLine;
	AcDbObjectId pLineId;
	ads_name en;
	ads_point pt;

	while (true) {
		if (acedEntSel(_T("\n��ѡ��һ���ߣ���������հ״�������"), en, pt) != RTNORM)
		{
			acutPrintf(_T("\n����"));
			break;
		}

		acdbGetObjectId(pLineId, en);
		acdbOpenObject(pLine, pLineId, AcDb::kForRead);

		// ����Э����չ��ĺ���
		ZcGePoint3d tmp = ACRX_PE_PTR(pLine, AsdkEntTemperature)->getCenter(pLine);

		center.x = (center.x + tmp.x) / 2;
		center.y = (center.y + tmp.y) / 2;
		center.z = (center.z + tmp.z) / 2;
	}

	acutPrintf(_T("\n center x: %f, y: %f, z:%f"), center.x, center.y, center.z);
}

void circleCenter() {
	AcDbEntity* pCircle;
	AcDbObjectId pCircleId;
	ads_name en;
	ads_point pt;

	if (acedEntSel(_T("\n��ѡ��һ��Բ��"), en, pt) != RTNORM)
	{
		acutPrintf(_T("\nδѡ��"));
		return;
	}

	acdbGetObjectId(pCircleId, en);
	acdbOpenObject(pCircle, pCircleId, AcDb::kForRead);

	// ����Э����չ��ĺ���
	ZcGePoint3d center = ACRX_PE_PTR(pCircle, AsdkEntTemperature)->getCenter(pCircle);

	pCircle->close();

	acutPrintf(_T("\n center x: %f, y: %f, z:%f"), center.x, center.y, center.z);

}

void polylineCenter() {
	AcDbEntity* pPolyline;
	AcDbObjectId pPolylineId;
	ads_name en;
	ads_point pt;

	if (acedEntSel(_T("\n��ѡ��һ������ߣ�"), en, pt) != RTNORM)
	{
		acutPrintf(_T("\nδѡ��"));
		return;
	}

	acdbGetObjectId(pPolylineId, en);
	acdbOpenObject(pPolyline, pPolylineId, AcDb::kForRead);

	// ����Э����չ��ĺ���
	ZcGePoint3d center = ACRX_PE_PTR(pPolyline, AsdkEntTemperature)->getCenter(pPolyline);

	pPolyline->close();

	acutPrintf(_T("\n center x: %f, y: %f, z:%f"), center.x, center.y, center.z);
}

void init()
{
	acutPrintf(_T("\n Init"));

	pDefaultTemp = new AsdkDefaultTemperature();
	pRegionTemp = new AsdkRegionTemperature();
	pCircleTemp = new AsdkCircleTemperature();
	pLineTemp = new AsdkLineTemperature();
	pPolylineTemp = new AsdkPolylineTemperature();

	AcDbEntity::desc()->addX(AsdkEntTemperature::desc(), pDefaultTemp);
	AcDbRegion::desc()->addX(AsdkEntTemperature::desc(), pRegionTemp);
	AcDbCircle::desc()->addX(AsdkEntTemperature::desc(), pCircleTemp);
	AcDbLine::desc()->addX(AsdkEntTemperature::desc(), pLineTemp);
	AcDbPolyline::desc()->addX(AsdkEntTemperature::desc(), pPolylineTemp);
}

void remove() {
	// ע��
	AcDbEntity::desc()->delX(AsdkEntTemperature::desc());
	delete pDefaultTemp;
	AcDbRegion::desc()->delX(AsdkEntTemperature::desc());
	delete pRegionTemp;
	AcDbCircle::desc()->delX(AsdkEntTemperature::desc());
	delete pCircleTemp;
	AcDbLine::desc()->delX(AsdkEntTemperature::desc());
	delete pLineTemp;
	AcDbPolyline::desc()->delX(AsdkEntTemperature::desc());
	delete pPolylineTemp;
}


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("init"), _T("init"), ACRX_CMD_MODAL, init);
	acedRegCmds->addCommand(cmd_group_name, _T("remove"), _T("remove"), ACRX_CMD_MODAL, remove);
	acedRegCmds->addCommand(cmd_group_name, _T("energy"), _T("energy"), ACRX_CMD_MODAL, energy);

	acedRegCmds->addCommand(cmd_group_name, _T("lineCenter"), _T("lineCenter"), ACRX_CMD_MODAL, lineCenter);
	acedRegCmds->addCommand(cmd_group_name, _T("circleCenter"), _T("circleCenter"), ACRX_CMD_MODAL, circleCenter);
	acedRegCmds->addCommand(cmd_group_name, _T("polylineCenter"), _T("polylineCenter"), ACRX_CMD_MODAL, polylineCenter);
}

void unloadapp()
{
	acedRegCmds->removeGroup(cmd_group_name);
}

extern "C" AcRx::AppRetCode zcrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
	switch (msg)
	{
	case AcRx::kInitAppMsg:
	{
		acrxDynamicLinker->unlockApplication(appId);
		acrxDynamicLinker->registerAppMDIAware(appId);
		initapp();
		AsdkEntTemperature::rxInit();
		acrxBuildClassHierarchy();

	}
	break;
	case AcRx::kUnloadAppMsg:
	{
		unloadapp();
		deleteAcRxClass(AsdkEntTemperature::desc());
		//remove();
	}
	break;

	default:
		break;
	}
	return AcRx::kRetOK;
}



#ifdef _WIN64
#pragma comment(linker, "/export:zcrxEntryPoint,PRIVATE")
#pragma comment(linker, "/export:zcrxGetApiVersion,PRIVATE")
#else // WIN32
#pragma comment(linker, "/export:_zcrxEntryPoint,PRIVATE")
#pragma comment(linker, "/export:_zcrxGetApiVersion,PRIVATE")
#endif

