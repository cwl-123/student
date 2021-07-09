#include "pch.h"

#include "tchar.h"
#include <aced.h>
#include <rxregsvc.h>
#include "SampleCustEnt.h"

#include "dbmain.h"
#include <dbapserv.h>

//ע������ʱ����ʶ����
#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(SampleCustEnt)
#endif

void Entity()
{
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForWrite);//�����ݿ��ȡ���
	if (pBT)//�ж��Ƿ�ɹ�
	{
		AcDbBlockTableRecord* pBTR = nullptr;
		pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//�ӿ���ȡģ�Ϳռ�Ŀ���¼ 
		pBT->close();//��������Ϲر�
		if (pBTR)
		{
			acutPrintf(_T("\nhello world"));

			ads_name en;
			ads_point apt;
			acedEntSel(_T("\nѡ��һ��ʵ��"), en, apt);

			//�������ݿ����
			AcDbObjectId circleId;
			acdbGetObjectId(circleId, en);
			AcDbCircle *circle = NULL;
			acdbOpenAcDbObject((AcDbObject *&)circle, circleId, AcDb::kForWrite);

			double radius = circle->radius();
			acutPrintf(_T("\n%f"), radius);

			//acutPrintf(_T("\n Hello!"));
			AcGePoint3d original(0.0, 0.0, 0.0);
			SampleCustEnt *sample_1 = new SampleCustEnt(circle->center(), radius + 100);
			circle->close();

			//sample_1->setCenter(original);

			pBTR->appendZcDbEntity(sample_1);//��ʾ��ͼֽ��
			sample_1->close();
			pBTR->close();
		}
	}
}


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("Entity"), _T("Entity"), ACRX_CMD_MODAL, Entity);
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
		SampleCustEnt::rxInit();//ע���Զ���ʵ��
		acrxBuildClassHierarchy();//����������ʱ����
		initapp();
	}
	break;
	case AcRx::kUnloadAppMsg:
	{
		deleteAcRxClass(SampleCustEnt::desc());//ע���Զ���ʵ��
		unloadapp();
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
