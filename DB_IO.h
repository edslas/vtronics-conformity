// DB_IO.h: interface for the CDB_IO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_IO_H__815D1A20_155A_11D5_B177_00E029467F2C__INCLUDED_)
#define AFX_DB_IO_H__815D1A20_155A_11D5_B177_00E029467F2C__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VNumbersDB.h"
#include "CustomerDB.h"

class CDB_IO  
{
public:
	int CopyDBData(VNumberStruct *pV);
	int CopyVData(VNumberStruct *pV);
	int WriteVDataToBD(VNumberStruct *pV);
	VNumberStruct * GetFirstVNumber();
	VNumberStruct * GetNextVNumber();
	char *GetFirstCustomer();
	char *GetNextCustomer();
	int AddNewCustomer(char *pNewCustomer);

	int WriteConformityData(ConformityDataStruct CD);
	CDB_IO();
	virtual ~CDB_IO();
private:
	CDatabase m_DB;
	BOOL m_bSuccess;
	CVNumbersDB *m_pVData;
	CCustomerDB	*m_pCData;

};

#endif // !defined(AFX_DB_IO_H__815D1A20_155A_11D5_B177_00E029467F2C__INCLUDED_)
