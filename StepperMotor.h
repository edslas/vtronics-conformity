extern int IMS_Open(int iCommPort);
extern int IMS_Close(int iCommPort); 
extern int IMS_SendCommand(int iCommPort, char *pCommand, char *pResponse); 
extern int IMS_GetVelocity(int iCommPort, double *dpVelocity);
extern int IMS_RelativeMove(int iCommPort, double dDegrees);

