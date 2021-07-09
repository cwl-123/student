#pragma once
#include "dbmain.h"



//���Զ���ʵ��̳���AcDbEntity��������õ��Զ���ʵ�常��
class __declspec(dllexport)  SampleCustEnt : public AcDbEntity {
	//
public:
	ACRX_DECLARE_MEMBERS(SampleCustEnt);//������һЩ����ʱ����ʶ���ת����������cast, desc, isA��

protected:
	AcGePoint3d m_center;
	double m_radius;
	static Adesk::UInt32 kCurrentVersionNumber;//��¼�Զ���ʵ��İ汾���������汾����

public:
	SampleCustEnt();//�޲ι���
	SampleCustEnt(AcGePoint3d center, double radius);//�вι���
	virtual ~SampleCustEnt();

	void setCenter(AcGePoint3d mpcenter);
	void setRadius(double mpradius);
	AcGePoint3d center();

protected:
	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw *mode);//�ӿ��޹ص���ʾ
	virtual void subViewportDraw(AcGiViewportDraw* mode);//�ӿ���ص���ʾ
	virtual Adesk::UInt32 subSetAttributes(AcGiDrawableTraits *traits);//������ʾ��ص�����

	//ͨ������任��ʵ��ƽ����ת����
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);

	//�Զ���ʵ���Լ�����е�λ�ã�ͨ��subGetGripPoints��CAD����λ�ã�
	//�ڷ����е���קʱͨ��subMoveGripPointsAt����е��ƶ�ʱ����Ϊ
	virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d& offset);

	//��CAD���Խ��ж���׽������£���꿿���Զ���ʵ��ᴥ��subGetOsnapPoints������
	//���Զ���ʵ����������μ��㲶׽��
	virtual Acad::ErrorStatus subGetOsnapPoints(
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d& pickPoint,
		const AcGePoint3d& lastPoint,
		const AcGeMatrix3d& viewXform,
		AcGePoint3dArray& snapPoints,
		AcDbIntArray & geomIds
	)const;

	//�����ļ� undo redo
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *pFiler) const;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *pFiler);

};

#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(SampleCustEnt)
#endif
