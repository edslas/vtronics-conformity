#include <userint.h>
#include "main.h"
#include <dataacq.h>
#include "Globals.h"
#include <stdlib.h>

//--------------------------------------------------------------------------
//InitializeDAQ
//--------------------------------------------------------------------------
// Initializes the data aquisition board for this application
int SetUpExternalTrigger(void);

int InitializeDAQ(void)
{
int ierror; 
short iBoardType;
int ireturnval = 0;

	ierror = Init_DA_Brds (1, &iBoardType);
	if(ierror == 0)
	{
		if(iBoardType != 36)
		{
			ierror = 1;
		}
	}
	else
	{
		ireturnval = ierror;
	}
	
	// Configure channels for input type, range, etc.
	if(ierror == 0)
	{
		 
		ierror = AI_Configure (
							1 					// Board #
							,SUPP_VOLTAGE_CH	// Channel
							,0					// 0=differ, 1=RSE, 2 = NRSE 
							,5					// Input Range 5,10,20
							,1					// 0 = bipolar, 1 = Unipolar
							,0					// Drive AISense
							);
		
	}
	if(ierror == 0)
	{
		// Configure channels for 
		ierror = AI_Configure (
							1 					// Board #
							,CONFORMITY_CH		// Channel
							,0					// 0=differ, 1=RSE, 2 = NRSE 
							,5					// Input Range 5,10,20
							,1					// 0 = bipolar, 1 = Unipolar
							,0					// Drive AISense
							);
		
	}
	
	return ierror;
}
//--------------------------------------------------------------------------
//ReadChannel
//--------------------------------------------------------------------------
int ReadChannel(int Channel, double *Reading)
{
int ierror = 0;
	// Set cnvert back to auto see SetUpExternalTrigger()
	ierror = Select_Signal (1, ND_IN_CONVERT, ND_INTERNAL_TIMER, ND_LOW_TO_HIGH);
	if(ierror == 0)
	{
		ierror = AI_VRead (1, Channel, 1, Reading);
	}
	return ierror;
																	 
																	  
}
//--------------------------------------------------------------------------
//ReadChannel
//--------------------------------------------------------------------------
// Tells Data Aq board to samle analog channel from PFI0, the encoder
int SetUpExternalTrigger(void)
{
int ierror = 0;

	ierror = Select_Signal (1, ND_IN_CONVERT, ND_PFI_0, ND_HIGH_TO_LOW);
	return ierror;
}
//--------------------------------------------------------------------------
//ReadChannel
//--------------------------------------------------------------------------
// Tells Data Aq board to samle analog channel from PFI0, the encoder
int ReadFromTrigger(double *dBuffer, int iNumPoints)
{
int ierror = 0;
short sBuffer[MAX_NUMBER_POINTS];

	ierror = SetUpExternalTrigger();
	if(ierror == 0)
	{
		Timeout_Config (
						1,	//short Board, 
						10*18	//long Timeout (18 units per second);
						);
		ierror = DAQ_Op (1, 1, 2, sBuffer, iNumPoints, 1.0);
	}
	if(ierror == 0)
	{
		for(int i = 0; i < iNumPoints; i++)
		{
			dBuffer[i] = ((double)sBuffer[i])*100.0 / 4096.0;
		}
	}
	else
	{
		InitializeDAQ();
	}
	return ierror;
}