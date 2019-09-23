using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RC4;

namespace TestRC4
{
    class Program
    {
        static void Main(string[] args)
        {

            Console.WriteLine("Введить ключ для шифрования");
            string mykey1 = Console.ReadLine();

            Console.WriteLine("Введить сообщение, которое хотите зашифровать");
            string myOpenText = Console.ReadLine();

            byte[] key1 = ASCIIEncoding.ASCII.GetBytes(mykey1);
            byte[] openText = ASCIIEncoding.ASCII.GetBytes(myOpenText);

            CRC4 encodeRC4 = new CRC4(8);

            byte[] closeText = encodeRC4.Encode(openText,key1);
            string myCloseText = ASCIIEncoding.ASCII.GetString(closeText);

            Console.WriteLine("Сообщение зашифровано");
            Console.WriteLine(myCloseText);

            CRC4 decodeRC4 = new CRC4(8);

            Console.WriteLine("Введите ключ для расшифровки");
            string mykey2 = Console.ReadLine();

            byte[] key2 = ASCIIEncoding.ASCII.GetBytes(mykey2);

            byte[] newOpenText = decodeRC4.Decode(closeText,key2);

            string myNewOpenText = ASCIIEncoding.ASCII.GetString(newOpenText);

            Console.WriteLine("Сообщение расшифровано");
            Console.WriteLine(myNewOpenText);

         
            Console.ReadLine();

        }
    }
}
