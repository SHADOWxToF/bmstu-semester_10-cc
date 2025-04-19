

using System.CodeDom.Compiler;

namespace lab_03
{
    partial class Grammar
    {
        private Dictionary<string, HashSet<string>> firstDict = [];
        private Dictionary<string, HashSet<string>> followDict = [];
        private Dictionary<string, Dictionary<string, List<string>>> syntaxTable = [];
        public HashSet<string> FIRST(List<string> symbols)
        {
            HashSet<string> first = [];
            int i = 0;
            bool epsilonInLastFirst = true;
            while (i < symbols.Count && epsilonInLastFirst)
            {
                epsilonInLastFirst = false;
                HashSet<string> innerFirst = FIRST(symbols[i]);
                foreach (var inner in innerFirst)
                    if (inner == "e")
                        epsilonInLastFirst = true;
                    else
                        first.Add(inner);
                ++i;
            }
            if (epsilonInLastFirst)
                first.Add("e");
            return first;
        }
        public HashSet<string> FIRST(string symbol)
        {
            if (symbol == "")
                symbol = "e";
            if (firstDict.ContainsKey(symbol)) {}
            else if (Terminals.Contains(symbol) || symbol == "e")
                firstDict[symbol] = [symbol];
            else
            {
                HashSet<string> first = [];
                foreach (var rule in Rules)
                {
                    if (rule.Left == symbol)
                        first = [..first.Concat(FIRST(rule.Right))];
                }
                firstDict[symbol] = [..first];
            }
            return firstDict[symbol];
        }
        public HashSet<string> FOLLOW(string symbol)
        {
            if (!followDict.ContainsKey(symbol))
            {
                followDict[symbol] = [];
                if (symbol == Start)
                    followDict[symbol] = ["$"];
                foreach (var rule in Rules)
                {
                    for (int i = 0; i < rule.Right.Count; ++i)
                    {
                        if (rule.Right[i] == symbol)
                        {
                            var innerFirst = FIRST(rule.Right[(i + 1)..]);
                            bool isEpsilonInFirst = false;
                            foreach (var inner in innerFirst)
                                if (inner == "e")
                                    isEpsilonInFirst = true;
                                else
                                    followDict[symbol].Add(inner);
                            if (isEpsilonInFirst)
                                followDict[symbol] = [..followDict[symbol].Concat(FOLLOW(rule.Left))];
                        }
                    }
                }
            }
            return followDict[symbol];
        }

