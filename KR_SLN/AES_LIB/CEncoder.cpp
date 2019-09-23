#include "CEncoder.h"

CEncoder::CEncoder()//конструктор без параметра, задает значения _Nb, _Nk, _Nr для ключа равного 128 бит
{
	_Nb = 4;
	_Nk = 4;
	_Nr = 10;
	_fErrorLengthKey = false;
	ResetSMatrix();
}

CEncoder::CEncoder(int lengthKey)//конструктор с параметром, задает значения _Nb, _Nk, _Nr в зависимости от длины ключа
{
	_Nb = 4;
	_fErrorLengthKey = false;
	switch (lengthKey)
	{
	case 128:
		_Nk = 4; _Nr = 10; break;
	case 192:
		_Nk = 6; _Nr = 12; break;
	case 256:
		_Nk = 8; _Nr = 14; break;
	default: _Nk = 4; _Nr = 10; _fErrorLengthKey = true;
	}
	ResetSMatrix();
}

void CEncoder::ResetSMatrix()//метод, заполняющий матрицу состояния нулями
{
	for (int i = 0; i<_Nb; i++)
	{
		_stateMatrix[i]._ui32 = 0;
	}
}

ui8 CEncoder::GetByte(int nRow, int nColumn)//метод, который получает байт из матрицы состояния
{
	U_4Bytes tmp = _stateMatrix[nColumn];
	switch (nRow)
	{
	case 0:  return tmp._byteStr.byte0;
	case 1:  return tmp._byteStr.byte1;
	case 2:  return tmp._byteStr.byte2;
	case 3:  return tmp._byteStr.byte3;
	}
}

void CEncoder::SetByte(int nRow, int nColumn, ui8 data)//метод, который заменяется значение указанного байта
{
	switch (nRow)
	{
	case 0: _stateMatrix[nColumn]._byteStr.byte0 = data; break;
	case 1: _stateMatrix[nColumn]._byteStr.byte1 = data; break;
	case 2: _stateMatrix[nColumn]._byteStr.byte2 = data; break;
	case 3: _stateMatrix[nColumn]._byteStr.byte3 = data; break;
	}
}

void CEncoder::ExpansionKey()//метод расширения ключа шифрования
{
	U_4Bytes temp;
	for (int i = 0; i<_Nk; i++)
	{
		_w[i]._ui32 = _key[i]._ui32;
	}
	for (int i = _Nk; i<_Nb*(_Nr + 1); i++)
	{
		temp = _w[i - 1];
		if (i % _Nk == 0)
		{
			temp._ui32 = SubWord(RotWord(temp))._ui32 ^ Rcon[i / _Nk];
		}
		else if ((_Nk > 6) && (i % _Nk == 4))
		{
			temp = SubWord(temp);
		}
		_w[i]._ui32 = _w[i - _Nk]._ui32 ^ temp._ui32;
	}
}

void CEncoder::AddRoundKey(int param)//метод сложения матрицы состояния с раундовым ключом
{
	for (int i = 0; i<_Nb; i++)
	{
		_stateMatrix[i]._ui32 = _stateMatrix[i]._ui32 ^ _w[i + param*_Nb]._ui32;
	}
}

void CEncoder::SubBytes()//метод замены байт в матрице состояния при помощи S-блока
{
	int N = 0x00000010;
	ui8 tmp;
	for (int i = 0; i<_Nb; i++)
	{
		for (int j = 0; j<_Nb; j++)
		{
			tmp = GetByte(i, j);
			SetByte(i, j, _SBlock[tmp % 16 + N*(tmp / 16)]);
		}
	}
}

void CEncoder::InvSubBytes()//метод, обратный методу SubBytes(), заменяет байты матрицы состояния при помощи обратного S-блока
{
	int N = 0x00000010;
	ui8 tmp;
	for (int i = 0; i<_Nb; i++)
	{
		for (int j = 0; j<_Nb; j++)
		{
			tmp = GetByte(i, j);
			SetByte(i, j, _InvSBlock[tmp % 16 + N*(tmp / 16)]);
		}
	}
}

