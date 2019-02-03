// DB_IO.cpp: implementation of the CDB_IO class.
//
//////////////////////////////////////////////////////////////////////

#include <afxdb.h>
#include "Globals.h"
#include "ConformityData.h"


#include "DB_IO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDB_IO::CDB_IO()
{
 

	m_bSuccess = TRUE;
	try 
	{
		m_bSuccess = m_DB.OpenEx("ODBC;DBQ=C:\\Voltronics\\Data\\Data.mdb;DefaultDir=C:\\Voltronics\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Voltronics\\Data\\Data.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
	}
	catch( CDBException eBD)
	{
		m_bSuccess = FALSE;
	}
	catch( CMemoryException eM )
	{
		m_bSuccess = FALSE;
	}
	catch(...)
	{
		m_bSuccess = FALSE;
	}
	if(!(m_DB.IsOpen()))
	{
		m_bSuccess = FALSE;
	}
	m_pVData = NULL;
	m_pCData = NULL;
}

CDB_IO::~CDB_IO()
{
	if(m_bSuccess) 
	{
		m_DB.Close();
	}
	m_bSuccess = false;

}

int CDB_IO::WriteConformityData(ConformityDataStruct CD)
{
	if(m_bSuccess)
	{
		CConformityData *Data = new CConformityData(&m_DB);
		
		if(Data)
		{
			if(Data->Open(CRecordset::dynaset))
			{
				Data->AddNew();
					Data->m_Customer =  CD.pCustomer;
					Data->m_PO_Number = CD.pPO;
					Data->m_Serial_Number = CD.pSerialNumber;
					Data->m_V_Number = CD.pVNumber;
					Data->m_Test_Date = *(new CTime(time(NULL)));
					Data->m_Resistance = CD.dResistance;
					Data->m_Conformity = CD.dConformity;
					Data->m_Operator = CD.pOperator;
				Data->Update();
			
			}
			else
			{
				AfxMessageBox("Can't open Part Data Data Base",  MB_ICONEXCLAMATION | MB_OK);
			}
			delete Data;
		}
	}
return (0);
}

VNumberStruct * CDB_IO::GetFirstVNumber()
{
static VNumberStruct RetData;

	if(!m_bSuccess)return NULL;
	if(m_pVData == NULL)
	{
		m_pVData = new CVNumbersDB(&m_DB);
	}
	if(!m_pVData) return NULL;
	if(!(m_pVData->Open(CRecordset::dynaset))) return NULL;
	if(m_pVData->IsBOF()) return NULL;
	m_pVData->MoveFirst();

	CopyDBData(&RetData);
	return(&RetData);

}
VNumberStruct * CDB_IO::GetNextVNumber()
{
static VNumberStruct RetData;

	if(!m_bSuccess)return NULL;
	if(!m_pVData) return NULL;
	m_pVData->MoveNext();
	if(m_pVData->IsEOF()) return NULL;
	CopyDBData(&RetData);
	return(&RetData);

}

int CDB_IO::CopyDBData(VNumberStruct *pV)
{
// Copies data from Data base into local structure definition
	if(m_pVData->m_VNumber.GetLength() < V_NUMBER_LENGTH - 1)
	{
		strcpy(pV->pVNumber,m_pVData->m_VNumber);
	}
	else
	{
		strncpy(pV->pVNumber,m_pVData->m_VNumber, V_NUMBER_LENGTH-1);
		pV->pVNumber[V_NUMBER_LENGTH - 1] = 0;
	}
	pV->dConformity		= m_pVData->m_Max_Conformity;
	pV->dDiameter		= m_pVData->m_Diameter;
	pV->iTolerence		= m_pVData->m_Tolerence;
	pV->iAngle			= m_pVData->m_Angle;
	pV->iBand1			= m_pVData->m_Band_1;
	pV->iBand2			= m_pVData->m_Band_2;
	pV->iDeadBand		= m_pVData->m_Dead;
	pV->iResistance		= m_pVData->m_Resistance;
	pV->iSplit			= m_pVData->m_Spilt;
	pV->iStacks			= m_pVData->m_Stack_Count;
	pV->iWinding		= m_pVData->m_Winding;
	if(m_pVData->m_Comment.GetLength() < COMMENT_LENGTH - 1)
	{
		strcpy(pV->pComment,m_pVData->m_Comment);
	}
	else
	{
		strncpy(pV->pComment,m_pVData->m_Comment, COMMENT_LENGTH);
		pV->pComment[COMMENT_LENGTH -1] = 0;
	}
return 0;

}
int CDB_IO::CopyVData(VNumberStruct *pV)
{
// Copies data from Parameter into DB
	m_pVData->m_VNumber = pV->pVNumber;
	m_pVData->m_Max_Conformity	= pV->dConformity;
	m_pVData->m_Diameter 		= pV->dDiameter;
	m_pVData->m_Tolerence 		= pV->iTolerence;
	m_pVData->m_Angle 			= pV->iAngle;
	m_pVData->m_Band_1 			= pV->iBand1;
	m_pVData->m_Band_2 			= pV->iBand2;
	m_pVData->m_Dead 			= pV->iDeadBand;
	m_pVData->m_Resistance 		= pV->iResistance;
	m_pVData->m_Spilt 			= pV->iSplit;
	m_pVData->m_Stack_Count 	= pV->iStacks;
	m_pVData->m_Winding 		= pV->iWinding;
	m_pVData->m_Comment = pV->pComment;
return 0;

}

int CDB_IO::WriteVDataToBD(VNumberStruct *pV)
{
VNumberStruct *pDBData = NULL;
bool found;

	if(!m_bSuccess)return -1;
	if(m_pVData == NULL)
	{
		m_pVData = new CVNumbersDB(&m_DB);
	}

	if(!m_pVData) return -2;

	// Check for Existing
	found = false;
	pDBData = GetFirstVNumber();
	while(pDBData && !found)
	{
		if(strcmp(pV->pVNumber, pDBData->pVNumber) == 0)
		{
			found = true;
		}
		else
		{
			pDBData = GetNextVNumber();
		}
	}
	if(found && pDBData)
	{
		// prompt
		if(MessageBox(NULL,"V Number Already Exists, OVERWRITE?", "Overwrite Existing?",MB_YESNO) == IDYES)
		{
			m_pVData->Edit();
			CopyVData(pV);
			m_pVData->Update();	
		}
		else
		{
			return -3;
		}
	}
	else
	{
		// Add New
		m_pVData->AddNew();
		CopyVData(pV);
		m_pVData->Update();

	}
	return 0;
}
char * CDB_IO::GetFirstCustomer()
{
static char Customer[MAX_CUSTOMER_LENGTH];

	if(!m_bSuccess)return NULL;
	if(m_pCData == NULL)
	{
		m_pCData = new CCustomerDB(&m_DB);
	}
	if(!m_pCData) return NULL;
	if(!(m_pCData->Open(CRecordset::dynaset))) return NULL;
	if(m_pCData->IsBOF()) return NULL;
	m_pCData->MoveFirst();

	if(m_pCData->m_Customer_Name.GetLength() < MAX_CUSTOMER_LENGTH -1)
	{
		strcpy(Customer, m_pCData->m_Customer_Name); 
	}
	else
	{
		strncpy(Customer, m_pCData->m_Customer_Name, MAX_CUSTOMER_LENGTH); 
		Customer[MAX_CUSTOMER_LENGTH -1] = 0;
	}
	return(Customer);

}
char * CDB_IO::GetNextCustomer()
{
static char Customer[MAX_CUSTOMER_LENGTH];

	if(!m_bSuccess)return NULL;
	if(!m_pCData) return NULL;
	m_pCData->MoveNext();
	if(m_pCData->IsEOF()) return NULL;
	if(m_pCData->m_Customer_Name.GetLength() < MAX_CUSTOMER_LENGTH -1)
	{
		strcpy(Customer, m_pCData->m_Customer_Name); 
	}
	else
	{
		strncpy(Customer, m_pCData->m_Customer_Name, MAX_CUSTOMER_LENGTH); 
		Customer[MAX_CUSTOMER_LENGTH -1] = 0;
	}
	return(Customer);

}
int CDB_IO::AddNewCustomer(char *pNewCustomer)
{
int ierror = 0;
char *pcCustomer;
CString Temp;
	// Check for duplicate
	if(!m_bSuccess)return NULL;
	pcCustomer = GetFirstCustomer();
	ierror = 0;
	Temp = pNewCustomer;
	
	while(pcCustomer && (ierror == 0))
	{
		// Load DB Entry into control:
		// add to control
		if(Temp.CompareNoCase(pcCustomer) == 0)
		{
			AfxMessageBox("Error\nCustomer Already Exists!",  MB_ICONEXCLAMATION | MB_OK);
			ierror = -1;
		}
		else
		{
			pcCustomer = GetNextCustomer();
		}	
	}
	if(ierror == 0)
	{
		// Add New
		m_pCData->AddNew();
		m_pCData->m_Customer_Name = pNewCustomer;
		m_pCData->Update();

	}

return ierror;
}
