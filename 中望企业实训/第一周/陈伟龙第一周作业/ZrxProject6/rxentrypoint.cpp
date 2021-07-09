#include "pch.h"

#include "tchar.h"
#include <dbapserv.h>
#include <aced.h>
#include <dbxrecrd.h>
#include <rxregsvc.h>
#include <dbmain.h>

void helloworld()
{
	//�����ݿ��ȡ���
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForWrite);
	AcDbObjectId btrId;
	AcDbDictionary* pNOD = nullptr;
	acdbOpenAcDbObject((AcDbObject*&)pNOD, pDb->namedObjectsDictionaryId(), AcDb::kForWrite);//��ȡ�����ʵ��Id����

	//�ж��Ƿ�ɹ���ִ��ҵ��
	if (pBT)
	{
		auto pBtr = new AcDbBlockTableRecord();
		pBtr->setName(_T("*U"));
		pBT->add(btrId, pBtr);

		auto pDict = new AcDbDictionary();//���������ʵ�
		AcDbObjectId dictId;
		pNOD->setAt(_T("MyDict"), pDict, dictId);//���������ʵ�����ƺ�ID

		//forѭ����Բ
		for (int i = 1; i <= 255; i++) {
			auto pCir = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, i);
			pCir->setColorIndex(i);
			pBtr->appendAcDbEntity(pCir);
			
			char str[10];
			for (int i = 0; i < 10; i++) {
				str[i] = '\0';
			}
			_itoa(i, str, 10);


			//�����ʵ��¼�����
			if (pDict) {
				if (!pDict->has(AcString(str)))
				{
					AcDbHandle hd;//�õ��˾��
					pCir->getAcDbHandle(hd);
					ACHAR strHD[16];
					hd.getIntoAsciiBuffer(strHD, 16);
					acutPrintf(_T("\n Handle: %s"), strHD);
					auto pXrec = new AcDbXrecord();//�����ݴ���xrecord��
					resbuf* pRb = acutBuildList(AcDb::kDxfText, (_T("\n Handle is %s."), strHD), RTNONE);
					pXrec->setFromRbChain(*pRb);
					AcDbObjectId xrecId;
					pDict->setAt(AcString(str), pXrec, xrecId);//��xrecord�浽�ʵ����������id
					pXrec->close();//�ǵ�Ҫclose
					acutRelRb(pRb);//resbuf�����Ƶ�xrecord��Ҫɾ��
					pRb = nullptr;
				}
			}

			//��չ�ʵ��¼��ɫ����
			if (pCir->extensionDictionary().isNull())//�жϸö����Ƿ��Ѿ�����չ�ʵ�
			{
				pCir->createExtensionDictionary();//û�оʹ���
			}
			//֮����ǳ���ʵ����
			AcDbDictionary* pXDict = nullptr;
			acdbOpenAcDbObject((AcDbObject*&)pXDict, pCir->extensionDictionary(), AcDb::kForWrite);
			if (pXDict)
			{
				auto pXrec1 = new AcDbXrecord();//�����ݴ���xrecord��
				resbuf* pRb = acutBuildList(AcDb::kDxfText, (_T("\n ColorIndex is %s."),str), RTNONE);
				pXrec1->setFromRbChain(*pRb);
				AcDbObjectId xrecId;
				pXDict->setAt(AcString(str), pXrec1, xrecId);//��xrecord�浽�ʵ����������id
				pXrec1->close();//�ǵ�Ҫclose
				acutRelRb(pRb);//resbuf�����Ƶ�xrecord��Ҫɾ��
				pRb = nullptr;
				pXDict->close();
			}
			pCir->close();
		}
		pDict->close();
		acutPrintf(_T("\n����Բ�ɹ�"));
		pBtr->close();
	}

	//�Ѷ�����ʾ�ڻ�����
	AcDbBlockTableRecord* pBTR = nullptr;
	pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);
	
	if (pBTR)
	{
		auto pRef = new AcDbBlockReference(AcGePoint3d::kOrigin, btrId);
		pBTR->appendAcDbEntity(pRef);
		pRef->close();
		pBTR->close();
	}
	pBT->close();
}


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("helloworld"), _T("helloworld"), ACRX_CMD_MODAL, helloworld);
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

