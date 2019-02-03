#if !defined(AFX_VNUMBERSDB_H__9F321967_5F2D_4FA6_A5A4_BB2A744C2B0B__INCLUDED_)
#define AFX_VNUMBERSDB_H__9F321967_5F2D_4FA6_A5A4_BB2A744C2B0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VNumbersDB.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVNumbersDB recordset

class CVNumbersDB : public CRecordset
{
public:
	CVNumbersDB(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CVNumbersDB)

// Field/Param Data
	//{{AFX_FIELD(CVNumbersDB, CRecordset)
	long	m_Band_1;
	long	m_Band_2;
	CString	m_Comment;
	long	m_Dead;
	double	m_Diameter;
	long	m_ID;
	double	m_Max_Conformity;
	long	m_Resistance;
	BOOL	m_Spilt;
	long	m_Stack_Count;
	long	m_Tolerence;
	CString	m_VNumber;
	long	m_Angle;
	long	m_Winding;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVNumbersDB)
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

#endif // !defined(AFX_VNUMBERSDB_H__9F321967_5F2D_4FA6_A5A4_BB2A744C2B0B__INCLUDED_)
