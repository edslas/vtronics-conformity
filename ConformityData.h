#if !defined(AFX_CONFORMITYDATA_H__990891A2_1545_11D5_B177_00E029467F2C__INCLUDED_)
#define AFX_CONFORMITYDATA_H__990891A2_1545_11D5_B177_00E029467F2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConformityData.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CConformityData recordset

class CConformityData : public CRecordset
{
public:
	void Write();
	CConformityData(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CConformityData)

// Field/Param Data
	//{{AFX_FIELD(CConformityData, CRecordset)
	double	m_Conformity;
	CString	m_Customer;
	long	m_ID;
	CString	m_Operator;
	CString	m_PO_Number;
	double	m_Resistance;
	CString	m_Serial_Number;
	CTime	m_Test_Date;
	CString	m_V_Number;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConformityData)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFORMITYDATA_H__990891A2_1545_11D5_B177_00E029467F2C__INCLUDED_)
