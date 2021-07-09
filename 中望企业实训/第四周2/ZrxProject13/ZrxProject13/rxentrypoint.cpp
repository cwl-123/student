#include "pch.h"

#include "tchar.h"
#include <aced.h>
#include <rxregsvc.h>
#include "acadstrc.h"
#include "dbproxy.h"
#include "rxboiler.h"
#include "dbidmap.h"
#include "dbcfilrs.h"
#include "dbmain.h"
#include <dbapserv.h>
#include "acestext.h"

#include "SampleCustEnt.h"

//ע������ʱ����ʶ����
#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(SampleCustEnt)
#endif

void helloworld()
{

	//acutPrintf(_T("\nHello World!"));
	acDocManager->lockDocument(curDoc());

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
			//acutPrintf(_T("\nhello world"));

			ads_name en;
			ads_point apt;
			acedEntSel(_T("\nѡ��һ��ʵ��"), en, apt);

			//�������ݿ����
			AcDbObjectId circleId;
			acdbGetObjectId(circleId, en);

			SampleCustEnt *sample_1 = new SampleCustEnt(circleId);
			AcDbObjectId sId;
			pBTR->appendZcDbEntity(sId, sample_1);//��ʾ��ͼֽ��
			sample_1->close();

			pBTR->close();

			AcApDocManager* pDocManager = acDocManager;
			AcApDocumentIterator* iter = acDocManager->newAcApDocumentIterator();
			if (iter != NULL)
			{

				//acutPrintf(_T("\niter is not null"));

				AcApDocument* tmpDoc = NULL;

				ZcDbObjectIdArray arr;

				arr.append(circleId);
				arr.append(sId);

				for (; !iter->done(); iter->step())
				{//�õ������������ĵ�
					//acutPrintf(_T("\niter for "));

					tmpDoc = iter->document();

					if (tmpDoc != NULL)
					{
						//acutPrintf(_T("\n tmpDoc is not null "));
						acDocManager->lockDocument(tmpDoc);

						// �½�һ�����ݿ�������
						AcDbDatabase* newpDb = new AcDbDatabase();

						// ��ȡid
						AcDbBlockTable* newpBT = nullptr;
						newpDb->getBlockTable(newpBT, AcDb::kForWrite);//�����ݿ��ȡ���
						AcDbBlockTableRecord* newpBTR = nullptr;
						auto err = newpBT->getAt(ACDB_MODEL_SPACE, newpBTR, AcDb::kForWrite);//�ӿ���ȡģ�Ϳռ�Ŀ���¼ 
						newpBT->close();//��������Ϲر�
						AcDbIdMapping idMap;
						idMap.setDestDb(newpDb);
						auto id = newpBTR->id();
						newpBT->close();
						newpBTR->close();

						// ����
						Acad::ErrorStatus status = pDb->wblockCloneObjects(arr, id, idMap, AcDb::DuplicateRecordCloning::kDrcIgnore);

						//acutPrintf(_T("\n status is: %s"), status);
						AcDbIdMappingIter iterM(idMap);

						for (iterM.start(); !iterM.done(); iterM.next())
						{
							//acutPrintf(_T("\n iterM for loop"));

							AcDbIdPair idPair;
							iterM.getMap(idPair);
							if (!idPair.isCloned()) { 
								//acutPrintf(_T("\n for continue"));
								continue;
							}
							//acutPrintf(_T("\bObjectId is: %Ld", idPair.value().asOldId()));
						}

						//acutPrintf(_T("\n out loop"));

						err = acDocManager->unlockDocument(tmpDoc);

						//acutPrintf(_T("\n out loop, err: %d", err));

						// ����
						err = newpDb->saveAs(_T("d:/aaa.dwg"));

						//acutPrintf(_T("\n save file, err: %d", err));
						delete newpDb;
						newpDb = NULL;
					}
				}

				delete iter; //����������Ҫdelete
				iter = NULL;
			}
		}
	}
	acDocManager->unlockDocument(curDoc());

}


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("ccc"), _T("ccc"), ACRX_CMD_SESSION, helloworld);
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
		SampleCustEnt::rxInit();
		acrxBuildClassHierarchy();//����������ʱ����
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

