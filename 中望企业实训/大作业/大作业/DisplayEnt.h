#pragma once
#include "dbmain.h"

// ��ʾģ�飬Ҫ�Ŵ��Ԫ�ػ���������ʾ
class DisplayEnt : public AcDbEntity
{
public:
	ACRX_DECLARE_MEMBERS(DisplayEnt);//������һЩ����ʱ����ʶ���ת����������cast, desc, isA��

	DisplayEnt();
	DisplayEnt(AcGePoint3d mpCenter, double mpRadius);
	~DisplayEnt();

	void setCenter(AcGePoint3d mpCenter);
	void setRadius(double mpRadius);
	void setObjIdArr(AcDbObjectIdArray mpObjIdArr);
	void setOriCenter(AcGePoint3d center);

	AcGePoint3d getCenter();
	double getRadius();
	AcDbObjectIdArray getObjIdArr();
	AcGePoint3d getOriCenter();

	Acad::ErrorStatus applyPartialUndo(AcDbDwgFiler* undoFiler, AcRxClass* classObj);

private:
	AcGePoint3d center;
	double radius;
	AcGePoint3d oriCenter;
	AcDbObjectIdArray objIdArr;

protected:
	Acad::ErrorStatus subWblockClone(
		AcRxObject*    pOwner,
		AcDbObject*&   pClonedObject,
		AcDbIdMapping& idMap,
		Adesk::Boolean isPrimary) const;
	Acad::ErrorStatus subDeepClone(AcDbObject* pOwner,
		AcDbObject*&   pClonedObject,
		AcDbIdMapping& idMap,
		Adesk::Boolean isPrimary) const;

	static Adesk::UInt32 kCurrentVersionNumber;//��¼�Զ���ʵ��İ汾���������汾����

	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode);//�ӿ��޹ص���ʾ
	virtual void subViewportDraw(AcGiViewportDraw* mode);//�ӿ���ص���ʾ
	virtual Adesk::UInt32 subSetAttributes(AcGiDrawableTraits* traits);//������ʾ��ص�����

	//ͨ������任��ʵ��ƽ����ת����
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);

	//�Զ���ʵ���Լ�����е�λ�ã�ͨ��subGetGripPoints��CAD����λ�ã�
	//�ڷ����е���קʱͨ��subMoveGripPointsAt����е��ƶ�ʱ����Ϊ
	virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset);

	//��CAD���Խ��ж���׽������£���꿿���Զ���ʵ��ᴥ��subGetOsnapPoints������
	//���Զ���ʵ����������μ��㲶׽��
	virtual Acad::ErrorStatus subGetOsnapPoints(
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d& pickPoint,
		const AcGePoint3d& lastPoint,
		const AcGeMatrix3d& viewXform,
		AcGePoint3dArray& snapPoints,
		AcDbIntArray& geomIds
	)const;

	//�����ļ� undo redo
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler);
};

#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(DisplayEnt)
#endif
