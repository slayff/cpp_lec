// Лекция 3 - 30.01.17

#include <iostream>

//Константные указатели, указатели на const объекты...
int main() {
    T* ptr1;
    const T* ptr2; // the data in the pointer is fixed
    T const* ptr3; //
    T* const ptr4; // the pointer itself is fixed
    const T* const ptr5;
    // read from right to left
}

void foo (int * ptr) {
    ++ptr; // - Это инкрементация указателя, а не значения!
}

void foo (int * const ptr) {
    ++*ptr; // Хороший вариант - передавать константный указатель.
}

//Новая тема:
//Идиома RAII
//Resource acquisition is initialization
//Получение ресурса должно быть инициализацией

//Как работалось с файлами в C:
//Просто для демонстрации
#include <stdio.h>
int main() {
    FILE * f;
    f = fopen("filename.txt", "r");
    //Если вдруг файл не открылся:
    if (f == NULL) {
        printf("Error!\n");
        return 42;
    }
    int x;
    fscanf(f, "%d", &x);
    fprintf(f, "goodbye!\n");

    fclose(f);
}

//Пример реализации идиомы RAII:
//Можно написать свой класс Exception:
class TFileException {
    //...
};

//Класс обертка над файлом
class TFile {
 private:
     FILE * f;
 public:
     TFile(const char* name, const char* mode) {
         f = fopen(name, mode);
         //Если произошла ошибка при открытии файла, нужно бросить ошибку
         if (f == nullptr) {
             throw TFileException();
         }
     }

     void Write(const char * s) {
        fprintf(f, s);
        //По аналогии с открытием файла, нужно проверить получилось ли записать:
        // if (....) {
        // trow ....;
        // }
     }
    //В деструкторе закроем файл:
     ~TFile() {
         if (f != nullptr) {
            fclose(f);
         }
     }

     TFile(const TFile&) = delete; // Запрещаем конструктор копирования
     TFile& operator = (const TFile&) = delete;

     //Конструктор перемещения:
     TFile(TFile&& other) {
         f = other.f;
         other.f = nullptr;
     }

     TFile& operator = (TFile&& other) {
        f = other.f;
        other.f = nullptr;
        return *this;
     }
};

int main() {
    //Пример использования:
    TFile f ("test.txt", "w");
    f.Write("hello");
    //То есть выделение ресурсов нужно оборачивать в специальный класс
    TFile f2(f); // Будет плохо - два раза вызовется деструктор, 2 раза закрыть один файл? Ну, такое.
    // Чтобы это исправить, можно запретить конструктор копирования.
    // Окей, конструктор копирования мы запретили, тепепрь сделаем так:
    // Кстати, здесь мы еще потеряем указатель на файл test2.txt
    TFile f3("test2.txt", "r");
    f3 = f1;
    //Можно опять же запретить оператор присваивания...
}

//Использование конструктора перемещения:
TFile MyOpen() {
    TFile f("a.txt", "r");
    return f;
}

int main() {
    TFile f = MyOpen();
    TFile f3(f2); // Так сделать по прежнему нельзя, здесь вызывается конструктор копирования
    TFile f3(std::move(f2)); // А вот так можно. Нужно подключить <utility>
}

//Легкая задачка на собеседовании:
//(Как правильно написать класс C в котором поле - указатель на объект типа A
//Определить для него конструктор копирования, оператор присваивания.
class A {

};

class C {
private:
    A * x;
public:
    C(): x(new A) {
    }

    C(const C& other): x(new A(*other.x)) {
    }

    ~C() {
        delete x;
    }
    C& operator = (const C& other) {
        *x = *other.x; //По умолчанию оператор копирования скопирует указатель, а не данные
        return *this;
        
    }
};

int main() {
    C c1;
    C c2(c1);
}

//Задачка посложнее (теперь поддерживаем две ссылки на два разных объекта):
class A {
    //...
};

class B {
    //...
};
//Если вдруг при создании С для А все прошло успешно, а для B выкинулся Exception, то все плохо.
class C {
private:
    A * x;
    B * y;
public:
    C():
        x(new A),
        y(new B) // Здесь может быть Exception
                 // И произойдет утечка памяти, ведь объект C еще не создался, для x деструктора нет. Память под A мы не освободим.
    {
    }

    //Лучше написать так:
    //(Но еще лучше обернуть указатели в отдельный класс для которых может быть вызван деструктор)
    //(Воспользоваться идиомой RAII)
    C() {
        x = new A;
        try {
            y = new B;
        } catch (...) {
            delete x;
            throw;
        }
    }
    
    ~C() {
        delete x;
        delete y;
    }
};

