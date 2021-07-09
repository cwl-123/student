#include "pch.h"
#include "AsdkObjectToNotify.h"
#include"SampleCustEnt.h"

ACRX_DXF_DEFINE_MEMBERS(AsdkObjectToNotify, AcDbObject,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	0, ASDKOBJECTTONOTIFY, persreac);

void AsdkObjectToNotify::modified(const AcDbObject* pObj) {
	AcDbCircle *pCircle = AcDbCircle::cast(pObj);
	if (!pCircle) {
		acutPrintf(_T("��Ӧ�������׸��ŵ������ʵ���ϣ�Ҫע����"));
		return;//��Ӧ�������׸��ŵ������ʵ���ϣ�Ҫע����
	}
	SampleCustEnt *pCircle2 = nullptr; // �Զ���ʵ��
	if (acdbOpenAcDbEntity((AcDbEntity*&)pCircle2, mId, AcDb::kForWrite) == Acad::eOk) {
		acutPrintf(_T("\n���Զ���ʵ��"));

		AcGePoint3d center = pCircle->center();
		double radius = pCircle->radius();
		acutPrintf(_T("\n pCircle %f, %f"), center.x, center.y);
		acutPrintf(_T("\n pCircle2 %f, %f"), pCircle2->center().x, pCircle2->center().y);

		pCircle2->setCenter(center);
		acutPrintf(_T("\n pCircle2 %f, %f"), pCircle2->center().x, pCircle2->center().y);

		pCircle2->setRadius(radius + 100);

		pCircle2->recordGraphicsModified();
		pCircle2->draw();
		pCircle2->close();
	}
}

Acad::ErrorStatus AsdkObjectToNotify::dwgInFields(AcDbDwgFiler* filer) {
	assertWriteEnabled();
	AcDbObject::dwgInFields(filer);
	filer->readItem((AcDbSoftPointerId*)&mId);
	return filer->filerStatus();
}
Acad::ErrorStatus AsdkObjectToNotify::dwgOutFields(AcDbDwgFiler* filer) const {
	assertReadEnabled();
	AcDbObject::dwgOutFields(filer);
	filer->writeItem((AcDbSoftPointerId&)mId);
	return filer->filerStatus();
}

void AsdkObjectToNotify::erased(const AcDbObject* dbObj, ZSoft::Boolean bErasing) {

	SampleCustEnt *pCircle2 = nullptr; // �Զ���ʵ��
	if (acdbOpenAcDbEntity((AcDbEntity*&)pCircle2, mId, AcDb::kForWrite) == Acad::eOk) {
		acutPrintf(_T("\n���Զ���ʵ��"));
		pCircle2->erase(bErasing);
		pCircle2->close();
	}
}
