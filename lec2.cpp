﻿//Модуль 3. Лекция 2. Указатели. Выделение динамической памяти.
#include <iostream>

//Здесь должны быть первые 10 минут лекций, но их нет :(

//Пример невалидной ссылки (x умирает в функции)
int& f() {
    int x = 17;
    return x;
}

//Пример невалидного указателя: x умрет в блоке, указатель станет невалидным.
int main() {
    int * ptr;
    {
        int x = 17;
        ptr = &x;
    }
    std::cout << *ptr << "\n";
}


int main() {
    int x = 17;
    //создаем переменную в динамической памяти:
    new int (17); //создали переменную, но не знаем её адрес. Более того, мы ее не удалили =>
                  //утечка памяти.
}

//Так лучше даже не пробовать делать. Поверьте. (Я предупреждал)
int main() {
    while (true) {
        new int(42);
    }
}

int main() {
    //Выделим память, получим на нее адрес, в этой ячейке памяти сконструируем объект инт
    //new <=> выделение памяти, конструирование объекта в этой области памяти
    //new ... вернет указатель, его мы и сохраняем в переменную ptr
    int * ptr = new int(42);
    *ptr = 17;
    std::cout << *ptr << "\n";

    //Delete <=> вызывается деструктор для объекта(для int этот шаг пропустится),
    //возвращаем память для ОС. delete от nullptr корректно отработает.
    delete ptr;

    //Это ошибка. Undefined behaviour.
    int x;
    delete &x;
}

//Как выделять динамическую память под сложные объекты?
//Во время выполнения оператора delete вызывается деструктор объекта
class C {
public:
    C() {
        std::cout << "C()\n";
    }
    ~C() {
        std::cout << "~C()\n";
    }
};

int main() {
    C* ptr = new C;
    delete ptr;
}


//Выделяем блок памяти
int main() {
    //Запросили память под 10 ячеек(4*10 = 40 bytes).
    //В каждой ячейке сконструировали объект int
    int * ptr = new int [10];
    //Записали в первую ячейку 17:
    *ptr = 17;
    //Как записать значение во вторую ячейку?
    *(ptr + 1) = 42;

    //Сдвигаемся вправо/влево в блоке памяти:
    // ptr + i;
    // ptr - i;
    // ptr++; ++ptr; --ptr; ptr--;
    
    //Вычитание двух указателей:
    int *ptr2 = ptr + 9;
    ptr2 - ptr;
    //Можно сконструировать указатель за последнюю ячейку памяти
    //(например, чтобы считать размер блока памяти):
    int * ptr2 = ptr + 10;

    //Как удалить такой блок памяти?
    delete ptr; //Неверно!
    delete[] ptr; //Корректно. delete[] - отдельный оператор.
    //Хорошо запомнить, что new - delete, new[] - delete[] идут парами.
}

//Посмотрим как были реализованы строки в C
int main() {
    char * s = new char[100];
    //По сути "под капотом" std::string вот такой динамический массив.
    std::cin >> s;
    std::cout << "Hello, " << s << '\n';
    //Не забудьте вызывать оператор delete[], чтобы вернуть память ОС.
    //Если этого не сделать, произойдет учейка памяти.
    delete[] s;

    //Если выделить слишком мало памяти (н-р new char[10]) 
    //программа повалится при вводе большой строки.

    //Строки в C должны заканчиваться символом '\0', они называются null-terminated strings.
    //При вводе строки длиной > 4 она обрежется, т.к. в s[4] принудительно помещен 0.
    char * s = new char[10];
    std::cin >> s;
    s[4] = 0; //или s[4] = '\0';
    std:: cout << "Hello, " << s <<"\n";
    delete[] s;
}

int main() {
    //Так делать плохо, нужно обязательно выделять память! Здесь же s указывает непонятно на что.
    char * s;
    std::cin >> s;
    std::cout << s << '\n';
    //Более того, нужно вызывать оператор delete[]

    //Корректно вот так:
    char * s = new char[100];
    std::cin >> s;
    std::cout << s << '\n';
    delete[] s;
}

//Про сравнение строк в С
#include <cstring>
int main() {
    char * s1 = new char[100];
    char * s2 = new char[10];
    std::cin >> s1;
    std::cin >> s2;
    std::cout << (s1 == s2) << "\n";
    delete[] s1;
    delete[] s2;
    //Как ни странно такой код всегда выведет 0.
    //s1 == s2 сравнивает не строки на равенство, а равенство указателей.
    //Указатели равны, если они указывают на один блок памяти.
    
    //В <cstring> есть функция strcmp
    std::cout << strcmp(s1, s2) << "\n";
    //Возвращает 0, если равны. Положительное/отрицательное число, если не равны (см. cppreference.com)
}

#include <algorithm>

int main() {
    char * s1 = new char[100];
    char * s2 = new char[100];
    std::cin >> s1;
    //Скопируем s1 в s2, copy лежит в <algorithm>
    //Заметьте, что если бы блок памяти s2 имел размер 10, то такой код бы не сработал.
    std::copy(s1, s1+100, s2);
    //Еще один способ (будет работать даже если строки имеют разную длину)
    strcpy(s2, s1);
}

//А теперь посмотрим, как оно реализовано на самом деле (реализуем strcpy):
void my_strcpy(char * dst, const char * src) {
    while (*dst++ = *src++);
}