void CEncoder::ShiftRows()//метод циклического сдвига байт матрицы состояния влево
{
	int l;
	for (int i = _Nb - 1; i >= 0; i--)
	{
		l = _Nb - i - 1;
		ui8 *row = new ui8[_Nb];
		ui8 *rowCopy = new ui8[_Nb];
		for (int j = 0; j<_Nb; j++)
		{
			*(row + j) = GetByte(i, j);
			*(rowCopy + j) = GetByte(i, j);
		}
		for (int j = 0; j<_Nb - l; j++)
		{
			row[j] = rowCopy[j + l];
		}
		for (int j = 0; l >= 1; j++, l--)
		{
			row[_Nb - l] = rowCopy[j];
		}
		for (int j = 0; j<_Nb; j++)
		{
			SetByte(i, j, row[j]);
		}
	}
}

void CEncoder::InvShiftRows()//метод, обратный методу ShiftRows(), циклически сдвигает байты матрицы состояния вправо
{
	int l;
	for (int i = _Nb - 1; i >= 0; i--)
	{
		l = _Nb - 1 - i;
		l = _Nb - l;
		ui8 *row = new ui8[_Nb];
		ui8 *rowCopy = new ui8[_Nb];
		for (int j = 0; j<_Nb; j++)
		{
			*(row + j) = GetByte(i, j);
			*(rowCopy + j) = GetByte(i, j);
		}
		for (int j = 0; j<_Nb - l; j++)
		{
			row[j] = rowCopy[j + l];
		}
		for (int j = 0; l >= 1; j++, l--)
		{
			row[_Nb - l] = rowCopy[j];
		}
		for (int j = 0; j<_Nb; j++)
		{
			SetByte(i, j, row[j]);
		}
	}
}

void CEncoder::MixColumns()//метод умножения каждого столбца матрицы состояния на фиксированный многочлен a(x)
{

	U_4Bytes _newstateMatrix[4];
	for (int i = 0; i<_Nb; i++)
	{
		U_4Bytes a = _stateMatrix[i];
		_newstateMatrix[i] = a;

		SetByte(3, i, Multiply(0x02, _newstateMatrix[i]._byteStr.byte3) ^ Multiply(0x03, _newstateMatrix[i]._byteStr.byte2) 
			^ _newstateMatrix[i]._byteStr.byte1 ^ _newstateMatrix[i]._byteStr.byte0);
		SetByte(2, i, _newstateMatrix[i]._byteStr.byte3 ^ Multiply(0x02, _newstateMatrix[i]._byteStr.byte2) 
			^ Multiply(0x03, _newstateMatrix[i]._byteStr.byte1) ^ _newstateMatrix[i]._byteStr.byte0);
		SetByte(1, i, _newstateMatrix[i]._byteStr.byte3 ^_newstateMatrix[i]._byteStr.byte2 
			^ Multiply(0x02, _newstateMatrix[i]._byteStr.byte1) ^ Multiply(0x03, _newstateMatrix[i]._byteStr.byte0));
		SetByte(0, i, Multiply(0x03, _newstateMatrix[i]._byteStr.byte3) ^ _newstateMatrix[i]._byteStr.byte2 
			^ _newstateMatrix[i]._byteStr.byte1 ^ Multiply(0x02, _newstateMatrix[i]._byteStr.byte0));
	}
}

