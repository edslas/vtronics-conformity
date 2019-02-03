#include <afxdb.h>
#include "globals.h"
#include "VNumbersDB.h"
#include "DB_IO.h"

#include <ANSI_C.h>
#include <userint.h>
#include "main.h"
#include "Globals.h"


extern int pSetup;
extern int pEditV;
extern int BuildVNumberList(void);
extern int CVICALLBACK VSelection (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2);

int SetEditVData(VNumberStruct *pData);
// Local Functions
int MakeVFile(VNumberStruct *pV);
//
int CVICALLBACK SplitChange (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2);

int CVICALLBACK SaveEditV (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
int ierror;
	switch (event)
		{
		case EVENT_COMMIT:
			VNumberStruct V;
			MakeVFile(&V); // reads the controls
			if( V.iSplit )
			{
				// Make Sure B1 + DB + B2 = DEG
				if((V.iBand1 + V.iDeadBand + V.iBand2) != V.iAngle)
				{
					AfxMessageBox("Sum of bands in split must equal angle.", MB_ICONEXCLAMATION | MB_OK);
					return 0;
				}
			}
			CDB_IO *DB;
			DB = new CDB_IO();
			ierror = DB->WriteVDataToBD(&V);
			delete DB;
			
			if(ierror == 0)
			{
				BuildVNumberList();
				AfxMessageBox("V Number Updated");
				// Updates setup screen with current info
				VSelection(0,0,EVENT_COMMIT,NULL,0,0);

				HidePanel (pEditV);
				DisplayPanel (pSetup);
			}
			else if(ierror != -3) // user canceled
			{
				AfxMessageBox("Error - Could Not Add/Edit V Number",  MB_ICONEXCLAMATION | MB_OK);
			}
			
			break;
		}
	return 0;
}

int CVICALLBACK CancelEditV (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
 			HidePanel (pEditV);
			//DisplayPanel (pSetup);
			break;
		}
	return 0;
}

int MakeVFile(VNumberStruct *pV)
{

//char pcVFilePath[256] = V_FILE_PATH;
//char pBuffer[1024];

//VNumberStruct V;

int ierror;
//FILE *pFileHandle;
//size_t BytesWritten;

//	pFileHandle = NULL;
	ierror = GetCtrlVal (pEditV, EDITV_VNUMBER, (void *)pV->pVNumber);
	ierror += GetCtrlVal (pEditV, EDITV_DIAMETER, (void *)&pV->dDiameter);
	ierror += GetCtrlVal (pEditV, EDITV_CONFORMITY, (void *)&pV->dConformity);
	ierror += GetCtrlVal (pEditV, EDITV_ANGLE, (void *)&pV->iAngle);
	ierror += GetCtrlVal (pEditV, EDITV_RESISTANCE, (void *)&pV->iResistance);
	ierror += GetCtrlVal (pEditV, EDITV_TOLERENCE, (void *)&pV->iTolerence);
	ierror += GetCtrlVal (pEditV, EDITV_WINDING, (void *)&pV->iWinding);
	ierror += GetCtrlVal (pEditV, EDITV_STACKS, (void *)&pV->iStacks);
	ierror += GetCtrlVal (pEditV, EDITV_SPLIT, (void *)&pV->iSplit);
	ierror += GetCtrlVal (pEditV, EDITV_BAND1, (void *)&pV->iBand1);
	ierror += GetCtrlVal (pEditV, EDITV_DEADBAND, (void *)&pV->iDeadBand);
	ierror += GetCtrlVal (pEditV, EDITV_BAND2, (void *)&pV->iBand2);
	ierror += GetCtrlVal (pEditV, EDITV_COMMENT, (void *)&pV->pComment);
	
#if 0	
	if(ierror >= 0)
	{
		strcat(pcVFilePath,V.pVNumber);
		strcat(pcVFilePath,".txt");
		pFileHandle = fopen (pcVFilePath, "w");
	}
	if(pFileHandle != NULL)
	{
  
		sprintf(pBuffer, 	"%s\n"
							"%1.5lf = Diameter\n"
							"%d = Angle\n"
							"%1.5lf = Conformity\n"
							"%d = Resistance\n"
							"%1.5lf = Tolerence\n"
							"%d = Winding type\n"
							"%d = Number Stacks\n"
							"%d = Split\n"
							"%d = Band 1\n"
							"%d = Dead Band\n"
							"%d = Band 2\n"
							"%s",
				  V.pVNumber,
				  V.dDiameter,
				  V.iAngle,
				  V.dConformity,
				  V.iResistance,
				  V.dTolerence,
				  V.iWinding,
				  V.iStacks,
				  V.iSplit,
				  V.iBand1,
				  V.iDeadBand,
				  V.iBand2,
				  V.pComment);
				  
		BytesWritten = fwrite (pBuffer, 1, strlen(pBuffer), pFileHandle);
		if(BytesWritten !=  strlen(pBuffer))
		{
			ierror = -11;
		}
				  
		fclose(pFileHandle);			  
	}
	if(ierror < 0)
	{
	   MessagePopup ("Error", "Could Not Create/Edit");
	}
	else
	{
	   MessagePopup ("Success", "V Number Created/Edited");
	}
#endif;
return ierror;

}

