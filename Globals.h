#if !defined(AFX_DB_GLOBALS_H__815D1A20_155A_11D5_B177_00E029467F2C__INCLUDED_)
#define AFX_DB_GLOBALS_H__815D1A20_155A_11D5_B177_00E029467F2C__INCLUDED_

#define V_FILE_PATH "C:\\VOLTRONICS\\VNUMBERS\\"  
#define SUCCESS 0;
#define V_NUMBER_LENGTH 32
#define COMMENT_LENGTH 256
#define SUPP_VOLTAGE_CH 	0
#define CONFORMITY_CH 		1
#define MAX_CUSTOMER_LENGTH 64
#define MAX_PO_LENGTH 32
#define MAX_SERIAL_LENGTH 32
#define MAX_OPERATOR_LENGTH 4

#define STATUS_IN_IDLE		0
#define STATUS_IN_PROGRESS	1
#define STATUS_IN_PASS		2
#define STATUS_IN_FAIL		3

#define CONFORMITY_POINTS_PER_DEGREE	((double)(500.0/360.0))
#define MAX_NUMBER_POINTS				512
#define CONFORMITY_OVERSHOOT			10 // degrees
typedef struct 
{
	char pVNumber[V_NUMBER_LENGTH]; 
	double dDiameter;
	double dConformity;
	int iTolerence;
	int iAngle;
	int iResistance;
	int iWinding;
	int iStacks;
	int iSplit;
	int iBand1;
	int iDeadBand;
	int iBand2;
	char pComment[COMMENT_LENGTH];
	bool bValidData;
}VNumberStruct;

typedef struct
{
	char pCustomer[MAX_CUSTOMER_LENGTH];
	char pPO[MAX_PO_LENGTH]; 
	char pSerialNumber[MAX_SERIAL_LENGTH];
	char pVNumber[V_NUMBER_LENGTH];
	double dResistance;
	double dConformity;
	char pOperator[MAX_OPERATOR_LENGTH];
}ConformityDataStruct;
#endif