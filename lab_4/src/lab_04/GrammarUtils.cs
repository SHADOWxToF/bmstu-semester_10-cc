namespace lab_04
{
    partial class Grammar
    {
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
    }
}