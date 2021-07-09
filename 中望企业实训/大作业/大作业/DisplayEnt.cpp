#pragma once
#include "pch.h"

#include "base.h"
#include "DisplayEnt.h"

//version����
Adesk::UInt32 DisplayEnt::kCurrentVersionNumber = 1;

//����е㣬����������
AcGePoint3d d_left;
AcGePoint3d d_right;
AcGePoint3d d_above;
AcGePoint3d d_bottom;
AcGePoint3d d;

//ע�ʹ��룬����Ч
ACRX_DXF_DEFINE_MEMBERS(
	DisplayEnt, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, DisplayEnt,
	ARXPROJECT3APP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)


DisplayEnt::DisplayEnt(){}

//-----------------------------------------------------------------------------
DisplayEnt::DisplayEnt(AcGePoint3d center, double radius) : AcDbEntity()
{
	assertWriteEnabled();
	setCenter(center);
	setRadius(radius);
}

DisplayEnt::~DisplayEnt(){}

enum PartialUndoCode
{
	kRadius,
	kCenter,
};

void DisplayEnt::setCenter(AcGePoint3d mpcenter) {
	assertWriteEnabled(false);
	AcDbDwgFiler* pFiler = NULL;
	if ((pFiler = undoFiler()) != NULL) {
		acutPrintf(_T("\n set center undo filter"));
		undoFiler()->writeAddress(DisplayEnt::desc()); //����ʵ����
		undoFiler()->writeItem((Adesk::Int16)kCenter); //�������Ա��
		undoFiler()->writePoint3d(center);
	}

	center = mpcenter; //�ǵ��ȵ������������µ�ֵ
}

void DisplayEnt::setRadius(double mpradius) {
	assertWriteEnabled(false);
	AcDbDwgFiler *pFiler = NULL;
	if ((pFiler = undoFiler()) != NULL) {
		acutPrintf(_T("\n set undo filter"));
		undoFiler()->writeAddress(DisplayEnt::desc()); //����ʵ����
		undoFiler()->writeItem((Adesk::Int16)kRadius); //�������Ա��
		undoFiler()->writeDouble(radius);
	}

	radius = mpradius;//�ǵ��ȵ������������µ�ֵ
}

void DisplayEnt::setObjIdArr(AcDbObjectIdArray mpObjIdArr)
{
	assertWriteEnabled();
	objIdArr = mpObjIdArr;
}

void DisplayEnt::setOriCenter(AcGePoint3d center)
{
	assertWriteEnabled();
	oriCenter = center;
}

AcGePoint3d DisplayEnt::getCenter() {
	assertReadEnabled();
	return center;
}

double DisplayEnt::getRadius()
{
	assertReadEnabled();
	return radius;
}

AcDbObjectIdArray DisplayEnt::getObjIdArr()
{
	assertReadEnabled();
	return objIdArr;
}

AcGePoint3d DisplayEnt::getOriCenter()
{
	assertReadEnabled();
	return oriCenter;
}

// ����undo
Acad::ErrorStatus DisplayEnt::applyPartialUndo(AcDbDwgFiler * undoFiler, AcRxClass * classObj)
{
	acutPrintf(_T("\n [applyPartialUndo]"));

	//���classObj�������ǵ���ʱ��һ��������ʵ����
	if (classObj != DisplayEnt::desc())
		return AcDbEntity::applyPartialUndo(undoFiler, classObj);
	Adesk::Int16 shortCode;
	undoFiler->readItem(&shortCode);
	PartialUndoCode code = (PartialUndoCode)shortCode;
	double rad = 0;
	AcGePoint3d cent;

	switch (code) {//�������Ա�����ò�ͬ������ֵ
	case kRadius:
		acutPrintf(_T("\n undo set radius"));

		//��dwgInFieldsһ��������ж�����ݣ�˳��Ҫ�뵼��ʱһ��
		undoFiler->readDouble(&rad);
		setRadius(rad);
		break;
	case kCenter:
		acutPrintf(_T("\n undo set center"));

		//��dwgInFieldsһ��������ж�����ݣ�˳��Ҫ�뵼��ʱһ��
		undoFiler->readPoint3d(&cent);
		setCenter(cent);
		break;
	default:
		assert(Adesk::kFalse);
		break;
	}
	return Acad::eOk;
}

