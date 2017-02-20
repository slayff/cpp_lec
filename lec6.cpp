// Лекция 6
// Наследование(продолжение), виртуальные функции, абстрактные классы.

#include <iostream>
#include <vector>
class Animal {
 public:
     //virtual std::string voice() const = 0;
    virtual std::string voice() const {
        return "Not implemented yet";
    }
    /*
    ~Animal() {
        std::cout << "~Animal()\n";
    }
    */
    virtual ~Animal() = 0;
};

class Cat: public Animal {
 public:
    Cat() {
    }
    std::string voice() const {
        return "Meow";
    }
    ~Cat() override {
        std::cout << "~Cat()\n";
    }
};

class Dog: public Animal {
 public:
    Dog() {
    }
    std::string voice() const {
        return "Woof";
    }
    ~Dog() override {
        std::cout << "~Dog()\n";
    }
};

int main() {
    // Следующий код не корректен, так как Animal сейчас - абстрактный класс.
    // Animal a;
    // std::cout << a.voice() << '\n';

    Cat c;
    std::cout << c.voice() << '\n';

    Dog d;
    std::cout << d.voice() << '\n';

    //Вжух, и следующий код работает так, как мы ожидаем.
    std::vector<Animal*> animals;
    animals.push_back(&c);
    animals.push_back(&d);
    for (auto x : animals)
        std::cout << x->voice() << '\n';
}

// Чтобы обезопасить себя от ошибок во время переопределения функций
// можно дописать ключевое слово override:
return_value function_name(arguments) const override {
}

// Ключевое слово final запрещает переопределять функции
// В классе Animal по сути функция voice бессмысленная, и объекты типа Animal
// мы создавать не хотим. Чтобы запретить создание экземпляров класса нужно
// создать внутри класса хотя бы одну чисто виртуальную функцию:
virtual std::string voice() = 0;
// В принципе тело функции можно было и оставить, но написать вне класса.
// Чисто виртуальная функция обязательно должна быть переопределена

//===========================
// Вроде бы простая функция, но объекты создаются на стеке локально в функции
// и там же умирают - ссылки станут невалидными.
std::vector<Animal*> factory() {
    std::string line;
    std::vector<Animal*> result;
    while (std::cin >> line) {
        if (line == "cat") {
            Cat c;
            result.push_back(&c);
        } else if (line == "dog") {
            Dog d;
            result.push_back(&d);
        }
    }
    return result;
}

// Вариант получше, но проблемы с утечкой памяти -
// каждый объект в динамической памяти придется вручную удалить
std::vector<Animal*> factory() {
    std::string line;
    std::vector<Animal*> result;
    while (std::cin >> line) {
        if (line == "cat") {
            result.push_back(new Cat);
        } else if (line == "dog") {
            Dog d;
            result.push_back(new Dog);
        }
    }
    return result;
}

// Почти оптимальный вариант - использование умных указателей:
#include <memory>
std::vector<std::unique_ptr<Animal>> factory() {
    std::string line;
    std::vector<std::unique_ptr<Animal>> result;
    while (std::cin >> line) {
        if (line == "cat") {
            result.push_back(std::unique_ptr<Cat>(new Cat));
        } else if (line == "dog") {
            Dog d;
            result.push_back(std::unique_ptr<Dog>(new Dog));
        }
    }
    return result;
}

// Но лучше всего было бы использовать std::make_unique<Cat>()
// Либо result.emplace_back(new Cat)
std::vector<std::unique_ptr<Animal>> factory() {
    std::string line;
    std::vector<std::unique_ptr<Animal>> result;
    while (std::cin >> line) {
        if (line == "cat") {
            result.emplace_back(new Cat);
        } else if (line == "dog") {
            Dog d;
            result.emplace_back(new Dog);
        }
    }
    return result;
}

int main() {
    auto animals = factory();
    for (const auto& animal : animals) {
        std::cout << animal->voice() << '\n';
    }
}

// Прописав деструкторы, можно увидеть, что программа до сих пор работает некорректно:
// Вызываeтся деструктор базового класса - память все-таки утекает.
// В нашем примере в Dog и Cat полей нет, но в реальном примере там может что-то лежать.
// Деструктор для Dog и Cat не вызовется - память утечет.
// Решение - объявить деструктор базового класса виртуальным.

//Еще одна увлекательная задачка
class A {
 public:
     A() {std::cout << "A()\n";}
     ~A() {std::cout << "~A()\n";}
};

class B: public A {
 public:
     B() {std::cout << "B()\n";}
     ~B() {std::cout << "~B()\n";}
};

class C {
 public:
     C() {std::cout << "C()\n";}
     ~C() {std::cout << "~C()\n";}
};

class D: public B {
 public:
     C c;
     D() {std::cout << "D()\n";}
     ~D() {std::cout << "~D()\n";}
};

// Скомпилируйте и посмотрите, как именно инициализируется объект класса D
int main() {
    D d;
    std::cout << "D-object has been built\n";
}
// На доске было показано, как именно инициализируется объект типа D
// (в каком порядке собирается матрешка из наследников вы можете посмотреть, скомпилировав код)
// и сколько памяти он занимает(к слову, 48 Byte)
