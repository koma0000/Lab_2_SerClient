using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace ConsoleClient
{
    
    class Program
    {
        private static int id_client;

        static void SendMessage(Socket s, int To, int From, MessageTypes Type = MessageTypes.MT_DATA, string Data = "")
        {
            Msg m = new Msg(To, From, Type, Data);
            m.Send(s);
        }

        static void connect(Socket s, IPEndPoint endPoint)//Представляет сетевую конечную точку в виде IP-адреса и номер порта.
        {
            s.Connect(endPoint);
        }

        static void disconnect(Socket s)
        {
            s.Shutdown(SocketShutdown.Both); //Блокирует передачу и получение данных для объекта Socket.
            s.Close();
        }
        //Поддерживает надежные двусторонние байтовые потоки в режиме с установлением подключения, без дублирования данных и без сохранения границ данных.
        //Объект Socket этого типа взаимодействует с одним узлом и требует установления подключения к удаленному узлу перед началом передачи данных.
        //Stream использует протокол TCP (ProtocolType.Tcp) и семейство адресов AddressFamily.InterNetwork .
        public static void GetData()
        {
            int nPort = 12345;
            while (true)
            {
                IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);// получаем адреса для запуска сокета
                Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);//создаем сокет
                Msg m = new Msg();

                connect(s, endPoint);// связываем сокет с локальной точкой, по которой будем принимать данные
                if (!s.Connected)
                {
                 throw new Exception("Connection error");
                }
                SendMessage(s, 0, id_client, MessageTypes.MT_GETDATA);
                if (m.Receive(s) == MessageTypes.MT_DATA)
                {
                    Console.WriteLine("Message from client " + m.getMsgHeader().msgFrom + ": " + m.getMsgData());
                }
                disconnect(s);
                Thread.Sleep(1000);
            }
        }


        public static void Main(string[] args)
        {
            // Спим секунду
            Thread.Sleep(1000);
            int nPort = 12345;
            Msg m = new Msg();
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            //Подключение к серверу
            connect(s, endPoint);
            if (!s.Connected)
            {
                throw new Exception("Connection error");
            }
            SendMessage(s, (int)Address.MIDDLEWARE, 0, MessageTypes.MT_INIT);
            if (m.Receive(s) == MessageTypes.MT_CONFIRM)
            {
                id_client = m.getMsgHeader().msgTo;
                Console.WriteLine("Client is starting\n");
                Console.WriteLine("ID " + id_client);
                Thread t = new Thread(GetData);
                t.Start();
            }
            disconnect(s);

            //Отправке сообщений
            while (true)
            {
                endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
                s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Console.WriteLine("1. Send Message\n2. Exit\n");
                int ch = Convert.ToInt32(Console.ReadLine());
                switch (ch)
                {
                    case 1:
                        {
                            int ClientID = (int)Address.BROADCAST;
                            Console.WriteLine("1. Only for one client\n2.For All Clients\n");
                            int ch2 = Convert.ToInt32(Console.ReadLine());
                            if (ch2 == 1)
                            {
                                Console.WriteLine("\nEnter ID of Client\n");
                                ClientID = Convert.ToInt32(Console.ReadLine());
                            }

                            Console.WriteLine("\nEnter your message\n");
                            string st = Console.ReadLine().ToString();
                            connect(s, endPoint);
                            SendMessage(s, ClientID, id_client, MessageTypes.MT_DATA, st);
                            if (m.Receive(s) == MessageTypes.MT_CONFIRM)
                            {
                                Console.WriteLine("\nsuccess\n");
                            }
                            else
                            {
                                Console.WriteLine("\nerror\n");
                            }
                            disconnect(s);
                            break;

                        }
                    case 2:
                        {
                            connect(s, endPoint);
                            SendMessage(s, (int)Address.MIDDLEWARE, id_client, MessageTypes.MT_EXIT);
                            if (m.Receive(s) == MessageTypes.MT_CONFIRM)
                            {
                                Console.WriteLine("\nsuccess\n");
                            }
                            else
                            {
                                Console.WriteLine("\nerror\n");
                            }

                            disconnect(s);
                            Environment.Exit(0);
                            break;
                        }

                    default:
                        break;
                }
            }
        }
    }
}
