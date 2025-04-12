

using System.Linq.Expressions;

namespace lab_02
{
    class Menu()
    {
        private static Grammar? grammar;
        public static void Run()
        {
            Console.WriteLine("\nВас приветствует лабораторная работа №2!!!");
            int choice = -1;
            while (choice != 0)
            {
                Console.WriteLine("\n\n\nКакое действие Вы хотите совешить?");
                Console.WriteLine("1 - ввести грамматику");
                Console.WriteLine("2 - сгенерировать файл конфигурации и заполнить грамматику на его основе");
                Console.WriteLine("3 - вывести грамматику");
                Console.WriteLine("4 - устранить левую рекурсию");
                Console.WriteLine("5 - применить левую факторизацию");
                Console.WriteLine("6 - избавиться от недостижимых символов");
                Console.WriteLine("7 - сразу все действия\n");
                Console.WriteLine("0 - выход\n");
                Console.Write("Ваш выбор: ");
                try
                {
                    choice = Convert.ToInt32(Console.ReadLine());
                }
                catch (Exception)
                {
                    choice = -1;
                }
                try
                {
                    switch (choice)
                    {
                    case 1:
                        grammar = ReadGrammar();
                        Console.WriteLine(grammar);
                        choice = -1;
                        break;
                    case 2:
                        grammar = GenerateGrammar();                        
                        Console.WriteLine(grammar);
                        choice = -1;
                        break;
                    case 3:
                        Console.WriteLine(grammar ?? throw new GrammarNotInitializedException());
                        choice = -1;
                        break;
                    case 4:
                        grammar?.EliminationOfLeftRecursion();
                        Console.WriteLine(grammar ?? throw new GrammarNotInitializedException());
                        choice = -1;
                        break;
                    case 5:
                        grammar?.LeftFactorization();
                        Console.WriteLine(grammar ?? throw new GrammarNotInitializedException());
                        choice = -1;
                        break;
                    case 6:
                        grammar?.DeleteUnreachableSymbols();
                        Console.WriteLine(grammar ?? throw new GrammarNotInitializedException());
                        choice = -1;
                        break;
                    case 7:
                        grammar?.EliminationOfLeftRecursion();
                        grammar?.LeftFactorization();
                        grammar?.DeleteUnreachableSymbols();
                        Console.WriteLine(grammar ?? throw new GrammarNotInitializedException());
                        choice = -1;
                        break;
                    case 0:
                        break;
                    default:
                        Console.WriteLine("\nНеверный ввод, повторите попытку!!!\n");
                        break;
                    }
                }
                catch (BaseLab02Exception ex)
                {
                    // case INCORRECT_FILENAME:
                    //     printf("Не удалось прочесть файл\n");
                    // case STRUCT_DIDNT_INITIALIZED:
                    //     printf("Грамматика не инициализирована\n");
                    // case INVALID_INPUT:
                    //     printf("Содержимое файла не является грамматикой\n");
                    // default:
                    //     printf("\nres = %d\n", result);
                    //     printf("Ой-ой, что-то пошло не так\n");
                    Console.WriteLine(ex.Message);
                }
                if (choice != 0)
                {
                    Console.Write("Нажмите enter для продолжения...");
                    Console.ReadLine();
                }
            }
            Console.WriteLine("Спасибо, что воспользовались данной программой!!!");
        }

        private static Grammar ReadGrammar()
        {
            Console.Write("Введите имя файла с грамматикой: ");
            string filename = Console.ReadLine() ?? throw new InvalidFilenameException();
            return Grammar.ReadGrammarFromFile(filename);
        }
        private static Grammar GenerateGrammar()
        {
            Console.Write("Введите имя файла в который сгенерировать грамматику: ");
            string filename = Console.ReadLine() ?? throw new InvalidFilenameException();
            return Grammar.GenerateGrammarToFile(filename);
        }
    }
}