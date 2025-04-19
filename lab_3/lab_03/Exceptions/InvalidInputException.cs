
namespace lab_03
{
    class InvalidInputException(string message) : BaseLab02Exception($"Неверный ввод: {message}");
}