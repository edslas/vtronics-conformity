#include <ANSI_C.h>
#include <userint.h>
#include "main.h"
#include "DataAquisition.h"
#include "Globals.h"
#include "StepperMotor.h"

#define SPEEDS_FILE_NAME "Speeds.bin"          

extern int pSetup;
extern int pTroubleShoot;
int SetSpeeds(void);
int CVICALLBACK TroubleShootTimer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
int ierror;
double Reading;
	if (GetActivePanel() != panel)
	{
		// don't bother if not on screen.
		return 0;
	}
	switch (event)
		{
		case EVENT_TIMER_TICK:
			// Read Analog Channels and Display 
			ierror = ReadChannel(SUPP_VOLTAGE_CH, &Reading);
			if(ierror == 0)
			{
			   SetCtrlVal (panel, TBLSHOOT_SUPVOLTAGE, Reading);
			}
			else
			{
				SetCtrlVal (panel, TBLSHOOT_SUPVOLTAGE, -99.999);
			}
			ierror = ReadChannel(CONFORMITY_CH, &Reading);
			if(ierror == 0)
			{
			   SetCtrlVal (panel, TBLSHOOT_CONFORMITY, Reading);
			}
			else
			{
				SetCtrlVal (panel, TBLSHOOT_CONFORMITY, -99.999);
			}
			// Read Stepper Motor Speed
			ierror = IMS_GetVelocity(1, &Reading); 
			if(ierror == 0)
			{
			   SetCtrlVal (panel, TBLSHOOT_SPEED, Reading);
			}
			else
			{
				SetCtrlVal (panel, TBLSHOOT_SPEED, -99.999);
			}
			break;
		}
	return 0;
}

int CVICALLBACK ExitTroubleShoot (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			HidePanel (pTroubleShoot);
			DisplayPanel (pSetup);
			 
			break;
		}
	return 0;
}

int CVICALLBACK StartTroubleShootMove (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
double dDistance;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel, TBLSHOOT_MOTOR_DEG, &dDistance); 
			SetSpeeds();
			IMS_RelativeMove(1, dDistance);
			break;
		}
	return 0;
}
int SetSpeeds(void)
{
int ierror;
char pCommand[64];
double dValue;

   	GetCtrlVal (pTroubleShoot, TBLSHOOT_MOTOR_VI, &dValue);
   	sprintf(pCommand, "vi=%0.3lf",dValue);
   	ierror = IMS_SendCommand(1,pCommand, NULL);  
   	
   	GetCtrlVal (pTroubleShoot, TBLSHOOT_MOTOR_VM, &dValue);
   	sprintf(pCommand, "vm=%0.3lf",dValue);
   	ierror += IMS_SendCommand(1,pCommand, NULL);
   	
   	GetCtrlVal (pTroubleShoot, TBLSHOOT_MOTOR_ACCL, &dValue);
   	sprintf(pCommand, "accl=%0.3lf",dValue);
   	ierror += IMS_SendCommand(1,pCommand, NULL);
   	
   	return ierror;
}
int SetDefaultSpeeds(void)
{
int ierror;
	ierror = RecallPanelState (pTroubleShoot, SPEEDS_FILE_NAME, 0);
	SetSpeeds();
	return ierror;
}

int SaveSpeeds(void)
{
int ierror;
	ierror = SavePanelState (pTroubleShoot, SPEEDS_FILE_NAME, 0);
	return ierror;
}
int CVICALLBACK SaveDefaultSpeeds (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			if(SaveSpeeds() == 0)
			{
				MessagePopup ("Confirmation", "Data was Saved");
			}
			else
			{
				MessagePopup ("Error", "Could not save data");
			}
			break;
		}
	return 0;
}
