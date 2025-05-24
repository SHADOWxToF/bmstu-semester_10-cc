
namespace lab_04
{
    class TreeConfigurationException(string message) : BaseLab02Exception($"Неверная конфигурация дерева: {message}");
}