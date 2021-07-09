#include "pch.h"

#include "base.h"
#include "Jig.h"
#include "DisplayEnt.h"


#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(DisplayEnt)
#endif

ZcDbObjectIdArray arr;

// ��ȡ������
AcGePoint3d getSamplePoint(AcDbObjectId pEntId) {
	AcDbCurve* pCrv = nullptr;
	AcGeCurve3d* pGeCrv = nullptr;
	acdbOpenAcDbEntity((AcDbEntity*&)pCrv, pEntId, AcDb::kForRead);
	if (pCrv)
	{
		pCrv->getAcGeCurve(pGeCrv);//��AcDbCurve��ȡAcGeCurve
		pCrv->close();
	}
	AcGePoint3dArray pts;
	pGeCrv->getSamplePoints(3, pts);//������ȡ��
	delete pGeCrv;//����Ҫdelete��
	pGeCrv = nullptr;

	return pts[1];
}

// ��ʾʵ��
void show(AcDbEntity* pEnt) {
	AcDbBlockTable* pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForWrite);

	AcDbBlockTableRecord* pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	pBlockTable->close();
	pBlockTableRecord->appendAcDbEntity(pEnt);
	pBlockTableRecord->close();
}

// ѡ��Ŵ󾵲�չʾ
void select() {
	Zcad::ErrorStatus err;

	// ��ʼ��Ҫ���Ƶ���һ���ĵ���ʵ��id����
	arr = ZcDbObjectIdArray();

	// ʹ��jig������ʾ��Χ
	Jig* jig = new Jig();
	DisplayEnt* displayEnt = new DisplayEnt();
	jig->startJig(displayEnt);

	show(displayEnt);
	AcDbObjectId displayEntId = displayEnt->objectId();

	displayEnt->close();

	// ��ȡ�Ŵ���
	ads_real num;
	if (acedGetReal(_T("������Ŵ���: \n"), &num) != RTNORM) { acutPrintf(_T("�Ŵ�����Ч \n")); return; }

	// ѡ����Ϊ�Ŵ󾵵�ʵ��
	AcDbEntity* pEnt;
	AcDbObjectId pEntId;
	ads_name en;
	ads_point pt;

	if (acedEntSel(_T("��ѡ��һ��Բ��\n"), en, pt) != RTNORM) { acutPrintf(_T("δѡ�� \n")); return; }

	acdbGetObjectId(pEntId, en);
	acdbOpenObject(pEnt, pEntId, AcDb::kForRead);

	arr.append(pEntId);

	// ���Ŵ�ת��ΪԲ������ȡ���ĺͰ뾶
	AcDbCircle* circle = (AcDbCircle*)pEnt;
	AcGePoint3d center = circle->center();
	double radius = circle->radius();

	AcDbExtents ext;
	pEnt->getGeomExtents(ext);

	// ���÷Ŵ���
	AcGeMatrix3d mat;
	mat.setToScaling(num);

	// Ҫ�Ŵ���ʾ��ʵ��id����
	AcDbObjectIdArray objIdArr;

	// ��ȡʵ���ѡ������ʵ��
	ads_name ss;
	acedSSGet(_T("C"), asDblArray(ext.minPoint()), asDblArray(ext.maxPoint()), NULL, ss);
	Adesk::Int32 len = 0;
	acedSSLength(ss, &len);

	// ������ѡ��ʵ��
	for (auto i = 0; i < len; i++) {
		// ��ȡʵ��id
		ads_name en1;
		acedSSName(ss, i, en1);
		AcDbObjectId objId1;
		auto err = acdbGetObjectId(objId1, en1);

		arr.append(objId1);

		AcDbEntity* pEnt1;
		if (acdbOpenAcDbEntity(pEnt1, objId1, AcDb::kForWrite) == Zcad::ErrorStatus::eOk) {
			// ��ȡ����
			ZcGePoint3dArray points;
			err = pEnt->intersectWith(pEnt1, ZcDb::Intersect::kOnBothOperands, points);
			acutPrintf(_T("��ȡ���� err: %d, points length: %d \n"), err, points.length());

			// ���
			if (pEnt1->isKindOf(AcDbCurve::desc())) {
				acutPrintf(_T("��ʼ��� \n"));

				AcDbCurve* pCurve;
				AcDbVoidPtrArray curveSegments;

				pCurve = AcDbCurve::cast(pEnt1);

				// ��ȡ����
				double param;
				double* params = new double[points.length()];
				for (auto y = 0; y < points.length(); y++) {
					pCurve->getParamAtPoint(points[y], param);
					params[y] = param;
				}

				// ����
				sort(params, params + points.length());

				// ת��Ϊ ZcGeDoubleArray
				ZcGeDoubleArray ZcGeparams;
				for (auto z = 0; z < points.length(); z++) {
					ZcGeparams.append(params[z]);
				}

				// ���
				pCurve->getSplitCurves(ZcGeparams, curveSegments);
				acutPrintf(_T("�����ɣ����ȣ� %d \n"), curveSegments.logicalLength());

				// ���û��ϣ�˵������ͼ�ζ���ʵ���У���������ͼ��
				if (curveSegments.logicalLength() == 0) {
					acutPrintf(_T("����ȫ�� \n"));
					// ��¡һ����ʵ�����Ŵ���ʾ
					AcDbEntity* pEnt2 = AcDbEntity::cast(pEnt1->clone());

					show(pEnt2);
					AcDbObjectId objId2 = pEnt2->objectId();
					pEnt2->transformBy(mat);
					pEnt2->close();
					// ��ӵ�����ʾ��objidarr��
					objIdArr.append(objId2);
				}
				else {
					acutPrintf(_T("\n ����ÿ��"));

					for (auto k = 0; k < curveSegments.logicalLength(); k++) {
						// �Ѵ�ϳ���������ת��Ϊʵ��
						AcDbEntity* pEnt2 = (AcDbEntity*)curveSegments[k];

						show(pEnt2);
						AcDbObjectId objId2 = pEnt2->objectId();
						pEnt2->close();

						// ��ʵ�����
						AcGePoint3d point = getSamplePoint(objId2);

						// �жϲ������Ƿ���Բ��
						double distance = pow((center.x - point.x), 2) + pow((center.y - point.y), 2);
						if (distance <= pow(radius, 2)) {
							// ��Բ�ڣ��Ŵ���ӵ�����ʾ������
							acdbOpenAcDbEntity(pEnt2, objId2, AcDb::kForWrite);

							pEnt2->transformBy(mat);
							objIdArr.append(objId2);
							pEnt2->close();
						}
					}
				}
				pEnt1->close();
			}
		}
	}

	// �ѷŴ����ʾʵ�峤��
	acutPrintf(_T("\n objIdArr���� :%d"), objIdArr.length());
	err = acdbOpenAcDbEntity((ZcDbEntity*&)displayEnt, displayEntId, AcDb::kForWrite);
	acutPrintf(_T("\n ��displayEnt err :%d"), err);

	// ����ʵ��id�ͷŴ�ʵ��Ļ���
	displayEnt->setObjIdArr(objIdArr);
	displayEnt->setOriCenter(AcGePoint3d(center.x * num, center.y * num, 0));

	displayEnt->close();

	arr.append(displayEntId);

	pEnt->close();
	acedSSFree(ss);
}

