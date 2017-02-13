//Лекция 5
//Тема 1. Возможная реализация shared_ptr

#include <iostream>
#include <memory>

namespace mystd {
template <typename T>
class shared_ptr {
 private:
     T * ptr;
     //static size_t counter; - плохой вариант
     //static std::map<T*, size_t> counter; - плохой вариант
     size_t * counter; // хороший вариант

 public:
     shared_ptr(): ptr(nullptr), counter(nullptr) {
     }

     explicit shared_ptr(T* raw_ptr):
         ptr(raw_ptr),
         counter(new size_t(1))
    {
        // При инициализации counter редко, но все же может произойти исключение
        // из-за нехватки памяти. В этом случае надо удалить ptr (обработать исключение)
    }

    explicit shared_ptr(const shared_ptr& other):
        ptr(other.ptr),
        counter(other.counter) {
            ++*counter;
    }

    shared_ptr& operator = (const shared_ptr& other) {
        //Наивный (и плохой) вариант:
        //~shared_ptr();
        //Хороший вариант:
        release(); // (Вынесли все, что было в деструкторе в отдельную функцию)
        ptr = other.ptr;
        counter = other.counter;
        ++*counter;
        return *this;
    }

    ~shared_ptr() {
        // Т.к в конструкторе по умолчанию counter может проинициализироваться nullptr
        // нужно сделать проверку if (counter == nullptr)
        --*counter;
        if (*counter == 0) {
            delete ptr;
            delete counter;
        }
    }

    T& operator *() {
        return *ptr;
    }

    T* operator ->() {
        return ptr;
    }

    size_t use_count() const {
        return *counter;
    }

 private:
    void release() {
        --*counter;
        if (*counter == 0) {
            delete ptr;
            delete counter;
        }
    }
};
}

int main() {
    mystd::shared_ptr<int> p1(new int(17));
    auto p2 = p1; //Конструктор копирования мы не сделали.
    mystd::shared_ptr<int> p3(new int(23));
    p3 = p2;
    std::cout << *p3 << "\n";
}

//Тема 2. Наследование
//Обычно в книжках приводят стандартный пример с геометрическими фигурами.
//По запросу зала будем разбирать наследование на примере животных.

#include <iostream>
#include <string>

class Animal {
 public:
     enum Color {
        UNKNOWN,
        RED,
        GREEN,
        BLUE
     };

 private:
     Color color;
     std::string name;
     std::string owner;

 public:
     Animal() {
     }

     Animal(const std::string& name_):
         color(Color::UNKNOWN),
         name(name_) {
    }

     std::string voice() const {
        //Not implemented yet
        return "Not implemented";
     }
};

//Здесь мы наследуемся от базового класса Animal
//Модификатор public говорит о том, что все поля public родительского класса останутся public
//и в наследнике

class Cat: public Animal {
 private:
     int type;

 public:
     Cat() {}
     Cat(std::string& name_):
         Animal(name_),
         type(0) {
    }
     //Здесь мы перегрузили функцию voice родительского класса.
     std::string voice() const {
        return "Meow";
     }
};

class Dog: public Animal {
 private:
     int type;

 public:
     std::string voice() const {
        return "Woof";
     }
};

#include <vector>
int main() {
    Animal a("Noname1");
    std::cout << a.voice() << '\n';

    Cat c; //Не заработает, если нет конструктора по умолчанию у родительского класса.
    //Решение: Animal() {}, впрочем не очень красивое. Либо добавить конструктор для класса Cat
    std::cout << c.voice() << '\n';

    Dog d;
    std::cout << d.voice() << '\n';

    //Попробуем создать контейнер с объектами разных классов.
    std::vector<Animal> animals;
    animals.push_back(c);
    animals.push_back(d);
    std::cout << "=========\n";
    for (auto animal : animals)
        std::cout << animal.voice() << '\n';
    //Внезапно, такой код не даст желаемого эффекта.
}

//Лирическое отступление.
//Здесь это не наследование, а композиция. Лектор говорит, что при прочих равных
//всегда предпочтительно использовать композицию, а не наследование.
class Cat2 {
 private:
     int type;
 public:
     Animal base;
};


//Немного про множественное наследование
class A {
 public:
     int x;
};

class B: /*virtual*/ public A {
};

class C: /*virtual*/ public C {
};

//Множественное наследование:
//В классе D два поля x!
//Чтобы побороть такое можно воспользоваться модификатором virtual. (Не будем углубляться в это)
class D: public B, public C {
};