//�ӿ��޹���ʾʵ��
Adesk::Boolean DisplayEnt::subWorldDraw(AcGiWorldDraw * mode) 
{
	assertReadEnabled();
	// ���Լ�
	mode->geometry().circle(center, radius, AcGeVector3d::kZAxis);
	
	AcDbObjectIdArray idArr = this->getObjIdArr();
	// ����ƫ����������������Ϊ�Զ���ʵ�������
	mode->geometry().pushModelTransform(AcGeMatrix3d::translation(center - this->getOriCenter()));

	acutPrintf(_T("\n subWorldDraw, length: %d"), idArr.length());

	// ����ÿ��ʵ�岢��ʾ
	for (int i = 0; i < idArr.length(); i++) {
		AcDbEntity* pEnt = nullptr;

		acdbOpenAcDbEntity(pEnt, idArr[i], AcDb::kForWrite);
		if (pEnt)
		{
			pEnt->setColorIndex(1);
			mode->geometry().draw(pEnt);
			pEnt->close();
		}
	}

	mode->geometry().popModelTransform();
	return (AcDbEntity::subWorldDraw(mode));
}

//�ӿ���ص���ʾʵ��
void DisplayEnt::subViewportDraw(AcGiViewportDraw * mode)
{
	assertReadEnabled();
	mode->geometry().circle(center, radius, AcGeVector3d::kZAxis);
	AcDbEntity::subViewportDraw(mode);
}

//������ʾ��ص�����
Adesk::UInt32 DisplayEnt::subSetAttributes(AcGiDrawableTraits * traits)
{
	assertReadEnabled();
	return (AcDbEntity::subSetAttributes(traits));
}

//ͨ������任��ʵ��ƽ����ת����
Acad::ErrorStatus DisplayEnt::subTransformBy(const AcGeMatrix3d & xform) {
	assertWriteEnabled();
	setCenter(center.transformBy(xform));
	return Acad::eOk;
}


//��CAD���ͼе㣨λ�ã�
Acad::ErrorStatus DisplayEnt::subGetGripPoints(AcGePoint3dArray & gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds) const {
	assertReadEnabled();//��Ȩ

	d_left[X] = center[X] - radius;
	d_left[Y] = center[Y];
	d_right[X] = center[X] + radius;
	d_right[Y] = center[Y];
	d_above[X] = center[X];
	d_above[Y] = center[Y] + radius;
	d_bottom[X] = center[X];
	d_bottom[Y] = center[Y] - radius;

	gripPoints.append(center);
	gripPoints.append(d_right);
	gripPoints.append(d_left);
	gripPoints.append(d_above);
	gripPoints.append(d_bottom);
	return Acad::eOk;
}

//��קʱ����е��ƶ���Ϊ
Acad::ErrorStatus DisplayEnt::subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d & offset) {
	assertWriteEnabled();

	switch (indices[0]) {
	case(0):
		setCenter(center + offset);
		break;
	case(1):
		d = d_right + offset;
		setRadius(center.distanceTo(d));
		break;
	case(2):
		d = d_left + offset;
		setRadius(center.distanceTo(d));
		break;
	case(3):
		d = d_above + offset;
		setRadius(center.distanceTo(d));
		break;
	case(4):
		d = d_bottom + offset;
		setRadius(center.distanceTo(d));
		break;
	}
	return Acad::eOk;
}


//�е㲶׽
Acad::ErrorStatus DisplayEnt::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds
) const {
	assertReadEnabled();
	AcDbCircle circle;
	circle.setCenter(center);
	circle.setRadius(radius);
	return circle.getOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
}


//�Զ���ʵ��Ķ�ȡ
Acad::ErrorStatus DisplayEnt::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(DisplayEnt::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	pFiler->writePoint3d(center);
	pFiler->writeDouble(radius);
	pFiler->writePoint3d(oriCenter);
	// ����Ҫ��ʾ��ʵ���id�ĳ���
	pFiler->writeInt32(objIdArr.length());
	// ����Ҫ��ʾ��ʵ���id
	for (int i = 0; i < objIdArr.length(); i++) {
		pFiler->writeHardPointerId(objIdArr[i]);
	}

	return (pFiler->filerStatus());
}

//�Զ���ʵ��ı���
Acad::ErrorStatus DisplayEnt::dwgInFields(AcDbDwgFiler * pFiler) {

	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > DisplayEnt::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	if (version < DisplayEnt::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//----- Read params
	pFiler->readPoint3d(&center);
	pFiler->readDouble(&radius);
	pFiler->readPoint3d(&oriCenter);

	int length;
	AcDbObjectIdArray arr;
	ZcDbHardPointerId _id;
	// ��ȡҪ��ʾ��ʵ��id����ĳ���
	pFiler->readInt32(&length);
	// ����Ҫ�Ŵ���ʾ��ʵ���id
	for (int i = 0; i < length; i++) {
		pFiler->readHardPointerId(&_id);
		arr.append(_id);
	}
	this->setObjIdArr(arr);
	return (pFiler->filerStatus());
}

Acad::ErrorStatus
DisplayEnt::subDeepClone(AcDbObject*    pOwner,
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
	auto *pClone = (DisplayEnt*)isA()->create();
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

Acad::ErrorStatus DisplayEnt::subWblockClone(
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
	auto *pClone = (DisplayEnt*)isA()->create();
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