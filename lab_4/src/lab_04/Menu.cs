
namespace lab_04
{
    class Menu()
    {
        private static Grammar? grammar;
        public static void Run()
        {
            Console.WriteLine("\nВас приветствует лабораторная работа №4!!!");
            int choice = -1;
            grammar = ReadGrammar();
            while (choice != 0)
            {
                Console.WriteLine("\n\n\nКакое действие Вы хотите совешить?");
                // Console.WriteLine("1 - ввести грамматику");
                Console.WriteLine("1 - вывести грамматику");
                Console.WriteLine("2 - проверить, что грамматика LL(1) типа");
                Console.WriteLine("3 - запустить нисходящий синтаксический анализ методом рекурсивного спуска\n");
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
                            Console.WriteLine(grammar ?? throw new GrammarNotInitializedException());
                            choice = -1;
                            break;
                        case 2:
                            if (grammar == null)
                                throw new GrammarNotInitializedException();
                            Console.WriteLine($"Грамматика{(grammar.IsGrammarLL1() ? "" : " не")} является LL(1)");
                            choice = -1;
                            break;
                        case 3:
                            Console.WriteLine(grammar ?? throw new GrammarNotInitializedException());
                            // foreach (var nonterminal in grammar.Nonterminals)
                            // {
                            //     Console.Write($"FIRST({nonterminal}) = {{{{ ");
                            //     List<string> f = [..grammar.FIRST(nonterminal)];
                            //     for (int i = 0; i < f.Count; ++i)
                            //         if (i + 1 < f.Count)
                            //             Console.Write(f[i] + ", ");
                            //         else
                            //             Console.Write(f[i]);
                            //     Console.Write(" }}");
                            //     Console.Write($"         FOLLOW({nonterminal}) = {{{{ ");
                            //     f = [..grammar.FOLLOW(nonterminal)];
                            //     for (int i = 0; i < f.Count; ++i)
                            //         if (i + 1 < f.Count)
                            //             Console.Write(f[i] + ", ");
                            //         else
                            //             Console.Write(f[i]);
                            //     Console.WriteLine(" }}");
                            // }
                            string filename = ReadString("Введите имя файла для компиляции: ");
                            try
                            {
                                grammar.Compile(filename, out List<Rule> generation, out AST ast);
                                Console.WriteLine(grammar.SimulateGeneration(generation));
                                ast.Visualize("MainAST");
                                List<AST> expressions = grammar.GetExpressionTrees(ast);
                                for (int i = 0; i < expressions.Count; ++i)
                                {
                                    expressions[i].Visualize($"exp{i}");
                                    Console.WriteLine(expressions[i].ReversePolishNotation());
                                }
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine(e.Message);
                            }
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
            // string filename = ReadString("Введите имя файла с грамматикой: ");
            string filename = "variant1";
            return Grammar.ReadGrammarFromFile(filename);
        }
        private static Grammar GenerateGrammar()
        {
            string filename = ReadString("Введите имя файла, в который сгенерировать грамматику: ");
            return Grammar.GenerateGrammarToFile(filename);
        }
        public static string ReadString(string text)
        {
            Console.Write(text);
            return Console.ReadLine() ?? throw new InvalidFilenameException();
        }
    }
}