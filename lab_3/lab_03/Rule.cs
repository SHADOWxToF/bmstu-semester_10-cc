
using System.Net;

namespace lab_03
{
    class Rule(string left, List<string> right)
    {
        public string Left {get;} = left;
        public List<string> Right {get;} = [..right];
        
        public Rule(Rule rule) : this(rule.Left, rule.Right) {}
        public override string ToString()
        {
            string result = $"{Left} -> ";
            foreach (var letter in Right)
                result += $"{letter}";
            return result;
        }

        public List<string> GetIntersection(Rule second)
        {
            List<string> intersection = [];
            int i = 0;
            while (i < Right.Count && i < second.Right.Count && Right[i] == second.Right[i])
                intersection.Add(Right[i++]);
            return intersection;
        }
        public bool HasPrefix(List<string> prefix)
        {
            int i = 0;
            while (i != prefix.Count && i != Right.Count && Right[i] == prefix[i])
                ++i;
            return i == prefix.Count;
        }
    }
}