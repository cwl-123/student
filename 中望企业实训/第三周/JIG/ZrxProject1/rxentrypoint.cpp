#include "pch.h"

#include "tchar.h"
#include <aced.h>
#include <rxregsvc.h>
#include "dbapserv.h"
#include "Jig.h"

void JIG()
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
	
			//�������ݿ����
			Jig *jigExample = new Jig();
			SampleCustEnt *sample_2 = new SampleCustEnt();
			jigExample->startJig(sample_2);
	
			acutPrintf(_T("\n%f, %f"), sample_2->center().x, sample_2->center().y);
	
			pBTR->appendZcDbEntity(sample_2);//��ʾ��ͼֽ��
	
			sample_2->close();
			pBTR->close();
		}
	}
}


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("JIG"), _T("JIG"), ACRX_CMD_MODAL, JIG);
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
		}
		break;
		case AcRx::kUnloadAppMsg:
		{
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

