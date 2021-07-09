#pragma once
#include "pch.h"
#include "tchar.h"
#include "acadstrc.h"
#include "dbproxy.h"
#include"rxboiler.h"
#include"dbappgrip.h"
#include"dbidmap.h"
#include"dbcfilrs.h"

#include "SampleCustEnt.h"

//version����
Adesk::UInt32 SampleCustEnt::kCurrentVersionNumber = 1;

//����е㣬����������
AcGePoint3d m_left;
AcGePoint3d m_right;
AcGePoint3d m_above;
AcGePoint3d m_bottom;
AcGePoint3d m;

//ע�ʹ��룬����Ч
ACRX_DXF_DEFINE_MEMBERS(
	SampleCustEnt, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, SAMPLECUSTENT,
	ARXPROJECT3APP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)


//-----------------------------------------------------------------------------
SampleCustEnt::SampleCustEnt() : AcDbEntity() {
	m_radius = 0;
}

SampleCustEnt::SampleCustEnt(AcGePoint3d center, double radius, AcDbObjectId id)
{
	m_center = center;
	m_radius = radius;
	m_srcId = id;
}

// ͨ��id��ȡ��Բ��Ȼ���½��Զ���ʵ��
SampleCustEnt::SampleCustEnt(const AcDbObjectId& circleId)
{
	m_srcId = circleId;
	AcDbCircle *circle = NULL;
	acdbOpenAcDbObject((AcDbObject *&)circle, m_srcId, AcDb::kForWrite);
	double radius = circle->radius();
	m_center = circle->center();
	m_radius = radius + 100;

	circle->close();
}

SampleCustEnt::~SampleCustEnt() {
}


void SampleCustEnt::setCenter(AcGePoint3d mpcenter) {
	assertReadEnabled();
	m_center = mpcenter;
}

void SampleCustEnt::setRadius(double mpradius) {
	assertReadEnabled();
	m_radius = mpradius;
}

AcGePoint3d SampleCustEnt::center() {
	assertReadEnabled();
	return m_center;
}

//�ӿ��޹���ʾʵ��
Adesk::Boolean SampleCustEnt::subWorldDraw(AcGiWorldDraw *mode) {
	assertReadEnabled();
	mode->geometry().circle(m_center, m_radius, AcGeVector3d::kZAxis);
	return (AcDbEntity::subWorldDraw(mode));
}

//�ӿ���ص���ʾʵ��
void SampleCustEnt::subViewportDraw(AcGiViewportDraw * mode)
{
	assertReadEnabled();
	mode->geometry().circle(m_center, m_radius, AcGeVector3d::kZAxis);
	AcDbEntity::subViewportDraw(mode);
}

//������ʾ��ص�����
Adesk::UInt32 SampleCustEnt::subSetAttributes(AcGiDrawableTraits * traits)
{
	assertReadEnabled();
	return (AcDbEntity::subSetAttributes(traits));
	// return Adesk::UInt32();
}

/////////////////subTransformBy ͨ������任��ʵ��ƽ����ת����////////////////
Acad::ErrorStatus SampleCustEnt::subTransformBy(const AcGeMatrix3d& xform) {
	assertWriteEnabled();
	m_center = m_center.transformBy(xform);
	return Acad::eOk;
}


/////////////////subGetGripPoints ��CAD���ͼе㣨λ�ã�////////////////
Acad::ErrorStatus SampleCustEnt::subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds) const {
	//const :˵��������"ֻ��"�������������޸ĳ�Ա������ֵ��������뱨��
	assertReadEnabled();//��Ȩ

	m_left[X] = m_center[X] - m_radius;
	m_left[Y] = m_center[Y];
	m_right[X] = m_center[X] + m_radius;
	m_right[Y] = m_center[Y];
	m_above[X] = m_center[X];
	m_above[Y] = m_center[Y] + m_radius;
	m_bottom[X] = m_center[X];
	m_bottom[Y] = m_center[Y] - m_radius;

	gripPoints.append(m_center);
	gripPoints.append(m_right);
	gripPoints.append(m_left);
	gripPoints.append(m_above);
	gripPoints.append(m_bottom);
	return Acad::eOk;
}

