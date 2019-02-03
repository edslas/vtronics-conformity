// ConformityData.cpp : implementation file
//

//#include "stdafx.h"
#include <afxdb.h>
#include "VNumbersDB.h"
#include "ConformityData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConformityData

IMPLEMENT_DYNAMIC(CConformityData, CRecordset)

CConformityData::CConformityData(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CConformityData)
	m_Conformity = 0.0;
	m_Customer = _T("");
	m_ID = 0;
	m_Operator = _T("");
	m_PO_Number = _T("");
	m_Resistance = 0.0;
	m_Serial_Number = _T("");
	m_V_Number = _T("");
	m_nFields = 9;
	//}}AFX_FIELD_INIT
	m_nDefaultType = dynaset;
}


CString CConformityData::GetDefaultConnect()
{
	return _T("ODBC;DSN=MS Access 97 Database");
}

CString CConformityData::GetDefaultSQL()
{
	return _T("[PartData]");
}

void CConformityData::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CConformityData)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Double(pFX, _T("[Conformity]"), m_Conformity);
	RFX_Text(pFX, _T("[Customer]"), m_Customer);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[Operator]"), m_Operator);
	RFX_Text(pFX, _T("[PO Number]"), m_PO_Number);
	RFX_Double(pFX, _T("[Resistance]"), m_Resistance);
	RFX_Text(pFX, _T("[Serial Number]"), m_Serial_Number);
	RFX_Date(pFX, _T("[Test Date]"), m_Test_Date);
	RFX_Text(pFX, _T("[V Number]"), m_V_Number);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CConformityData diagnostics

#ifdef _DEBUG
void CConformityData::AssertValid() const
{
	CRecordset::AssertValid();
}

void CConformityData::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

void CConformityData::Write()
{
}
