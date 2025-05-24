
namespace lab_04
{
    class InvalidInputException(string message) : BaseLab02Exception($"Неверный ввод: {message}");
}