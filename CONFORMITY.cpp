/*************************************************************************/
/*                                                                       */
/* File Name:           Conformity.CPP                                     */
/* Name:				Voltronics, Potentiometer Tester                 */
/* Project Number:		1000                                             */
/*                                                                       */
/* Date:                March 8, 2001                                  */
/*                                                                       */
/* Operating System:    WIN2000                                           */
/* Compiler:            Visual C++ 6.0                                   */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*   Main module for the HMI                                             */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* REVISION HISTORY                                                      */
/*                                                                       */
/* mm/dd/yyyy                                                            */
/*************************************************************************/

#include <afxdb.h>
#include "globals.h"
#include "VNumbersDB.h"
#include "DB_IO.h"

#include <UTILITY.h>
#include <ANSI_C.h>
//#include <memory.h>
#include <cvirte.h>		/* Needed if linking in external compiler; harmless otherwise */
#include <userint.h>
#include "main.h"
#include "globals.h"
#include "DataAquisition.h"
#include "StepperMotor.h"
#include "TroubleShoot.h"
#include "Add_V.h"
#include "Conformity_Test.h"

#define MAIN_UIR "C:\\Voltronics\\Conformity\\Program\\main.uir"

//extern int GetVDatFromFile(VNumberStruct *pVEntry,char *pPath);
extern int SetEditVData(VNumberStruct *pData);  
int CVICALLBACK EditV (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2);

// Global Declarations:
int pSetup;
int pEditV;
int pConformityTest; 
int pTroubleShoot;

VNumberStruct CurrentVNumber;
ConformityDataStruct CurrentPartData;

int BuildVNumberList(void);
int SetVData(VNumberStruct *pData);



int CVICALLBACK Exit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			IMS_Close(-1);
			exit(0);
			break;
		}
	return 0;
}

int CVICALLBACK TroubleShoot (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			//HidePanel (pSetup);
			DisplayPanel (pTroubleShoot);
			break;
		}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////
//
//		V Number Handeling
//

int BuildVNumberList(void)
{
	int i = 0;
	VNumberStruct *pVEntry, *pListVEntry;	
	int ierror = 0;
	int Count;
	unsigned int uiData;
	CDB_IO DB;	

	// Kill old list
	//
	i = 0;
	ierror = GetNumListItems (pSetup, SETUP_VNUMBER, &Count);
	while(ierror == 0 && i < Count)
	{
		ierror = GetValueFromIndex (pSetup, SETUP_VNUMBER, i, &uiData);
		if(ierror == 0)
		{
			free ((void *)(uiData));
		}
		i++;
	}
	ierror = ClearListCtrl (pSetup, SETUP_VNUMBER);
	
	pVEntry = DB.GetFirstVNumber();
	i = 0;
	ierror = 0;
	while(pVEntry && (ierror == 0))
	{
		// Load DB Entry into control:
		// Malloc memory for entry
		pListVEntry = (VNumberStruct *)malloc(sizeof(VNumberStruct));
		memcpy(pListVEntry,pVEntry, sizeof(VNumberStruct));
		// add to control
		ierror = InsertListItem (pSetup, SETUP_VNUMBER, i, pListVEntry->pVNumber, (unsigned int)pListVEntry);		
		if(ierror == 0)
		{
			if(i == 0)
			{
				CurrentVNumber = *pListVEntry; 
				SetVData(pListVEntry);
				ierror = SetCtrlIndex (pSetup, SETUP_VNUMBER, 0);
				
			}
			i++;
			pVEntry = DB.GetNextVNumber();
		}	
		else
		{
			ierror = -1;	
		}
	}
	
	return ierror;
}


int CVICALLBACK VSelection (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
int i, ierror;
VNumberStruct NullVGData, DBData;
int ListCount = 0;
bool Found = false;
unsigned int Temp;
char EnteredVNumber[V_NUMBER_LENGTH];
	switch (event)
	{
		case EVENT_COMMIT:
			// Find V Number in list
			ierror = GetCtrlVal(pSetup, SETUP_VENTRY, EnteredVNumber);
			ierror = GetNumListItems (pSetup, SETUP_VNUMBER, &ListCount);
			for( i = 0; i < ListCount && !Found; i++)
			{
				ierror = GetValueFromIndex (pSetup, SETUP_VNUMBER, i, &Temp);	
				DBData = *((VNumberStruct *)Temp);
				if(strcmp(DBData.pVNumber, EnteredVNumber) == 0)
				{
					CurrentVNumber = DBData;
					CurrentVNumber.bValidData = true;
					SetVData(&CurrentVNumber);
					Found = true;
				}
			}
			if(!Found)
			{
				CurrentVNumber.bValidData = false;
				if(AfxMessageBox( "V Number Not Found, Do you wish to add it", MB_YESNO,0) == IDYES)
				{
						strcpy(CurrentVNumber.pVNumber, EnteredVNumber);
						EditV (0,0 ,EVENT_COMMIT, NULL,0, 0);

				}
			}
			break;
		case EVENT_VAL_CHANGED:
			// 0 out display when user changes text
			memset(&NullVGData, 0, sizeof(VNumberStruct));
			SetVData(&NullVGData);
			break;
	}
	return 0;
}

