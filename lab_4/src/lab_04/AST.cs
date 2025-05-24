using System.Diagnostics;
using System.Diagnostics.Metrics;
using System.Net.Http.Headers;
using System.Numerics;

namespace lab_04
{
    class Node
    {
        public string Name { get; set; }
        public List<Node> Childs { get; }
        public Node(string name, List<Node> childs)
        {
            Name = name;
            Childs = [.. childs];
        }
        public Node() : this("", []) { }
        public Node(string name) : this(name, []) { }

    }

    class AST(Node tree)
    {
        public Node Tree { get; set; } = tree;
        public void Visualize(string filename)
        {
            string dotLanguage = "";
            dotLanguage += "digraph AST {\n";
            List<string> leaves = [];
            dotLanguage += NodeToString(Tree, "0", leaves);
            dotLanguage += "{rank=sink;";
            foreach (var leaf in leaves)
                dotLanguage += $" \"{leaf}\";";
            dotLanguage += "}\n}";
            try
            {
                File.WriteAllText($"{filename}.gv", dotLanguage);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                throw new InvalidFilenameException();
            }
            var proc = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "dot",
                    Arguments = $"-Tjpg {filename}.gv -o{filename}.jpg",
                    RedirectStandardOutput = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            proc.Start();
            proc.WaitForExit();
            proc = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "mspaint",
                    Arguments = $"{filename}.jpg",
                    RedirectStandardOutput = true,
                    UseShellExecute = false
                }
            };
            proc.Start();
        }
        public string ReversePolishNotation()
        {
            return ReversePolishNotationRecursive(Tree);
        }
        private string ReversePolishNotationRecursive(Node node)
        {
            string res = "";
            foreach (var child in node.Childs)
                res += $"{ReversePolishNotationRecursive(child)}";
            res += $" {node.Name} ";
            return res;
        }
        private static string NodeToString(Node node, string alias, List<string> leaves)
        {
            if (node.Childs.Count == 0)
                leaves.Add($"node_{alias}");
            string dotLanguage = $"\"node_{alias}\" [label=\"{node.Name}\"];\n";
            int counter = 0;
            foreach (var child in node.Childs)
            {
                string childAlias = alias + $"_{counter++}";
                dotLanguage += $"\"node_{alias}\" -> \"node_{childAlias}\";\n";
                dotLanguage += NodeToString(child, childAlias, leaves);
            }
            return dotLanguage;
        }
    }
}