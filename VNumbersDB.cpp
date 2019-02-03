// VNumbersDB.cpp : implementation file
//

//#include "stdafx.h"
#include <afxdb.h>

#include "VNumbersDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVNumbersDB

IMPLEMENT_DYNAMIC(CVNumbersDB, CRecordset)

CVNumbersDB::CVNumbersDB(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CVNumbersDB)
	m_Band_1 = 0;
	m_Band_2 = 0;
	m_Comment = _T("");
	m_Dead = 0;
	m_Diameter = 0.0;
	m_ID = 0;
	m_Max_Conformity = 0.0;
	m_Resistance = 0;
	m_Spilt = FALSE;
	m_Stack_Count = 0;
	m_Tolerence = 0;
	m_VNumber = _T("");
	m_Angle = 0;
	m_Winding = 0;
	m_nFields = 14;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CVNumbersDB::GetDefaultConnect()
{
	return _T("ODBC;DSN=MS Access Database");
}

CString CVNumbersDB::GetDefaultSQL()
{
	return _T("[V Number]");
}

void CVNumbersDB::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CVNumbersDB)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[Band 1]"), m_Band_1);
	RFX_Long(pFX, _T("[Band 2]"), m_Band_2);
	RFX_Text(pFX, _T("[Comment]"), m_Comment);
	RFX_Long(pFX, _T("[Dead]"), m_Dead);
	RFX_Double(pFX, _T("[Diameter]"), m_Diameter);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Double(pFX, _T("[Max Conformity]"), m_Max_Conformity);
	RFX_Long(pFX, _T("[Resistance]"), m_Resistance);
	RFX_Bool(pFX, _T("[Spilt]"), m_Spilt);
	RFX_Long(pFX, _T("[Stack Count]"), m_Stack_Count);
	RFX_Long(pFX, _T("[Tolerence]"), m_Tolerence);
	RFX_Text(pFX, _T("[VNumber]"), m_VNumber);
	RFX_Long(pFX, _T("[Angle]"), m_Angle);
	RFX_Long(pFX, _T("[Winding]"), m_Winding);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CVNumbersDB diagnostics

#ifdef _DEBUG
void CVNumbersDB::AssertValid() const
{
	CRecordset::AssertValid();
}

void CVNumbersDB::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