int SetVData(VNumberStruct *pData)
{
int ierror;
char splittext[64];
	ierror = SetCtrlVal (pSetup, SETUP_DIAMETER, pData->dDiameter);
	ierror += SetCtrlVal (pSetup, SETUP_ANGLE, pData->iAngle);
	ierror += SetCtrlVal (pSetup, SETUP_CONFORMITY, pData->dConformity);
	ierror += SetCtrlVal (pSetup, SETUP_RESISTANCE, pData->iResistance);
	ierror += SetCtrlVal (pSetup, SETUP_TOLERENCE, pData->iTolerence);
	ierror += SetCtrlVal (pSetup, SETUP_WINDING, pData->iWinding);
	ierror += SetCtrlVal (pSetup, SETUP_STACKS, pData->iStacks);
	ierror += ResetTextBox (pSetup, SETUP_COMMENT, pData->pComment);
	
	if(pData->iSplit)
	{
		sprintf(splittext,"%d - %d - %d",
				pData->iBand1,
				pData->iDeadBand,
				pData->iBand2);
	}
	else
	{
		strcpy(splittext, "none");	
	}
	ierror += ResetTextBox (pSetup, SETUP_SPLIT, splittext);
	return ierror;
}
int CVICALLBACK EditV (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if(strlen(CurrentVNumber.pVNumber))
			{
				SetEditVData(&CurrentVNumber);		 
			}
			//HidePanel (pSetup);
			DisplayPanel (pEditV);
			
			break;
		}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////
//
//		Customer Handeling
//

