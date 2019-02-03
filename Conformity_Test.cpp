#include <afxdb.h>
#include "Globals.h"
#include "VNumbersDB.h"
#include "DB_IO.h"
#include <windows.h>

//#include <UTILITY.h>
#include <ANSI_C.h>
#include <userint.h>
#include "main.h"
#include "Utility.h"
#include "steppermotor.h"
#include "DataAquisition.h"
#include "Troubleshoot.h"

extern int pSetup;
extern int pConformityTest;
extern VNumberStruct CurrentVNumber;
extern ConformityDataStruct CurrentPartData;
#define AVE_WINDOW	1

int PlotData(double *xData, double *yData, double *lyData, double *DeviationData, int iNumberOfPoints, int iOffset);
int CalculateDeviation(double *xData, double *yData, double *lyData, double *DeviationData, double *dMax, double *dMaxIndex, int iNumberOfPoints, int iOffset);
int WriteResultsToDB(double dMax);
void UpdateResultList(char *pSerialNumber, double dMax, double dMaxIndex);
void IncrementSerialNumber(char *pSerialNumber);
int FindFlats(double *yData, int *iOffset, int *iLength);
int FindDeadBand(double *yData, int *iOffset, int *iLength);
int CheckUserData(void);
int SetStatusLights(int iStatus);
int SmoothData(double *Data, int Length, int Window);
int CheckReady(void);
int CheckOutputRange(double *Data, int iNumberOfPoints);

int CVICALLBACK StartTest (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
char pSerialNumber[32];
char pBuffer[128];
int ierror;
double xData[MAX_NUMBER_POINTS]; 
double yData[MAX_NUMBER_POINTS];
double lyData[MAX_NUMBER_POINTS];
double DeviationData[MAX_NUMBER_POINTS];
double dMax, dMax2;
double dMaxIndex, dMaxIndex2;
int iNumberOfPoints;
int iOffset, iLength;
int iBand1Length, iBand2Length, iDeadLength;
double dtemp;

	if(event != EVENT_COMMIT) return 0;
	if(CheckUserData() != 0) return 0;
	if(CheckReady() != 0) return 0;

	ierror = GetCtrlVal (pSetup, SETUP_SERIALENTRY, pSerialNumber);
	// execute Test
	SetStatusLights(STATUS_IN_PROGRESS);

	memset(yData, 0, sizeof(yData));
	
	dtemp = ((double)((CurrentVNumber.iAngle + CONFORMITY_OVERSHOOT)))*CONFORMITY_POINTS_PER_DEGREE;
	iNumberOfPoints = (int)dtemp;
	if(iNumberOfPoints > MAX_NUMBER_POINTS) iNumberOfPoints = MAX_NUMBER_POINTS;

	// Initialize motor
	IMS_SendCommand(1, "MUNIT=51200/360", NULL);
	SetDefaultSpeeds();
	// Start Motor
	IMS_RelativeMove(1, CurrentVNumber.iAngle + CONFORMITY_OVERSHOOT);	
	// Read Data
	ierror = ReadFromTrigger(yData , iNumberOfPoints);

	if(ierror)
	{
		SetStatusLights(STATUS_IN_FAIL);
		sprintf(pBuffer, "Error Reading Data = %d", ierror);
		AfxMessageBox(pBuffer, MB_OK,0);
		return 0;
	}
	
	if(CheckOutputRange(yData, iNumberOfPoints) != 0) 
	{
		SetStatusLights(STATUS_IN_FAIL);
		return 0;
	}

	iLength = iNumberOfPoints;

	

	FindFlats(yData, &iOffset, &iLength);
	
	

	iNumberOfPoints = iLength;
	if(iNumberOfPoints == 0)
	{
		SetStatusLights(STATUS_IN_FAIL);
		sprintf(pBuffer, "Could Not enough data");
		AfxMessageBox(pBuffer, MB_OK,0);
		return 0;
	}
	if(CurrentVNumber.iSplit)
	{
		iBand1Length = (int)(CurrentVNumber.iBand1 * CONFORMITY_POINTS_PER_DEGREE);
		iDeadLength = (int)(CurrentVNumber.iDeadBand * CONFORMITY_POINTS_PER_DEGREE);
		iBand2Length = (int)(CurrentVNumber.iBand2 * CONFORMITY_POINTS_PER_DEGREE);
		iNumberOfPoints = iBand1Length + iDeadLength + iBand2Length;
		// Band 1
	

		ierror = CalculateDeviation(
			xData, 
			yData, 
			lyData, 
			DeviationData, 
			&dMax, 
			&dMaxIndex, 
			iBand1Length, // just up to end of band 1 
			iOffset);
		// Dead Band
		ierror += CalculateDeviation(
			xData, 
			yData, 
			lyData, 
			DeviationData, 
			&dMax2, 
			&dMaxIndex2, 
			iDeadLength, // length 
			iOffset + iBand1Length);
		// Band 
		
		ierror += CalculateDeviation(
			xData, 
			yData, 
			lyData, 
			DeviationData, 
			&dMax2, 
			&dMaxIndex2, 
			iBand2Length, // length 
			iOffset + iBand1Length + iDeadLength);
		if(dMax2 > dMax) // use greater of two bands
		{
			dMax = dMax2;
			dMaxIndex = dMaxIndex2;
		}
	}
	else
	{
		
		ierror = CalculateDeviation(xData, yData, lyData, DeviationData, &dMax, &dMaxIndex, iNumberOfPoints, iOffset);
	}
	
	if(ierror)
	{
		SetStatusLights(STATUS_IN_FAIL);
		sprintf(pBuffer, "Bad Data Error= %d", ierror);
		AfxMessageBox(pBuffer, MB_OK,0);
		return 0;
	}
	PlotData(xData, yData, lyData, DeviationData, iNumberOfPoints, iOffset);
	
	// Turn off in progress + Set pass fail
	SetCtrlVal (pSetup, SETUP_INPROGRESS, 0);
	if(dMax <= CurrentVNumber.dConformity)
	{
		SetStatusLights(STATUS_IN_PASS);   	
	}
	else
	{
		SetStatusLights(STATUS_IN_FAIL);   	 
	}
	
	// Write to DataBase
	ierror = WriteResultsToDB(dMax);
	UpdateResultList(pSerialNumber, dMax, dMaxIndex);
	IncrementSerialNumber(pSerialNumber);
	
	return 0;
}

