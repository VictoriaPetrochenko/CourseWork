#include<iostream> 
#include "..\AES_LIB/Common.h";
#include "..\AES_LIB/CEncoder.h";

using namespace std;


int main()
{
	setlocale(LC_ALL, "Russian");
	//------------------------------------------------
	ui32 arra[4] = { 0x00112233,0x44556677,0x8899aabb,0xccddeeff };
	ui32 keyab[4] = { 0x00010203,0x04050607, 0x08090a0b, 0x0c0d0e0f};
	ui32 arrb[4] = { 0x69c4e0d8, 0x6a7b0430, 0xd8cdb780, 0x70b4c55a };
	CEncoder a;
	a.Receiver(arra);
	a.ReceiverKey(keyab);
	a.Encrypt();
	CEncoder b;
	b.Receiver(arrb);
	b.ReceiverKey(keyab);
	b.Decrypt();
	

	


	//-----------------------------------------------
	system("pause");
	return 0;
}