//Лекция 4 - "Умные указатели (обертки над указателями)"

//Как и в прошлой лекции, допустим, что есть класс A, B и в C используются
//указатели на объекты этих классов
class A {
    //...
};

class B {
    //...
};

//Обертка над указателем, шаблонная
template <typename T>
class T_wrapper {
 private:
     T * ptr;

 public:
     T_wrapper(): ptr(new T) {}

     T_wrapper(const T_wrapper& other): ptr(new T(*other.ptr)) {}

     T_wrapper& operator = (const T_wrapper& other) {
        *ptr = *other.ptr;
        return *this;
     }
    //noexcept показывает, что в этом блоке кода никогда не будет exception
     ~T_wrapper() noexcept{
        delete ptr;
     }

     const T * get() const {
        return ptr;
     }

     T* get() {
        return ptr;
     }
};

class C {
 private:
     T_wrapper<A> a;
     T_wrapper<B> b;
 public:
     //В принципе, теперь и не обязательно самому писать конструктор, подойдет стандартный.
     C() {
     }
     //(Впрочем, как и деструктор)
};

//Пример класса матрицы на основе низкоуровневых двумерных массивов.
template<typename T>
class Matrix {
 private:
     size_t m, n;
     T ** data;

 public:
     //Очевидный конструктор, где может вылезти куча exceptions и произойти утечка данных.
     /*
     Matrix(size_t rows, size_t cols):
        m(rows),
        n(cols) {
            data = new T* [rows];
            for (size_t i = 0; i != rows; ++i) {
                data[i] = new T[cols];
                for (size_t j = 0; j != cols; ++j)
                    data[i][j] = 0;
            }
        }
    */

     //Чуть исправленный конструктор, но все же так делать не стоит.
     Matrix(size_t rows, size_t cols):
        m(rows),
        n(cols) {
            data = new T* [rows];
            for (size_t i = 0; i != rows; ++i) {
                try {
                data[i] = new T[cols];
                } catch (...) {
                    for (size_t j = 0; j != i; ++j)
                        delete [] data[j];
                    delete [] data;
                    throw;
                }
                for (size_t j = 0; j != cols; ++j) 
                    data[i][j] = 0;
            }
        }

     ~Matrix() {
        for (size_t i = 0; i != m; ++i)
            delete [] data[i];
        delete [] data;
     }
};

//А теперь гвоздь нашей программы - умный указатель!
#include <iostream>
#include <memory>

int main() {
    //int * ptr = new int(42);
    //Используем умный указатель:
    std::unique_ptr<int> ptr (new int (42));

    std::cout << *ptr << "\n";
    //И теперь даже не нужно вызывать delete!
    //delete ptr;
 
    //Попробуем скопировать объект типа unique_ptr
    auto ptr2(ptr); // compilation error!
}

//Потенциальный источник багов:
//Почему? Ну, вызовем foo() в main и не сохраним указатель.
char * foo() {
    // ...
    return new char[100];
}

//Хороший вариант:
std::unique_ptr<char> foo() {
    //...
    return std::unique_ptr<char>(new char);
}

int main() {
    //И теперь можно безболезненно написать вот так!
    //Заметьте, что здесь умный указатель не скопируется из foo, а переместится!
    foo();
}

//Напишем свой класс unique_ptr:
namespace mystd {
template <typename T>
class unique_ptr {
 private:
     T * ptr;

 public:
     //Запретим неявное преобразование типов при инициализации путем добавления
     //ключевого слова expicit
     explicit unique_ptr(T * src = nullptr):
         ptr(src) {
     }
     //Такие конструкторы генерируются компилятором по умолчанию, но мы их запретим:
     unique_ptr(const unique_ptr&) = delete;
     void operator = (const unique_ptr&) = delete;

     //Конструктор перемещения:
     unique_ptr(unique_ptr&& tmp): ptr(tmp.ptr) {
        tmp.ptr = nullptr;
     }

     //Оператор присваивания:
     unique_ptr& operator = (unique_ptr&& tmp) {
         //WARNING! Надо вернуть память, на которую указывал старый указатель:
         delete ptr;

         ptr = tmp.ptr;
         tmp.ptr = nullptr;
         return *this;
     }

     //Перегрузка оператора присваивания:
     unique_ptr& operator = (T* src) {
        delete ptr;
        ptr = src;
        return *this;
     }

     ~unique_ptr() noexcept {
        delete ptr;
     }

     const T& operator *() const {
        return *ptr;
     }
};
}

mystd::unique_ptr<char> foo() {
    return mystd::unique_ptr<char>(new char);
}

int main() {
    //Здесь будет вызван конструктор перемещения:
    auto ptr(foo());
    
    mystd::unique_ptr<int> ptr1;
    ptr1 = new int(17);

    //Следующие две строчки эквивалентны
    mystd::unique_ptr<int> ptr2(static_cast<mystd::unique_ptr<int>&&>(ptr1));
    mystd::unique_ptr<int> ptr2(std::move(ptr1));
}

//Немного про shared_ptr
#include <memory>
int main() {
    //Умный указатель, который внутри имеет счетчик ссылок, и поэтому позволяет копировать себя.
    //Память очистится в деструкторе последнего объекта, ссылающегося на память.
    std::shared_ptr<int> ptr1(new int(17));
    auto ptr2 = ptr1;

    std::shared_ptr<int> ptr3;
    ptr3 = ptr1;
}
