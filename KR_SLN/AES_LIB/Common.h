#pragma once
typedef unsigned char ui8;
typedef unsigned int ui32;

typedef struct
{
	ui8 byte0 : 8;
	ui8 byte1 : 8;
	ui8 byte2 : 8;
	ui8 byte3 : 8;
}SUI32AsStruct,*pSUI32AsStruct;

typedef union
{
	SUI32AsStruct _byteStr;
	ui32 _ui32;
}U_4Bytes;