        public bool IsGrammarLL1()
        {
            foreach (var nonterminal in Nonterminals)
            {
                List<Rule> rules = [];
                foreach (var rule in Rules)
                    if (rule.Left == nonterminal)
                        rules.Add(rule);
                for (int i = 0; i < rules.Count; ++i)
                    for (int j = i + 1; j < rules.Count; ++j)
                    {
                        HashSet<string> firstI = [..FIRST(rules[i].Right)];
                        HashSet<string> firstJ = [..FIRST(rules[j].Right)];
                        HashSet<string> intersection = [..firstI.Intersect(firstJ)];
                        if (intersection.Count != 0)
                            return false;
                        HashSet<string> followNonterminal = [..FOLLOW(nonterminal)];
                        if (firstI.Contains("e"))
                        {
                            intersection = [..firstJ.Intersect(followNonterminal)];
                            if (intersection.Count != 0)
                                return false;
                        }
                        if (firstJ.Contains("e"))
                        {
                            intersection = [..firstI.Intersect(followNonterminal)];
                            if (intersection.Count != 0)
                                return false;
                        }
                    }
            }
            return true;
        }
        public void GenerateSyntaxTable()
        {
            foreach (var nonterminal in Nonterminals)
            {
                syntaxTable[nonterminal] = [];
                foreach (var terminal in Terminals)
                    syntaxTable[nonterminal][terminal] = [];
                syntaxTable[nonterminal]["$"] = [];
            }
            foreach (var rule in Rules)
            {
                HashSet<string> first = FIRST(rule.Right);
                foreach (var symbol in first)
                {
                    if (symbol == "e")
                    {
                        HashSet<string> follow = FOLLOW(rule.Left);
                        foreach (var followSymbol in follow)
                            syntaxTable[rule.Left][followSymbol] = [..rule.Right];
                    }
                    else
                        syntaxTable[rule.Left][symbol] = [..rule.Right];
                    
                }
            }
            // Console.WriteLine(SyntaxTableToString());
        }
        private string SyntaxTableToString()
        {
            string res = "          ";
            foreach (var jkey in syntaxTable[Nonterminals[0]].Keys)
                res += $"{jkey, -10}";
            res += '\n';
            foreach (var key in syntaxTable.Keys)
            {
                res += $"{key, -10}";
                foreach (var jkey in syntaxTable[key].Keys)
                {
                    string s = "";
                    foreach (var symbol in syntaxTable[key][jkey])
                        s += symbol;
                    res += $"{s, -10}";
                }
                res += '\n';
            }
            return res;
        }
        public bool Compile(string filename, out List<Rule> leftGeneration)
        {
            GenerateSyntaxTable();
            return Analyse(ParseFile(filename), out leftGeneration);
        }
        public List<string> ParseFile(string filename)
        {
            string text;
            try
            {
                text = File.ReadAllText(filename);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                throw new InvalidFilenameException();
            }
            List<string> symbols = [];
            foreach (var group in text.Split(' ', '\n', '\r'))
            {
                string symbol = "";
                int i = 0;
                while (i < group.Length)
                {
                    symbol += group[i];
                    if (Terminals.Contains(symbol))
                    {
                        while (PrefixCount(symbol, Terminals) > 1 && ++i < group.Length)
                            symbol += group[i];
                        if (PrefixCount(symbol, Terminals) == 0)
                        {
                            symbol = symbol[..^1];
                            --i;
                        }
                        while (!Terminals.Contains(symbol) && ++i < group.Length)
                            symbol += group[i];
                        if (!Terminals.Contains(symbol))
                            throw new InvalidSyntaxException($"{group}");
                        symbols.Add(symbol);
                        symbol = "";
                    }
                    ++i;
                }
                if (symbol.Length != 0)
                    throw new InvalidSyntaxException($"{group}");
            }
            symbols.Add("$");
            return symbols;
        }
        public bool Analyse(List<string> inputStream, out List<Rule> leftGeneration)
        {
            leftGeneration = [];
            Stack<string> stack = [];
            stack.Push("$");
            stack.Push(Start);
            int inputStreamPointer = 0;
            string X = stack.Peek();
            while (X != "$")
            {
                string a = inputStream[inputStreamPointer];
                if (X == a)
                {
                    stack.Pop();
                    ++inputStreamPointer;
                }
                else if (Terminals.Contains(X))
                    throw new CompilationErrorException($"ожидался символ {X}, но получен {a} (символ №{inputStreamPointer + 1})");
                else if (syntaxTable[X][a].Count == 0)
                    throw new CompilationErrorException($"неожиданный символ {a} (символ №{inputStreamPointer + 1})");
                else
                {
                    leftGeneration.Add(new Rule(X, syntaxTable[X][a]));
                    stack.Pop();
                    if (syntaxTable[X][a][0] != "e")
                    {
                        for (int i = syntaxTable[X][a].Count - 1; i >= 0; --i)
                            stack.Push(syntaxTable[X][a][i]);
                    }
                }
                X = stack.Peek();
            }
            return true;
        }
        public string SimulateGeneration(List<Rule> leftGeneration)
        {
            string res = "";
            string beforeWorkSpace = "";
            Stack<string> afterWorkSpace = [];
            afterWorkSpace.Push(Start);
            res += beforeWorkSpace + SententialFormToString(afterWorkSpace);
            foreach (var rule in leftGeneration)
            {
                while (Terminals.Contains(afterWorkSpace.Peek()))
                    beforeWorkSpace += ' ' + afterWorkSpace.Pop();
                if (rule.Left != afterWorkSpace.Peek())
                {
                    res += " -> Error";
                    break;
                }
                afterWorkSpace.Pop();
                int i = 0;
                while (i < rule.Right.Count && Terminals.Contains(rule.Right[i]))
                    beforeWorkSpace += ' ' + rule.Right[i++];
                for (int j = rule.Right.Count - 1; j >= i; --j)
                    if (rule.Right[j] != "e")
                        afterWorkSpace.Push(rule.Right[j]);
                res += " -> " + beforeWorkSpace + SententialFormToString(afterWorkSpace) + '\n';
            }
            return res;
        }
        private string SententialFormToString(Stack<string> sententialForm)
        {
            string res = "";
            foreach (var symbol in sententialForm)
                res += ' ' + symbol;
            return res;
        }
    }
}