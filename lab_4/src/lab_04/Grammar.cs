
namespace lab_04
{
    partial class Grammar
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
            string[] sets = text.Split(["{{", "}}"], StringSplitOptions.None);
            var terminals = ParseSets(sets[1]);
            var nonterminals = ParseSets(sets[3]);
            var rules = ParseRules(sets[5], [..terminals.Concat(nonterminals)]);
            var start = ParseStartSymbol(sets[6], nonterminals);
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
        public override string ToString()
        {
            SortRules();
            string resultString = "Terminals: {{";
            for (int i = 0; i < Terminals.Count - 1; ++i)
                resultString += $"{Terminals[i]}, ";
            resultString += $"{Terminals[^1]}";
            
            resultString += "}}\nNonterminals: {{";
            for (int i = 0; i < Nonterminals.Count - 1; ++i)
                resultString += $"{Nonterminals[i]}, ";
            resultString += $"{Nonterminals[^1]}";
            
            resultString += "}}\nRules: {{";
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
            
            resultString += $"}}}}\nStart symbol: {Start}\n";
            return resultString;
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
        private static List<Rule> ParseRules(string str, List<string> symbols)
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
                    int i = 0;
                    while (i < trimmedRule.Length)
                    {
                        if (letter.Length == 0 && trimmedRule[i] == '"')
                        {
                            while (i + 1 < trimmedRule.Length && trimmedRule[++i] != '"')
                                letter += trimmedRule[i];
                            if (trimmedRule[i] != '"')
                                throw new InvalidInputException($"{left}->{rule}");
                            if (letter != "e" && !symbols.Contains(letter))
                                throw new InvalidInputException($"invalid term in quotes {left}->{rule}");
                            right.Add(letter);
                            letter = "";
                        }
                        else
                        {
                            letter += trimmedRule[i];
                            if (letter == "e")
                            {
                                right.Add(letter);
                                letter = "";
                            }
                            else if (symbols.Contains(letter))
                            {
                                while (PrefixCount(letter, symbols) > 1 && ++i < trimmedRule.Length)
                                    letter += trimmedRule[i];
                                if (PrefixCount(letter, symbols) == 0)
                                {
                                    letter = letter[..^1];
                                    --i;
                                }
                                while (!symbols.Contains(letter) && ++i < trimmedRule.Length)
                                    letter += trimmedRule[i];
                                if (!symbols.Contains(letter))
                                    throw new InvalidInputException($"{left}->{rule}");
                                right.Add(letter);
                                letter = "";
                            }
                        }
                        ++i;
                    }
                    if (letter.Length != 0)
                        throw new InvalidInputException($"{left}->{rule}");
                    rules.Add(new Rule(left, right));
                }
            }
            return rules;
        }
        private static int PrefixCount(string prefix, List<string> symbols)
        {
            int counter = 0;
            foreach (var symbol in symbols)
                counter += symbol.StartsWith(prefix) ? 1 : 0;
            return counter;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        /// <exception cref="InvalidInputException"></exception>
        private static string ParseStartSymbol(string str, List<string> nonterminals)
        {
            int startIndex = str.IndexOf("Start symbol: ");
            if (startIndex == -1)
                throw new InvalidInputException("no start symbol");
            str = str[(startIndex + 14)..];
            int i = 0;
            string letter = $"{str[i]}";
            while (PrefixCount(letter, nonterminals) > 1 && ++i < str.Length)
                letter += str[i];
            if (PrefixCount(letter, nonterminals) == 0)
            {
                letter = letter[..^1];
                --i;
            }
            while (!nonterminals.Contains(letter) && ++i < str.Length)
                letter += str[i];
            if (!nonterminals.Contains(letter))
                throw new InvalidInputException("invalid start symbol");
            return letter;
        }
    }
}