void CEncoder::InvMixColumns()//метод, обратный методу MixColumns(), умножает каждый столбец матрицы состояния на обратный к многочлену a(x)
{
	U_4Bytes _newstateMatrix[4];
	for (int i = 0; i<_Nb; i++)
	{
		U_4Bytes a = _stateMatrix[i];
		_newstateMatrix[i] = a;
		SetByte(3, i, Multiply(0x0e, _newstateMatrix[i]._byteStr.byte3) ^ Multiply(0x0b, _newstateMatrix[i]._byteStr.byte2) 
			^ Multiply(0x0d, _newstateMatrix[i]._byteStr.byte1) ^ Multiply(0x09, _newstateMatrix[i]._byteStr.byte0));
		SetByte(2, i, Multiply(0x09, _newstateMatrix[i]._byteStr.byte3) ^ Multiply(0x0e, _newstateMatrix[i]._byteStr.byte2) 
			^ Multiply(0x0b, _newstateMatrix[i]._byteStr.byte1) ^ Multiply(0x0d, _newstateMatrix[i]._byteStr.byte0));
		SetByte(1, i, Multiply(0x0d, _newstateMatrix[i]._byteStr.byte3) ^ Multiply(0x09, _newstateMatrix[i]._byteStr.byte2) 
			^ Multiply(0x0e, _newstateMatrix[i]._byteStr.byte1) ^ Multiply(0x0b, _newstateMatrix[i]._byteStr.byte0));
		SetByte(0, i, Multiply(0x0b, _newstateMatrix[i]._byteStr.byte3) ^ Multiply(0x0d, _newstateMatrix[i]._byteStr.byte2) 
			^ Multiply(0x09, _newstateMatrix[i]._byteStr.byte1) ^ Multiply(0x0e, _newstateMatrix[i]._byteStr.byte0));
	}
}

ui8 CEncoder::Multiply(ui8 a, ui8 b)//метод перемножения двух многочленов в поле Галуа
{
	ui8 p = 0;
	ui8 hi_bit_set;
	for (int i = 0; i < 8; i++) 
	{
		if (b & 1)
			p ^= a;
		hi_bit_set = (a & 0x80);
		a <<= 1;
		if (hi_bit_set)
			a ^= 0x1b; /* x^8 + x^4 + x^3 + x + 1 */
		b >>= 1;
	}
	return p;
}

U_4Bytes CEncoder::SubWord(U_4Bytes w)//метод замены при помощи S-блока слова w
{
	int N = 0x00000010;
	w._byteStr.byte0 = _SBlock[w._byteStr.byte0 % 16 + N*(w._byteStr.byte0 / 16)];
	w._byteStr.byte1 = _SBlock[w._byteStr.byte1 % 16 + N*(w._byteStr.byte1 / 16)];
	w._byteStr.byte2 = _SBlock[w._byteStr.byte2 % 16 + N*(w._byteStr.byte2 / 16)];
	w._byteStr.byte3 = _SBlock[w._byteStr.byte3 % 16 + N*(w._byteStr.byte3 / 16)];
	return w;
}

U_4Bytes CEncoder::RotWord(U_4Bytes w)//метод циклического сдвига строк слова w
{
	ui8 k;
	k = w._byteStr.byte3;
	w._byteStr.byte3 = w._byteStr.byte2;
	w._byteStr.byte2 = w._byteStr.byte1;
	w._byteStr.byte1 = w._byteStr.byte0;
	w._byteStr.byte0 = k;
	return w;
}

void CEncoder::Receiver(ui32* pArray)//заполнение матрицы состояния массивом данных 
{
	for (int i = 0; i<_Nb; i++)
	{
		_stateMatrix[i]._ui32 = pArray[i];
	}
}

void CEncoder::ReceiverKey(ui32* pArray)//заполнение ключа шифрования массиовом данных
{
	for (int i = 0; i<_Nk; i++)
	{
		_key[i]._ui32 = pArray[i];
	}
	ExpansionKey();
}

void CEncoder::Encrypt()//метод зашифровать 
{
	AddRoundKey(0);
	for (int i = 1; i <= _Nr; i++)
	{
		while (i<_Nr)
		{
			SubBytes();
			ShiftRows();
			MixColumns();
			AddRoundKey(i);
			i++;
		}
		SubBytes();
		ShiftRows();
		AddRoundKey(i);
	}
}

void CEncoder::Decrypt()//метод дешифровать
{
	AddRoundKey(_Nr);
	for (int i = _Nr - 1; i >= 0; i--)
	{
		while (i>0)
		{
			InvShiftRows();
			InvSubBytes();
			AddRoundKey(i);
			InvMixColumns();
			i--;
		}
		InvShiftRows();
		InvSubBytes();
		AddRoundKey(i);
	}
}

CEncoder::~CEncoder()
{
}
