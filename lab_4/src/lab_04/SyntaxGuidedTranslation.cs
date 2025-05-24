namespace lab_04
{
    partial class Grammar
    {
        public List<AST> GetExpressionTrees(AST majorAST)
        {
            try
            {
                Node operatorList = majorAST.Tree.Childs[0].Childs[1];
                List<AST> astList = [GetExpressionTree(operatorList.Childs[0])];
                operatorList = operatorList.Childs[1].Childs[0];
                while (operatorList.Name != "e")
                {
                    astList.Add(GetExpressionTree(operatorList.Childs[1]));
                    operatorList = operatorList.Childs[2].Childs[0];
                }
                return astList;
            }
            catch (Exception)
            {
                return [];
            }
        }
        private AST GetExpressionTree(Node head)
        {
            Node expression = new(head.Childs[1].Name);
            Node left = head.Childs[0];
            while (left.Childs.Count != 0)
                left = left.Childs[0];
            expression.Childs.Add(new Node(left.Name));

            Node right = head.Childs[2];

            Node sign = right.Childs[1];
            while (sign.Childs.Count != 0)
                sign = sign.Childs[0];

            if (sign.Name != "e")
            {
                expression.Childs.Add(new Node(sign.Name));
                expression.Childs[1].Childs.Add(ParseSimpleExpression(right.Childs[0]));
                expression.Childs[1].Childs.Add(ParseSimpleExpression(right.Childs[1].Childs[1]));
            }
            else
                expression.Childs.Add(ParseSimpleExpression(right.Childs[0]));
            return new AST(expression);
        }
        private Node ParseSimpleExpression(Node head)
        {
            if (head.Name != "<простое выражение>")
                throw new TreeConfigurationException($"ожидался нетерминал <простое выражение>, а получен {head.Name}");

            Node simpleExpression;
            if (head.Childs.Count == 3)
            {
                simpleExpression = new Node(head.Childs[0].Childs[0].Name);
                simpleExpression.Childs.Add(ParseTerm(head.Childs[1]));
            }
            else
            {
                simpleExpression = ParseTerm(head.Childs[0]);
            }
            Node? next = ParseSimpleExpressionStreak(head.Childs.Last());
            if (next != null)
            {
                next.Childs.Add(simpleExpression);
                next.Childs.Reverse();
                simpleExpression = next;
            }
            return simpleExpression;
        }
        private Node? ParseSimpleExpressionStreak(Node head)
        {
            if (head.Name != "<простое выражение>'")
                throw new TreeConfigurationException($"ожидался нетерминал <простое выражение>', а получен {head.Name}");

            if (head.Childs.Count == 1)
                return null;

            Node simpleExpresssionStreak = new(head.Childs[0].Childs[0].Name);

            Node? next = ParseSimpleExpressionStreak(head.Childs[2]);
            if (next == null)
                simpleExpresssionStreak.Childs.Add(ParseTerm(head.Childs[1]));
            else
            {
                next.Childs.Add(ParseTerm(head.Childs[1]));
                next.Childs.Reverse();
                simpleExpresssionStreak.Childs.Add(next);
            }
            return simpleExpresssionStreak;

        }
        private Node ParseTerm(Node head)
        {
            if (head.Name != "<терм>")
                throw new TreeConfigurationException($"ожидался нетерминал <терм>, а получен {head.Name}");

            Node term = ParseFactor(head.Childs[0]);

            Node? next = ParseTermStreak(head.Childs[1]);
            if (next != null)
            {
                next.Childs.Add(term);
                next.Childs.Reverse();
                term = next;
            }
            return term;
        }
        private Node? ParseTermStreak(Node head)
        {
            if (head.Name != "<терм>'")
                throw new TreeConfigurationException($"ожидался нетерминал <терм>', а получен {head.Name}");
            
            if (head.Childs.Count == 1)
                return null;

            Node termStreak = new(head.Childs[0].Childs[0].Name);

            Node? next = ParseTermStreak(head.Childs[2]);
            if (next == null)
                termStreak.Childs.Add(ParseFactor(head.Childs[1]));
            else
            {
                next.Childs.Add(ParseFactor(head.Childs[1]));
                next.Childs.Reverse();
                termStreak.Childs.Add(next);
            }
            return termStreak;
        }
        private Node ParseFactor(Node head)
        {
            if (head.Name != "<фактор>")
                throw new TreeConfigurationException($"ожидался нетерминал <фактор>, а получен {head.Name}");

            if (head.Childs[0].Name == "<идентификатор>" || head.Childs[0].Name == "<константа>")
                return new Node(head.Childs[0].Childs[0].Name);
            else if (head.Childs[0].Name == "not")
                return new Node("not", [ParseFactor(head.Childs[1])]);
            else
                return ParseSimpleExpression(head.Childs[1]);
        }
    }
}