int GetVDatFromFile(VNumberStruct *pVEntry,char *pPath)
{
int ierror = 0;

FILE *pFileHandle;
size_t BytesRead;
char pBuffer[1024];
char *peol, *pcur = NULL;

	pFileHandle = fopen (pPath, "r"); 
	
	if(pFileHandle)		
	{
		BytesRead = fread (pBuffer, 1, 1024, pFileHandle);	
		fclose(pFileHandle);
	}
	else ierror = -1;
	
	if(BytesRead > 0)
	{
		peol =  strstr(pBuffer, "\n");
		
		if(peol)
		{
			// Get V From File
			strncpy(pVEntry->pVNumber, pBuffer, peol - pBuffer); 
			pVEntry->pVNumber[peol - pBuffer] = 0;
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol
		}
		else ierror = -2;
		
		if(peol && (ierror == 0) )
		{
			// Get Diameter from file
			sscanf(pcur, "%lf", &pVEntry->dDiameter);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol  
		}
		else ierror = -3;
		
		if(peol&& (ierror == 0) )
		{
			// Get Angle from file
			sscanf(pcur, "%d", &pVEntry->iAngle);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol
		}		
		else ierror = -4;
		
		if(peol&& (ierror == 0) )
		{
			// Get Conformity from file
			sscanf(pcur, "%lf", &pVEntry->dConformity);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol
		}		
		else ierror = -5;
		
		if(peol&& (ierror == 0) )
		{
			// Get Resistance from file
			sscanf(pcur, "%d", &pVEntry->iResistance);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol
		}		
		else ierror = -6;		
		
		if(peol&& (ierror == 0) )
		{
			// Get Tolerence from file
			sscanf(pcur, "%lf", &pVEntry->iTolerence);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol
		}		
		else ierror = -7;
	
		if(peol&& (ierror == 0) )
		{
			// Get Winding type from file
			sscanf(pcur, "%d", &pVEntry->iWinding);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol 
			
		}		
		else ierror = -8;
		
		if(peol&& (ierror == 0) )
		{
			// Get Number stacks from file
			sscanf(pcur, "%d", &pVEntry->iStacks);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol 
			
		}		
		else ierror = -9;
		
		if(peol&& (ierror == 0) )
		{
			// Get Split from file
			sscanf(pcur, "%d", &pVEntry->iSplit);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol 
			
		}		
		else ierror = -10;
		
		if(peol&& (ierror == 0) )
		{
			// Get Band 1
			sscanf(pcur, "%d", &pVEntry->iBand1);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol 
			
		}		
		else ierror = -11;
		
		if(peol&& (ierror == 0) )
		{
			// Get Dead Band
			sscanf(pcur, "%d", &pVEntry->iDeadBand);
			pcur = peol + 1; // get past eol
			peol =  strstr(pcur, "\n"); // find next eol 
			
		}		
		else ierror = -12;
		
		if(peol&& (ierror == 0) )
		{
			// Get Get Band 2
			sscanf(pcur, "%d", &pVEntry->iBand2);
			pcur = peol + 1; // get past eol
			// Last field has to EOL
			//peol =  strstr(pcur, "\n"); // find next eol 
			
		}		
		else ierror = -13;
		
		if(peol&& (ierror == 0) )
		{
			// Get Comment from file
			strncpy(pVEntry->pComment, pcur, (pBuffer + BytesRead) - pcur);
			pVEntry->pComment[(pBuffer + BytesRead) - pcur] = 0;
			
		}		
		else ierror = -14;
		
	}
	else ierror = -15;
		
	return ierror;
}
int SetEditVData(VNumberStruct *pData)
{
int ierror;
	ierror = SetCtrlVal (pEditV, EDITV_VNUMBER, pData->pVNumber);
	ierror += SetCtrlVal (pEditV, EDITV_DIAMETER, pData->dDiameter);
	ierror += SetCtrlVal (pEditV, EDITV_ANGLE, pData->iAngle);
	ierror += SetCtrlVal (pEditV, EDITV_CONFORMITY, pData->dConformity);
	ierror += SetCtrlVal (pEditV, EDITV_RESISTANCE, pData->iResistance);
	ierror += SetCtrlVal (pEditV, EDITV_TOLERENCE, pData->iTolerence);
	ierror += SetCtrlVal (pEditV, EDITV_WINDING, pData->iWinding);
	ierror += SetCtrlVal (pEditV, EDITV_STACKS, pData->iStacks);
	ierror += SetCtrlVal (pEditV, EDITV_SPLIT, pData->iSplit);
	ierror += SetCtrlVal (pEditV, EDITV_BAND1, pData->iBand1);
	ierror += SetCtrlVal (pEditV, EDITV_DEADBAND, pData->iDeadBand);
	ierror += SetCtrlVal (pEditV, EDITV_BAND2, pData->iBand2);
	ierror = ResetTextBox (pEditV, EDITV_COMMENT, pData->pComment);
	SplitChange(pEditV, 0, EVENT_COMMIT, NULL, 0,0); // disables / enables split
	return ierror;
}

// Enables disables Split parameter controls
int CVICALLBACK SplitChange (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
int iSplitActive;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal(pEditV, EDITV_SPLIT, &iSplitActive);
			SetCtrlAttribute (pEditV, EDITV_BAND1, ATTR_DIMMED, !iSplitActive);
			SetCtrlAttribute (pEditV, EDITV_DEADBAND, ATTR_DIMMED, !iSplitActive);
			SetCtrlAttribute (pEditV, EDITV_BAND2, ATTR_DIMMED, !iSplitActive);
			break;
		}
	return 0;
}
