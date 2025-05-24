
namespace lab_04
{
    class CompilationErrorException(string message) : BaseLab02Exception($"Ошибка компиляции: {message}");
}