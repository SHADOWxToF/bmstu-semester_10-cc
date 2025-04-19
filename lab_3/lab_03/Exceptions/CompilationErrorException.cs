
namespace lab_03
{
    class CompilationErrorException(string message) : BaseLab02Exception($"Ошибка компиляции: {message}");
}