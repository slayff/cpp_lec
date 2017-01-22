#include <iostream>
#include <vector>
//Тема: обработка исключений
//Деструкторы для объектов вызываются при throwing exceptions (можно посмотреть на примере класса)
class C {
 public:
     C() {
        std::cout << "C()\n";
     }
}
void bar(int x) {
    //...
    throw 42;
}

void bar() {
    try {
        bar(1);
    } catch (int x) {
    
    } catch (...) {
        //Так ловятся все остальные типы исключений.
    }
}
//Вектор генерирует std::out_of_range exception
try {
    cout << vector_name.at(10);
} catch (out_of_range& s) {
    cout << "Exception!!!";
}
//Лучше генерировать исключения в конструкторе. В деструкторе подобные вещи делать опасно
//Можно перебросить исключение с помощью 
throw;

//Создаем свой класс exception:
class IncorrectDate: public std::exception {
//...
};

class Date {
 public:
     Date(int d, int m, int y) {
        if (m <= 0 || m > 12) {
            throw IncorrectDate();
        }
    }
};

int main() {
    try {
        int d, m, y;
        std::cin >> d >> m >> y;
        Date date(d, m, y);
    } catch (IncorrectDate& e) {
        std::cout << "Exception caught\n" << e.what();
    }
}

//Проверка на этапе компиляции:
//(Compile time assert)
int main() {
    static_assert(2 * 2 == 5, "Something strange!\n");
}
//Run-time assert
int main() {
    int x, y;
    std::cin >> x >> y;
    assert(x == y);

}

