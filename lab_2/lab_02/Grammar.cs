
namespace lab_02
{
    class Grammar
    {
        public List<string> Terminals { get; private set;}
        public List<string> Nonterminals { get; private set;}
        public List<Rule> Rules { get; private set;}
        public string Start {get; private set;}

        /// <summary>
        /// 
        /// </summary>
        /// <param name="terminals"></param>
        /// <param name="nonterminals"></param>
        /// <param name="rules"></param>
        /// <param name="start"></param>
        /// <exception cref="InvalidGrammarException"></exception>
        public Grammar(List<string> terminals, List<string> nonterminals, List<Rule> rules, string start)
        {
            terminals.ForEach(x => 
            {
                if (nonterminals.Contains(x))
                    throw new InvalidGrammarException();
            });
            if (!nonterminals.Contains(start) || terminals.Contains(start))
                throw new InvalidGrammarException();
            rules.ForEach(x =>
            {
                if (!nonterminals.Contains(x.Left) || terminals.Contains(x.Left))
                    throw new InvalidGrammarException();
                if (x.Right.Count != 1 || x.Right[0].Length != 1 || x.Right[0][0] != 'e')
                {
                    foreach (var symbol in x.Right)
                    {
                        if (!nonterminals.Contains(symbol) && !terminals.Contains(symbol))
                            throw new InvalidGrammarException();
                    }
                }
            });
            Terminals = [..terminals];
            Nonterminals = [..nonterminals];
            Rules = [..rules];
            Start = start;
        }
        public Grammar() : this(["a", "b"], ["A", "B", "C"], [new Rule("A", ["a", "A"]),
                                                              new Rule("A", ["b", "A"]),
                                                              new Rule("A", ["a", "B"]),
                                                              new Rule("B", ["b", "C"]),
                                                              new Rule("C", ["b"])], "A") {}
        public Grammar(Grammar other) : this(other.Terminals, other.Nonterminals, other.Rules, other.Start) {}
        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        /// <exception cref="InvalidFilenameException"></exception>
        public static Grammar ReadGrammarFromFile(string filename)
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
            string[] sets = text.Split('{', '}');
            var terminals = ParseSets(sets[1]);
            var nonterminals = ParseSets(sets[3]);
            var rules = ParseRules(sets[5], terminals, nonterminals);
            var start = ParseStartSymbol(sets[6], terminals, nonterminals);
            return new Grammar(terminals, nonterminals, rules, start);
        }
        public static Grammar GenerateGrammarToFile(string filename)
        {
            // Terminals: {+, *, id, (, )}
            // Nonterminals: {E, T, F}
            // Rules: {E -> E+T | T,
            // T -> T*F | F,
            // F -> (E) | id}
            // Start symbol: E
            List<string> terminals = ["+", "*", "id", "(", ")"];
            List<string> nonterminals = ["E", "T", "F"];
            List<Rule> rules = [new Rule("E", ["E", "+", "T"]),
                                new Rule("E", ["T"]),
                                new Rule("T", ["T", "*", "F"]),
                                new Rule("T", ["F"]),
                                new Rule("F", ["(", "E", ")"]),
                                new Rule("F", ["id"])];
            string start = "E";
            Grammar newGrammar = new(terminals, nonterminals, rules, start);
            try
            {
                File.WriteAllText(filename, newGrammar.ToString());
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                throw new InvalidFilenameException();
            }
            
            return newGrammar;
        }
        public void EliminationOfLeftRecursion()
        {
            int countOfNonterminals = Nonterminals.Count;
            for (int i = 0; i < countOfNonterminals; ++i)
            {
                for (int j = 0; j < i; ++j)
                    ReplaceRules(Nonterminals[i], Nonterminals[j]);
                EliminationOfDirectLeftRecursion(Nonterminals[i]);
            }
        }
        public void LeftFactorization()
        {
            List<Rule> newRules = [];
            int nonTerminalCount = Nonterminals.Count;
            for (int i = 0; i < nonTerminalCount; ++i)
            {
                List<Rule> nonterminalRules = [];
                foreach (var rule in Rules)
                {
                    if (rule.Left == Nonterminals[i])
                        nonterminalRules.Add(rule);
                }
                newRules = [..newRules.Concat(NonterminalLeftFactorization(nonterminalRules))];
            }
            Rules = newRules;
        }
        public void DeleteUnreachableSymbols()
        {
            List<string> setNonterminals = [Start];
            List<string> setTerminals = [];
            List<Rule> newRules = [];
            for (int i = 0; i < setNonterminals.Count; ++i)
            {
                for (int j = 0; j < Rules.Count; ++j)
                {
                    if (Rules[j].Left == setNonterminals[i])
                    {
                        newRules.Add(Rules[j]);
                        foreach (var symbol in Rules[j].Right)
                            if (Nonterminals.Contains(symbol) && !setNonterminals.Contains(symbol))
                                setNonterminals.Add(symbol);
                            else if (Terminals.Contains(symbol) && !setTerminals.Contains(symbol))
                                setTerminals.Add(symbol);
                    }
                }
            }
            Rules = newRules;
            Nonterminals = setNonterminals;
            Terminals = setTerminals;
        }
        public override string ToString()
        {
            SortRules();
            string resultString = "Terminals: {";
            for (int i = 0; i < Terminals.Count - 1; ++i)
                resultString += $"{Terminals[i]}, ";
            resultString += $"{Terminals[^1]}";
            
            resultString += "}\nNonterminals: {";
            for (int i = 0; i < Nonterminals.Count - 1; ++i)
                resultString += $"{Nonterminals[i]}, ";
            resultString += $"{Nonterminals[^1]}";
            
            resultString += "}\nRules: {";
            for (int i = 0; i < Rules.Count; ++i)
            {
                resultString += $"{Rules[i].Left} -> ";
                foreach (var letter in Rules[i].Right)
                    resultString += letter;
                int j = i + 1;
                while (j != Rules.Count && Rules[j].Left == Rules[i].Left)
                {
                    resultString += $" | ";
                    foreach (var letter in Rules[j].Right)
                        resultString += letter;
                    ++j;
                }
                i = j - 1;
                if (i != Rules.Count - 1)
                    resultString += $",\n";
            }
            
            resultString += $"}}\nStart symbol: {Start}\n";
            return resultString;
        }
        private List<Rule> NonterminalLeftFactorization(List<Rule> rules)
        {
            if (rules.Count == 0)
                return [];
            string nonterminal = rules[0].Left;
            string newNonterminal = nonterminal + "'";
            while (Nonterminals.Contains(newNonterminal))
                newNonterminal += "'";
            List<string> prefix = FindMaxPrefix(rules);
            List<Rule> newNonterminalRules = [];
            while (prefix.Count != 0)
            {
                List<Rule> newRules = [];
                Nonterminals.Add(newNonterminal);
                newRules.Add(new Rule(nonterminal, [..prefix.Concat([newNonterminal])]));
                foreach (var rule in rules)
                {
                    if (rule.HasPrefix(prefix))
                    {
                        List<string> right = rule.Right[prefix.Count..];
                        newNonterminalRules.Add(new Rule(newNonterminal, right.Count == 0 ? ["e"] : right));
                    }
                    else
                        newRules.Add(new Rule(rule));
                }
                rules = newRules;
                prefix = FindMaxPrefix(rules);
                newNonterminal += "'";
            }
            return [..rules.Concat(newNonterminalRules)];
        }
        private static List<string> FindMaxPrefix(List<Rule> rules)
        {
            List<string> maxPrefix = [];
            for (int i = 0; i < rules.Count; ++i)
            {
                for (int j = i + 1; j < rules.Count; ++j)
                {
                    List<string> intersection = rules[i].GetIntersection(rules[j]);
                    if (intersection.Count > maxPrefix.Count)
                        maxPrefix = intersection;
                }
            }
            return maxPrefix;
        }
        private void ReplaceRules(string leftTerm, string rightTerm)
        {
            int oldRulesCount = Rules.Count;
            for (int i = 0; i < oldRulesCount; ++i)
            {
                if (Rules[i].Left == leftTerm && Rules[i].Right[0] == rightTerm)
                {
                    var rule = Rules[i];
                    Rules = [..Rules[..i].Concat(Rules[(i + 1)..])];
                    --oldRulesCount;
                    for (int j = 0; j < oldRulesCount; ++j)
                        if (Rules[j].Left == rightTerm)
                        {
                            if (Rules[j].Right[0] == "e")
                                Rules.Add(new Rule(rule.Left, rule.Right[1..]));
                            else
                                Rules.Add(new Rule(rule.Left, [..Rules[j].Right.Concat(rule.Right[1..])]));
                        }
                }
            }
        }
        private void EliminationOfDirectLeftRecursion(string term)
        {
            bool isRecursive = false;
            foreach (var rule in Rules)
                isRecursive = isRecursive || (rule.Left == term && rule.Right[0] == term);
            if (isRecursive)
            {
                string newTerm = term + "'";
                Nonterminals.Add(newTerm);
                List<Rule> newRules = [];
                foreach (var rule in Rules)
                {
                    if (rule.Left != term)
                        newRules.Add(rule);
                    else if (rule.Right[0] == term)
                        newRules.Add(new Rule(newTerm, [..rule.Right[1..].Concat([newTerm])]));
                    else if (rule.Right[0] == "e")
                        newRules.Add(new Rule(term, [newTerm]));
                    else
                        newRules.Add(new Rule(term, [..rule.Right.Concat([newTerm])]));
                }
                newRules.Add(new Rule(newTerm, ["e"]));
                Rules = newRules;
            }
        }
        private void SortRules()
        {
            List<string> terms = [Start];
            List<Rule> newRules = [];
            for (int index = 0; index < terms.Count && index < Rules.Count; ++index)
            {
                for (int j = 0; j < Rules.Count; ++j)
                {
                    if (Rules[j].Left == terms[index])
                    {
                        newRules.Add(Rules[j]);
                        foreach (var terminal in Rules[j].Right)
                            if (Nonterminals.Contains(terminal) && !terms.Contains(terminal))
                                terms.Add(terminal);
                    }
                }
            }
            foreach (var nonterminal in Nonterminals)
                if (!terms.Contains(nonterminal))
                    for (int j = 0; j < Rules.Count; ++j)
                        if (Rules[j].Left == nonterminal)
                            newRules.Add(Rules[j]);
            Rules = newRules;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        /// <exception cref="InvalidInputException"></exception>
        private static List<string> ParseSets(string str)
        {
            List<string> letters = new(str.Split(','));
            for (int i = 0; i < letters.Count; ++i)
            { 
                letters[i] = letters[i].Trim();
            }
            return letters;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        /// <exception cref="InvalidInputException"></exception>
        private static List<Rule> ParseRules(string str, List<string> terminals, List<string> nonterminals)
        {
            List<string> groups = [..str.Split(',')];
            List<Rule> rules = [];
            foreach (var group in groups)
            {
                string[] splitted = group.Split("->");
                string left = splitted[0].Trim();
                foreach (var rule in splitted[1].Trim().Split('|'))
                {
                    string letter = "";
                    List<string> right = [];
                    var trimmedRule = rule.Trim();
                    for (int i = 0; i < trimmedRule.Length; ++i)
                    {
                        letter += trimmedRule[i];
                        if (letter == "e" || terminals.Contains(letter) || nonterminals.Contains(letter))
                        {
                            right.Add(letter);
                            letter = "";
                        }
                    }
                    if (letter.Length != 0)
                        throw new InvalidInputException();
                    rules.Add(new Rule(left, right));
                }
            }
            return rules;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        /// <exception cref="InvalidInputException"></exception>
        private static string ParseStartSymbol(string str, List<string> terminals, List<string> nonterminals)
        {
            int startIndex = str.IndexOf("Start symbol: ");
            if (startIndex == -1)
                throw new InvalidInputException();
            str = str[(startIndex + 14)..];
            int i = 0;
            string letter = $"{str[i++]}";
            while (!terminals.Contains(letter) && !nonterminals.Contains(letter))
                letter += str[i++];
            return letter;
        }
    }
}