/////////////////subMoveGripPointsAt ��קʱ����е��ƶ���Ϊ////////////////
Acad::ErrorStatus SampleCustEnt::subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d& offset) {
	assertWriteEnabled();

	switch (indices[0]) {
	case(0):
		m_center += offset;
		break;
	case(1):
		m = m_right + offset;
		m_radius = m_center.distanceTo(m);
		break;
	case(2):
		m = m_left + offset;
		m_radius = m_center.distanceTo(m);
		break;
	case(3):
		m = m_above + offset;
		m_radius = m_center.distanceTo(m);
		break;
	case(4):
		m = m_bottom + offset;
		m_radius = m_center.distanceTo(m);
		break;
	}
	return Acad::eOk;
}


/////////////////subGetOsnapPoints �е㲶׽////////////////
Acad::ErrorStatus SampleCustEnt::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d& pickPoint,
	const AcGePoint3d& lastPoint,
	const AcGeMatrix3d& viewXform,
	AcGePoint3dArray& snapPoints,
	AcDbIntArray & geomIds
) const {
	assertReadEnabled();
	AcDbCircle circle;
	circle.setCenter(m_center);
	circle.setRadius(m_radius);
	return circle.getOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
}


/////////////////dwgOutFields �Զ���ʵ��Ķ�ȡ////////////////
Acad::ErrorStatus SampleCustEnt::dwgOutFields(AcDbDwgFiler *pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(SampleCustEnt::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	pFiler->writePoint3d(m_center);
	pFiler->writeDouble(m_radius);
	AcDbHardPointerId id(m_srcId);
	pFiler->writeHardPointerId(id);

	return (pFiler->filerStatus());
}

/////////////////dwgInFields �Զ���ʵ��ı���///////////////
Acad::ErrorStatus SampleCustEnt::dwgInFields(AcDbDwgFiler *pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > SampleCustEnt::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	if (version < SampleCustEnt::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//----- Read params
	pFiler->readPoint3d(&m_center);
	pFiler->readDouble(&m_radius);
	AcDbHardPointerId id;
	pFiler->readHardPointerId(&id);
	return (pFiler->filerStatus());
}



Acad::ErrorStatus
SampleCustEnt::subDeepClone(AcDbObject*    pOwner,
	AcDbObject*&   pClonedObject,
	AcDbIdMapping& idMap,
	Adesk::Boolean isPrimary) const
{
	//��������Ӧ�ڴ�������ʱ�����ΪNULL�������Է���һ������Ҫ���ÿ�
	//������һû���Ƴɹ��ó�ȥ����Ұָ��
	pClonedObject = NULL;

	//������ʵ���Ѿ�������Id Map�����Ѿ������ƹ��ˣ���Id Map���Ѿ�������ɶԵ�Id�ˣ���
	//�Ͳ����ٸ����ˣ�ֱ�ӷ���
	//˼����ʲô����»ᷢ�������£�
	bool isPrim = false;
	if (isPrimary)//isPrimary��ʾ��ǰ�����ƵĶ����Ǳ��������ƵĻ�����Ϊ���й�ϵ�������Ƶ�
		isPrim = true;
	AcDbIdPair idPair(objectId(), (AcDbObjectId)NULL,
		false, isPrim);
	if (idMap.compute(idPair) && (idPair.value() != NULL))
		return Acad::eOk;

	//����һ��������ͬ�Ķ���
	auto *pClone = (SampleCustEnt*)isA()->create();
	if (pClone != NULL)
		pClonedObject = pClone;    //������Ǹ��Ƴ����Ķ��󣬿����ȴ浽����ֵ��
	else
		return Acad::eOutOfMemory;

	AcDbDeepCloneFiler filer;
	dwgOut(&filer);//ͨ��dwgOut��ȡ�����������
	filer.seek(0L, AcDb::kSeekFromStart);//���������ļ���ָ�룬���������dwgIn
	pClone->dwgIn(&filer);

	//����Ҫ����owner�����������
	//1. ���owner�ǿ���¼���Ǿ�ֱ�ӵ���appendAcDbEntity�ӽ�ȥ
	//2. ���owner�Ǵʵ䣬��ʹ��setAt�ӵ��ʵ���������û�����֣�
	//3. �����ǰ��������Ϊ���ù�ϵ�������Ƶģ���ʹ��addAcDbObject()
	//�����ӵ�owner�����ݿ��ﲢֱ����������owner
	bool bOwnerXlated = false;//��ʾowner��id�Ƿ��Ѿ�ת��
	if (isPrimary)
	{
		AcDbBlockTableRecord *pBTR =
			AcDbBlockTableRecord::cast(pOwner);
		if (pBTR != NULL)
		{
			pBTR->appendAcDbEntity(pClone);
			bOwnerXlated = true;
		}
		else
		{//����������������֧��ʵ�ǲ����ܽ����ģ�����ֻ���ݴ�
			pOwner->database()->addAcDbObject(pClone);
		}
	}
	else {
		pOwner->database()->addAcDbObject(pClone);
		pClone->setOwnerId(pOwner->objectId());
		bOwnerXlated = true;
	}


	//ÿһ����deepclone�����ﴴ���Ķ���Ҫ�ڼ���Id Mapǰ����setAcDbObjectIdsInFlux()����
	pClone->setAcDbObjectIdsInFlux();
	pClone->disableUndoRecording(true);//pClone�����Ѿ������ݿ��ͨ�������ʽ���Խ��õ�����undo


	//��Դ����͸��ƶ���ճ�һ�ԣ��ŵ�Id Map��
	idPair.setValue(pClonedObject->objectId());
	idPair.setIsCloned(Adesk::kTrue);
	idPair.setIsOwnerXlated(bOwnerXlated);
	idMap.assign(idPair);

	//����Դ��������й�ϵ���������ǵ�deepClone����
	AcDbObjectId id;
	while (filer.getNextOwnedObject(id)) {

		AcDbObject *pSubObject;
		AcDbObject *pClonedSubObject;

		//���й�ϵ�Ķ���id��һ��������Ч�ģ���Ҫ�����
		if (id == NULL)
			continue;

		//ע����Ϊ��Щ�������������й�ϵ�������Ƶģ�����deepClone�����һ������(isPrimary)��Ҫ��Ϊfalse
		acdbOpenAcDbObject(pSubObject, id, AcDb::kForRead);
		pClonedSubObject = NULL;
		pSubObject->deepClone(pClonedObject,
			pClonedSubObject,
			idMap, Adesk::kFalse);

		//��ĳ���������(����ĳЩ����µ�Ԥ��)������ʵ����ֻ�����ݡ��ƶ�����
		//pSubObject��pClonedSubObject��ָ��ͬһ���������ʱ��Ͳ���close pSubObject��
		if (pSubObject != pClonedSubObject)
			pSubObject->close();

		//����������ʧ��pClonedSubObject�ͻ��ǿ�ָ�룬��Ҫ���
		if (pClonedSubObject != NULL)
			pClonedSubObject->close();
	}

	//ע���ʱpClonedObject��û�б��رյģ����ɵ�����������
	return Acad::eOk;
}

Acad::ErrorStatus SampleCustEnt::subWblockClone(
	AcRxObject*    pOwner,
	AcDbObject*&   pClonedObject,
	AcDbIdMapping& idMap,
	Adesk::Boolean isPrimary) const
{
	//�ǵ��Ȱѷ���ֵ�ÿ�
	pClonedObject = NULL;

	//���Դ���ݿ��Ŀ�����ݿ���ͬһ��������ʡ�Դ󲿷ֲ��裬ֱ�ӵ��û����wblockClone�Ϳ�����
	AcDbDatabase *pDest, *pOrig;
	idMap.destDb(pDest);
	idMap.origDb(pOrig);
	if (pDest == pOrig)
		return AcDbEntity::wblockClone(pOwner, pClonedObject,
			idMap, isPrimary);

	//��������ڽ���xbind����������xbindֻ����ģ�Ϳռ�Ķ������Դ������ͼֽ�ռ䣬��ֱ�ӷ���
	AcDbObjectId pspace;
	AcDbBlockTable *pTable;
	database()->getSymbolTable(pTable, AcDb::kForRead);
	pTable->getAt(ACDB_PAPER_SPACE, pspace);
	pTable->close();
	if (idMap.deepCloneContext() == AcDb::kDcXrefBind
		&& ownerId() == pspace)
		return Acad::eOk;

	//���Դ�����Ѿ������ƹ��ˣ�ֱ�ӷ���
	bool isPrim = false;
	if (isPrimary)
		isPrim = true;
	AcDbIdPair idPair(objectId(), (AcDbObjectId)NULL,
		false, isPrim);
	if (idMap.compute(idPair) && (idPair.value() != NULL))
		return Acad::eOk;

	//Դ�����owner�п��������ݿ⣬Ҳ�п�����һ��AcDbObject��
	//���owner�����ݿ⣬��ʾ����������������Ӳָ���ϵ�������Ƶģ�������ʵ��owner����ת��׶α���ȷ����
	//��ʱpOwn��ֵ����NULL����Ϊһ����־
	AcDbObject   *pOwn = AcDbObject::cast(pOwner);
	AcDbDatabase *pDb = AcDbDatabase::cast(pOwner);
	if (pDb == NULL)
		pDb = pOwn->database();

	//����һ��ͬ���͵Ķ���
	auto *pClone = (SampleCustEnt*)isA()->create();
	if (pClone != NULL)
		pClonedObject = pClone;
	else
		return Acad::eOutOfMemory;

	//���owner�Ǹ��������������֪����������ʲô���Ϳ���ֱ�����ø��ƶ����owner
	//����Ͱ�Դ�����owner���ø����ƶ�����ת��׶ν���ת���������Դ������Ŀ�����ownerһ����Ҳ����Ϊת��׶�Ҫ�õ���һ����־
	//Ҫע�������Դ�����ownerҲ����Ҫ�����Ƶ�Ŀ������ݿ���ģ�����ת��׶λ����
	AcDbBlockTableRecord *pBTR = NULL;
	if (pOwn != NULL)
		pBTR = AcDbBlockTableRecord::cast(pOwn);
	if (pBTR != NULL && isPrimary) {
		pBTR->appendAcDbEntity(pClone);
	}
	else {
		pDb->addAcDbObject(pClonedObject);
	}

	//ͨ��dwgOut��dwgIn�������ݵ����ƶ�����
	AcDbWblockCloneFiler filer;
	dwgOut(&filer);
	filer.seek(0L, AcDb::kSeekFromStart);
	pClone->dwgIn(&filer);

	//���Դ�����븴�ƶ���Id Map��ע��AcDbIdPair���캯�������һ������isOwnerXlated�����ƶ����owner�Ƿ���Ҫת��
	//�������������ͨ�������pOwn�Ƿ�Ϊ�����жϵ�
	idMap.assign(AcDbIdPair(objectId(), pClonedObject->objectId(),
		Adesk::kTrue,
		isPrim, (Adesk::Boolean)(pOwn != NULL)));
	pClonedObject->setOwnerId((pOwn != NULL) ?
		pOwn->objectId() : ownerId());

	//��deepcloneһ����Ҫ��ÿ�����ƶ������setAcDbObjectIdsInFlux()
	pClone->setAcDbObjectIdsInFlux();

	//�ݹ鸴�����е�Ӳ���к�Ӳָ��
	AcDbObjectId id;
	while (filer.getNextHardObject(id)) {
		AcDbObject *pSubObject;
		AcDbObject *pClonedSubObject;

		//ĳЩ����»�������õĶ���Ϊ�գ���Ҫ����
		if (id == NULL)
			continue;

		//������ö�����������һ�����ݿ⣬����Ҫ����
		acdbOpenAcDbObject(pSubObject, id, AcDb::kForRead);
		if (pSubObject->database() != database()) {
			pSubObject->close();
			continue;
		}

		//���������������������������ö����owner���Ͱ����ݿ���Ϊowner����ȥ
		//�����deepcloneһ��������isPrimary����Ҫ��Ϊfalse
		pClonedSubObject = NULL;
		if (pSubObject->ownerId() == objectId()) {
			pSubObject->wblockClone(pClone,
				pClonedSubObject,
				idMap, Adesk::kFalse);
		}
		else {
			pSubObject->wblockClone(
				pClone->database(),
				pClonedSubObject,
				idMap, Adesk::kFalse);
		}
		pSubObject->close();

		//�������ʧ�ܣ�pClonedSubObject���ǿ�ָ�룬��Ҫ���
		if (pClonedSubObject != NULL)
			pClonedSubObject->close();
	}

	//��deepcloneһ�������ƶ����ɵ�������close
	return Acad::eOk;
}