int CVICALLBACK StopTest (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
int ierror;
	switch (event)
		{
		case EVENT_COMMIT:
			// Clear out results + Serial Number
			ierror = ClearListCtrl (pSetup, SETUP_RESULTS);
			ierror = SetCtrlVal (pSetup, SETUP_SERIALENTRY, "");		

			break;
		}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////
// PlotData
//
//	Draws both graphs given
///////////////////////////////////////////////////////////////////////////////////////////////
int PlotData(double *xData, double *yData, double *lyData, double *DeviationData, int iNumberOfPoints, int iOffset)
{
int ierror;
double xDataShifted[MAX_NUMBER_POINTS]; 
int i;
		
	// Shift X Data for display Purposes
	for(i = iOffset; i < iNumberOfPoints + iOffset; i++)
	{
		xDataShifted[i] = xData[i] - xData[iOffset];
	}
	// ouput ratio 
	// Clear old graph
	ierror = DeleteGraphPlot (pSetup, SETUP_OUTPUTRATIO, -1, VAL_IMMEDIATE_DRAW);
	// Draw real data
	PlotXY (pSetup, SETUP_OUTPUTRATIO, &xDataShifted[iOffset], &yData[iOffset], iNumberOfPoints,
						 VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE,
						 VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
	// draw linear fit
	PlotXY (pSetup, SETUP_OUTPUTRATIO, &xDataShifted[iOffset], &lyData[iOffset], iNumberOfPoints,
						 VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE,
						 VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLACK);		
				
	// Deviation 
	// Delete old
	ierror += DeleteGraphPlot (pSetup, SETUP_DEVIATION, -1, VAL_IMMEDIATE_DRAW);
	// draw dots
	PlotXY (pSetup, SETUP_DEVIATION, &xDataShifted[iOffset], &DeviationData[iOffset],
								  iNumberOfPoints, VAL_DOUBLE,
								  VAL_DOUBLE, VAL_SCATTER, VAL_SOLID_CIRCLE,
								  VAL_SOLID, 1, VAL_BLACK);
	return ierror;
}
///////////////////////////////////////////////////////////////////////////////////////////////
// CalculateDeviation
//
//	Calculates best linear fit
//	Fills in DeviationData = abs(yData - lyData)
//  Calculates Max deviation and the index it happens at.
///////////////////////////////////////////////////////////////////////////////////////////////
int CalculateDeviation(double *xData, double *yData, double *lyData, double *DeviationData, double *dMax, double *dMaxIndex, int iNumberOfPoints, int iOffset)
{
int ierror,i;
double dSlope, dOffset, dMSE;
	//SmoothData(&yData[iOffset], iNumberOfPoints, AVE_WINDOW);
	ierror = BestLinearFit( &yData[iOffset], iNumberOfPoints, &dSlope, &dOffset, &dMSE); 
	if(ierror)
	{
		return ierror;
	}
	
	for (i = iOffset ; i < iNumberOfPoints + iOffset; i++)
	{
		xData[i] = ((double)i) / CONFORMITY_POINTS_PER_DEGREE; // translate to degrees
		lyData[i] = (dSlope*(i-iOffset)) + dOffset;
		DeviationData[i] =  lyData[i] -  yData[i];
	}
	SmoothData(&DeviationData[iOffset], iNumberOfPoints, AVE_WINDOW);
	
	for (i = iOffset ; i < iNumberOfPoints + iOffset; i++)
	{
		if(i == iOffset)
		{
			*dMax = fabs(DeviationData[i]);
			*dMaxIndex = xData[i] - xData[iOffset];
		}
		else
		{
			if (fabs(DeviationData[i]) > *dMax)
			{
				*dMax = fabs(DeviationData[i]);
				*dMaxIndex = xData[i] - xData[iOffset];
			}
		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////
// WriteResultsToDB
//
//	Uses global data and result to make data base entry
///////////////////////////////////////////////////////////////////////////////////////////////
int WriteResultsToDB(double dMax)
{
CDB_IO DB;
int ierror;
char pSerialNumber[128];

	// load CurrentPartData struct
	ierror = GetCtrlVal (pSetup, SETUP_PO, CurrentPartData.pPO);	
	ierror = GetCtrlVal (pSetup, SETUP_OPERATOR, CurrentPartData.pOperator);
	
	// Make initials upper case
	CurrentPartData.pOperator[0] = toupper(CurrentPartData.pOperator[0]);
	CurrentPartData.pOperator[1] = toupper(CurrentPartData.pOperator[1]);
	CurrentPartData.pOperator[2] = toupper(CurrentPartData.pOperator[2]);

	ierror = GetCtrlVal (pSetup, SETUP_CUSTENTRY, CurrentPartData.pCustomer);	
	ierror = GetCtrlVal (pSetup, SETUP_VENTRY, CurrentPartData.pVNumber);	
	ierror = GetCtrlVal (pSetup, SETUP_SERIALENTRY, pSerialNumber);
	strcpy(CurrentPartData.pSerialNumber, pSerialNumber);

	CurrentPartData.dConformity = dMax;
	CurrentPartData.dResistance = 0.0;
	// write to screen
	ierror = DB.WriteConformityData(CurrentPartData);

	return ierror;
}
///////////////////////////////////////////////////////////////////////////////////////////////
// UpdateResultList
//
//	Builds string to add to result list
///////////////////////////////////////////////////////////////////////////////////////////////
void UpdateResultList(char *pSerialNumber, double dMax, double dMaxIndex)
{
int ierror;
char pBuffer[64];
int iCount;
	// efs edit sprintf(pBuffer, "%s\033p050l%1.2lf@%1.1lf",pSerialNumber, dMax, dMaxIndex);
sprintf(pBuffer, "%s\033p050l%1.2lf",pSerialNumber, dMax);


	// Make sure list is not too long
	GetNumListItems(pSetup, SETUP_RESULTS, &iCount);
	if(iCount > 50)
	{
		DeleteListItem(pSetup, SETUP_RESULTS, 0, 10);
	}
	// Add to list				
	ierror = InsertListItem (pSetup, SETUP_RESULTS, -1,
			 pBuffer, NULL);
	ierror = GetNumListItems (pSetup, SETUP_RESULTS, &iCount);              
	// Set current selection to last added
	ierror = SetCtrlIndex (pSetup, SETUP_RESULTS, iCount -1);
}
///////////////////////////////////////////////////////////////////////////////////////////////
// IncrementSerialNumber
//
//	Auto increment serial number if single stack
///////////////////////////////////////////////////////////////////////////////////////////////
void IncrementSerialNumber(char *pSerialNumber)
{
int ierror;
int iSerial;
	if(CurrentVNumber.iStacks == 1)
	{
		iSerial = atoi (pSerialNumber);
		iSerial = iSerial + 1;
		sprintf(pSerialNumber, "%d", iSerial);
		ierror = SetCtrlVal (pSetup, SETUP_SERIALENTRY, pSerialNumber);		
	}
}
int FindFlats(double *yData, int *iOffset, int *iLength)
{
int i;
double dAverageDifference = 0.0;
double dDifference;
int start = 0;
int end = 0;
int Length;
int AngleInPoints;
int	WindowDeficit, WindowSurplus;
bool bCanAdjustStart = true;
bool bCanAdjustEnd = true;
	AngleInPoints = (int)(CurrentVNumber.iAngle * CONFORMITY_POINTS_PER_DEGREE) + 1;
	// calculate average difference between points.
	for(i = 0; i < *iLength - 1; i++)
	{
		dAverageDifference += fabs(yData[i] - yData[i+1]);
	}
	dAverageDifference = dAverageDifference / (double)i;
	
	// See when average Difference starts
	for(i = 1; (i < *iLength - 2) && (start == 0); i++)
	{
		dDifference = fabs(yData[i] - yData[i+1]);
		if(dDifference > (dAverageDifference/1.25))
		{
			if(dDifference < (dAverageDifference*3.0))
			start = i;
		}
	}
	end = *iLength - 1;
	for(i = *iLength - 2; ((i > 0) && (end == *iLength - 1)); i--)
	{
		dDifference = fabs(yData[i] - yData[i-1]);
		if(dDifference > (dAverageDifference/2.0))
		{
			end = i;
		}
	}

	Length = end - start;
	// Open window to spec IF NEEDED.
	WindowDeficit = AngleInPoints - Length;
	while((WindowDeficit < 0) && bCanAdjustStart && bCanAdjustEnd)
	{
		
		if(start > 0)
		{
			start--;
			bCanAdjustStart = true;
		}
		else
		{
			bCanAdjustStart = false;
		}
		Length = end - start;
		WindowDeficit = AngleInPoints - Length;
		if((end < *iLength) && (WindowDeficit < 0))
		{
			end++;
			bCanAdjustEnd = true;
		}
		else
		{
			bCanAdjustEnd = false;
		}
		Length = end - start;
		WindowDeficit = AngleInPoints - Length;
	}
	// CLOSE window to spec IF NEEDED.
	Length = end - start;
	WindowSurplus = Length - AngleInPoints;

	while(WindowSurplus > 0 )
	{
		start+=2;
		
		Length = end - start;
		WindowSurplus = Length - AngleInPoints;
		if(WindowSurplus > 0)
		{
			end--;
		
			Length = end - start;
			WindowSurplus = Length - AngleInPoints;
		}
	}
	*iOffset = start;
	*iLength = end - start;
	return 0;
}
int FindDeadBand(double *yData, int *iOffset, int *iLength)
{
// Finds the dead band in the data by correlation
// NOT USED AT THIS TIME
int iNumPointsInDead;
double Ave, MSE, minMSE;
int Start, i; 
int StartOfDead = 0;
	minMSE = -1;
	iNumPointsInDead = (int)(CurrentVNumber.iDeadBand * CONFORMITY_POINTS_PER_DEGREE);
	if(iNumPointsInDead <= 0) return -1;
	// step through the data
	for(Start = *iOffset; Start < *iLength + *iOffset- iNumPointsInDead; Start++)
	{
		MSE = 0;
		// Calculate best fit strait line for this range
		for(i = Start; i < Start + iNumPointsInDead; i++)
		{
			Ave += yData[i];
		}
		Ave = Ave / iNumPointsInDead;
		// Calculate MSE from strait line in this range
		for(i = Start; i < Start + iNumPointsInDead; i++)
		{
			MSE = (yData[i] - Ave)*(yData[i] - Ave);
		}
		MSE = MSE / iNumPointsInDead;
		// compare 
		if(MSE < minMSE)
		{
			minMSE = MSE;
			StartOfDead = Start;
		}
	}
	*iOffset = StartOfDead;
	*iLength = iNumPointsInDead;
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////
// CheckUserData
//
//	Verifies that user input is valid. 
///////////////////////////////////////////////////////////////////////////////////////////////
int CheckUserData(void)
{
char pSerialNumber[32];

	GetCtrlVal (pSetup, SETUP_SERIALENTRY, pSerialNumber);
	if(strlen(pSerialNumber) == 0)
	{
		MessagePopup ("Error", "A Serial Number is Required");
		return -1;
	}
	if(CurrentVNumber.bValidData == false)
	{
		MessagePopup ("Error", "The VNumber must be Valid");
		return -2;
	}
	
return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////
// CheckUserData
//
//	Verifies that user input is valid. 
///////////////////////////////////////////////////////////////////////////////////////////////
int SetStatusLights(int iStatus)
{
	switch(iStatus)
	{
		case  STATUS_IN_IDLE:
			SetCtrlVal (pSetup, SETUP_INPROGRESS, 0);
			SetCtrlVal (pSetup, SETUP_PASS, 0);
			SetCtrlVal (pSetup, SETUP_FAIL, 0);       
			break;
		case  STATUS_IN_PROGRESS:
			SetCtrlVal (pSetup, SETUP_INPROGRESS, 1);
			SetCtrlVal (pSetup, SETUP_PASS, 0);
			SetCtrlVal (pSetup, SETUP_FAIL, 0);  
			break;
		case  STATUS_IN_PASS:
			SetCtrlVal (pSetup, SETUP_INPROGRESS, 0);
			SetCtrlVal (pSetup, SETUP_PASS, 1);
			SetCtrlVal (pSetup, SETUP_FAIL, 0);  
			break;
		case  STATUS_IN_FAIL:
			SetCtrlVal (pSetup, SETUP_INPROGRESS, 0);
			SetCtrlVal (pSetup, SETUP_PASS, 0);
			SetCtrlVal (pSetup, SETUP_FAIL, 1);  
			for(int i = 0; i < 20; i++)
			{
				::Beep(10000,1); 
				::Beep(5000,1);
			}

			break;
	}
	return 0;
}
int SmoothData(double *Data, int Length, int Window)
{
int i, j, count;
double sum;
double *Temp;
	if(Window <= 0) return -1;
	if(Length <= 0) return -2;
	Temp = (double *)malloc(Length*sizeof(double));
	if(!Temp) return -3;

	for(i = 0; i < 	Length; i++)
	{
		sum = 0.0;
		count = 0;
		for(j = i - Window ; j < i + Window; j++)
		{
			if((j < Length) && (j >= 0))
			{
				sum += Data[j];
				count++;
			}
		}
		if(count > 0) // beware of divide by 0
		{
			Temp[i] = sum / (double)count; // compute average + assign
		}
		else
		{
			Temp[i] = Data[i];
		}
	}
	memcpy(Data, Temp, Length*sizeof(double));
	free(Temp);
	return 0;
}
int CheckReady(void)
{
double SupplyVoltage;

	ReadChannel(SUPP_VOLTAGE_CH, &SupplyVoltage);
	if(SupplyVoltage < 4.5)
	{
		AfxMessageBox("Test Supply Voltage less than 4.5 V."
						"Test not started!\n"
						"Check for short on Red + Black leads\n"
						"Also try shorting Red + Black leads for 1 second to reset power supply.", MB_OK,0);
		return -1;
	}
	else
	{
		return 0;
	}
}
int CheckOutputRange(double *Data, int iNumberOfPoints)
{
int i;
bool bMinFound = false,bMaxFound = false;

	// make sure output goes from at least 20 - 80 %
	for( i = 0; (i < iNumberOfPoints) && !(bMinFound && bMaxFound); i++)
	{
		if(Data[i] < 20) bMinFound = true;
		if(Data[i] > 80) bMaxFound = true;
	}
	if(bMinFound && bMaxFound)
	{
		return 0;
	}
	else
	{
		AfxMessageBox("Test Failed. Output Ratio deviation too small."
						"Check for short on Red/Black with Green lead\n"
						"Check for mechanical linkage with part\n"
						"Check Stepper motor operation / Supply in Trouble Shooting", MB_OK,0);
		
		return -1;
	}

}