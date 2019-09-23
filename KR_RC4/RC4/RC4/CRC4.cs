namespace RC4
{
    static class SwapElem
    {
        // Функция, которая меняет местами i1-й и i2-й элементы массива array
        public static void Swap<T>(this T[] array, int i1, int i2)
        {
            T temp = array[i1];
            array[i1] = array[i2];
            array[i2] = temp;
        }
    }
    public class CRC4
    {
        private byte[] S; //S-блок - массив
        private int length; //Размер S-блока - размер массива
        private int ii = 0; //параметр для метода PRGA
        private int jj = 0; //параметр для метода PRGA

        // Конструктор с параметром для создания массива из 2^n элеиентов
        public CRC4(int n)
        {
            length = 1 << n;
            S = new byte[length];
        }

        // Метод, который реализует заполнение S-блока начальными значениями
        private void FillS()
        {
            for (int i = 0; i < length; i++)
            {
                S[i] = (byte)i;
            }
        }

        // Метод, который реализует алгоритм KSA
        private void KSA(byte[] key)
        {
            FillS();
            int keyLength = key.Length;
            int j = 0;
            for (int i = 0; i < length; i++)
            {
                j = (j + S[i] + key[i%keyLength]) % length;
                S.Swap(i, j);
            }
        }

        // Метод, который реализует алгоритм PRGA
        private byte PRGA()
        {
            ii = (ii + 1) % length;
            jj = (jj + S[ii]) % length;
            S.Swap(ii, jj);
            int t = (S[ii] + S[jj]) % length;
            return S[t];
        }

        // Метод зашифровать
        public byte[] Encode(byte[] openT, byte[] key)
        {
            KSA(key);
            byte[] closeT = new byte[openT.Length];
            for (int m = 0; m < openT.Length; m++)
            {
                closeT[m] = (byte)(openT[m] ^ PRGA());
            }
            return closeT;
        }

        // Метод дешифровать
        public byte[] Decode(byte[] closeT, byte[] key)
        {
            KSA(key);
            byte[] openT = new byte[closeT.Length];
            for (int m = 0; m < openT.Length; m++)
            {
                openT[m] = (byte)(closeT[m] ^ PRGA());
            }
            return openT;
        }
    }
}