// �ĵ��临��
void clone()
{
	Zcad::ErrorStatus err;
	acDocManager->lockDocument(curDoc());

	ZcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcApDocManager* pDocManager = acDocManager;
	AcApDocumentIterator* iter = acDocManager->newAcApDocumentIterator();
	if (iter != NULL)
	{
		AcApDocument* tmpDoc = NULL;

		for (; !iter->done(); iter->step()) //�õ������������ĵ�
		{
			tmpDoc = iter->document();
			if (tmpDoc != NULL)
			{
				acDocManager->lockDocument(tmpDoc);

				// �½�һ�����ݿ�������
				AcDbDatabase* newpDb = new AcDbDatabase();

				// ��ȡid
				AcDbBlockTable* newpBT = nullptr;
				newpDb->getBlockTable(newpBT, AcDb::kForWrite);//�����ݿ��ȡ���
				AcDbBlockTableRecord* newpBTR = nullptr;
				newpBT->getAt(ACDB_MODEL_SPACE, newpBTR, AcDb::kForWrite);//�ӿ���ȡģ�Ϳռ�Ŀ���¼ 
				newpBT->close();//��������Ϲر�
				AcDbIdMapping idMap;
				idMap.setDestDb(newpDb);
				auto id = newpBTR->id();
				newpBTR->close();

				// ����
				err = pDb->wblockCloneObjects(arr, id, idMap, AcDb::DuplicateRecordCloning::kDrcIgnore);
				acutPrintf(_T("wblockCloneObjects, err: %d \n"), err);

				AcDbIdMappingIter iterM(idMap);

				for (iterM.start(); !iterM.done(); iterM.next())
				{
					AcDbIdPair idPair;
					iterM.getMap(idPair);
					if (!idPair.isCloned()) {
						continue;
					}
				}

				acDocManager->unlockDocument(tmpDoc);

				// ����
				err = newpDb->saveAs(_T("c:/code/aaa.dwg"));
				acutPrintf(_T("save file, err: %d \n"), err);
				delete newpDb;
				newpDb = NULL;
			}
		}
		delete iter; //����������Ҫdelete
		iter = NULL;
	}
	acDocManager->unlockDocument(curDoc());
}

void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("ss"), _T("ss"), ACRX_CMD_MODAL, select);
	acedRegCmds->addCommand(cmd_group_name, _T("cc"), _T("cc"), ACRX_CMD_SESSION, clone);
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
		DisplayEnt::rxInit();//ע���Զ���ʵ��
		acrxBuildClassHierarchy();//����������ʱ����
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