int BuildCustomerList(void)
{
	int i = 0;
	
	int ierror = 0;
	char *pcCustomer;
	CDB_IO DB;	

	ierror = ClearListCtrl (pSetup, SETUP_CUSTOMER);
	
	pcCustomer = DB.GetFirstCustomer();
	i = 0;
	ierror = 0;
	while(pcCustomer && (ierror == 0))
	{
		// Load DB Entry into control:
		// add to control
		ierror = InsertListItem (pSetup, SETUP_CUSTOMER, i, pcCustomer, NULL);		
		if(ierror == 0)
		{
			i++;
			pcCustomer = DB.GetNextCustomer();
		}	
		else
		{
			ierror = -1;	
		}
	}
	
	return ierror;
}
int CVICALLBACK AddCustomer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
int ierror;
char NewName[MAX_CUSTOMER_LENGTH] = "";
	switch (event)
		{
		case EVENT_COMMIT:
			ierror = GetCtrlVal(pSetup, SETUP_CUSTENTRY, NewName);
			if(ierror == 0)
			{
				if(strlen(NewName) > 0)
				{
					if(strlen(NewName) < MAX_CUSTOMER_LENGTH -1)
					{
						CDB_IO *DB;
						DB = new CDB_IO();
						ierror = DB->AddNewCustomer(NewName);
						delete DB;
						BuildCustomerList();
						if(ierror ==0)
						{
							AfxMessageBox("Customer Added");
						}
					}
					else
					{
						AfxMessageBox("Error\nName too long!",  MB_ICONEXCLAMATION | MB_OK);
					}
				}
			}
			break;
		}
	return 0;
}
int CVICALLBACK CustSelection (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
int i, ierror;
int ListCount = 0;
bool Found = false;
char EnteredCustomer[MAX_CUSTOMER_LENGTH];
char DBCustomer[MAX_CUSTOMER_LENGTH];
CString csEnteredCustomer;
	switch (event)
	{
		case EVENT_COMMIT:
			// Find V Number in list
			ierror = GetCtrlVal(pSetup, SETUP_CUSTENTRY, EnteredCustomer);
			csEnteredCustomer = EnteredCustomer;
			ierror = GetNumListItems (pSetup, SETUP_CUSTOMER, &ListCount);
			for( i = 0; i < ListCount && !Found; i++)
			{
				//ierror = SetCtrlIndex (pSetup, SETUP_CUSTOMER, i);
				ierror = GetLabelFromIndex (pSetup, SETUP_CUSTOMER, i, DBCustomer);
				if(csEnteredCustomer.CompareNoCase(DBCustomer) == 0)
				{
					ierror = SetCtrlVal(pSetup, SETUP_CUSTENTRY, DBCustomer);
					Found = true;
				}
			}
			if(!Found)
			{
				if(AfxMessageBox("Customer Not Found, Do you wish to add it", MB_YESNO,0) == IDYES)
				{
					AddCustomer( 0,0,EVENT_COMMIT,NULL,0,0);
				}
				else
				{
					SetCtrlVal(pSetup, SETUP_CUSTENTRY, "");
				}
			}
			break;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//		Main Function
//
int __stdcall WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                       LPSTR lpszCmdLine, int nCmdShow)
{
int ierror;
double Reading; 


	if (InitCVIRTE(hInstance,0,0)==0)	/* Needed if linking in external compiler; harmless otherwise */
		return -1;	/* out of memory */
	if ((pSetup = LoadPanel (0, MAIN_UIR, SETUP)) < 0)
		return -1;
	if ((pEditV = LoadPanel (0, MAIN_UIR, EDITV)) < 0)
		return -1;
	if ((pConformityTest = LoadPanel (0, MAIN_UIR, TEST)) < 0)
		return -1;
	if ((pTroubleShoot = LoadPanel (0, MAIN_UIR, TBLSHOOT)) < 0)
		return -1;
	// pSetup Callbacks
	InstallCtrlCallback(pSetup,SETUP_EXIT,Exit,NULL);
	InstallCtrlCallback(pSetup,SETUP_EDITV,EditV,NULL);
	InstallCtrlCallback(pSetup,SETUP_TROUBLESHOOT,TroubleShoot,NULL);
	InstallCtrlCallback(pSetup,SETUP_START,StartTest,NULL);
	InstallCtrlCallback(pSetup,SETUP_VENTRY,VSelection,NULL);
	InstallCtrlCallback(pSetup,SETUP_CUSTENTRY,CustSelection,NULL);

	// EditV Callbacks
	InstallCtrlCallback(pEditV,EDITV_SPLIT,SplitChange,NULL);
	InstallCtrlCallback(pEditV,EDITV_CANCEL,CancelEditV,NULL);
	InstallCtrlCallback(pEditV,EDITV_SAVE,SaveEditV,NULL);


	

	// TroubleShoot Callbacks
	InstallCtrlCallback(pTroubleShoot,TBLSHOOT_TIMER,TroubleShootTimer,NULL);
	InstallCtrlCallback(pTroubleShoot,TBLSHOOT_START_MOVE,StartTroubleShootMove,NULL);
	InstallCtrlCallback(pTroubleShoot,TBLSHOOT_SAVE_DEFAULT,SaveDefaultSpeeds,NULL);
	InstallCtrlCallback(pTroubleShoot,TBLSHOOT_CANCEL,ExitTroubleShoot,NULL);
	
	// Hide V And Customer lists
	ierror = SetCtrlAttribute (pSetup, SETUP_VNUMBER, ATTR_VISIBLE, 0);
	ierror = SetCtrlAttribute (pSetup, SETUP_CUSTOMER, ATTR_VISIBLE, 0);
	


	if(InitializeDAQ() != 0)
	{
		MessagePopup ("Fatal Error", "Could not initialize DAQ"); 
		return -1;
	}
	if(SetUpExternalTrigger() != 0)
	{
		MessagePopup ("Fatal Error", "Could not set trigger"); 
		return -1;
	}
	
	if(ReadChannel(SUPP_VOLTAGE_CH, &Reading) != 0)
	{
		MessagePopup ("Fatal Error", "Could not Read Analog Input from DAQ"); 
		return -1;	
	}
	
	memset(&CurrentPartData, 0, sizeof(ConformityDataStruct));
	
	if(IMS_Open(1) != 0)
	{
		MessagePopup ("Fatal Error", "Could not open COM1. Restart Computer"); 
		return -1;		
	}
	IMS_SendCommand(1, "MUNIT=51200/360", NULL);
	SetDefaultSpeeds();
	CurrentVNumber.bValidData = false;
	BuildVNumberList();
	BuildCustomerList();
	DisplayPanel (pSetup);
	RunUserInterface ();
	return 0;
}


