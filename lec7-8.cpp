// Лекция 7 - собственная реализация вектора

// Для начала упрощенный вариант - вектор с внутренним типом int

#include <iostream>
#include <algorithm>
class Vector {
 private:
    using T = int;
    //Зададим значения по умолчанию:
    T* data = nullptr;
    size_t sz = 0; // - size
    size_t cp = 0; // - capacity

 public:
     // Конструктор по умолчанию
     Vector();
     // Конструктор копирования
     Vector(const Vector& other):
         data(new int[other.sz]),
         sz(other.sz),
         cp(sz) {
         //Следующий код можно заменить вызовом std::copy()
         // for (size_t i = 0; i != other.sz; ++i)
         //    data[i] = other.data[i];
         //         data[i] эквивалентно *(data + i)
         std::copy(other.data, other.data + other.sz, data); // std::copy лежит в <algorithm>
     }
     // Конструктор перемещения
     Vector(Vector&& other):
         data(other.data),
         sz(other.sz),
         cp(other.cp) {
             other.data = nullptr;
             other.sz = 0;
             other.cp = 0;
     }
     // Это мы не реализовали
     Vector(size_t n, T elem);

     ~Vector() {
         // Это было для отладки
         // std::cout << "~Vector()" << std::endl;
         delete[] data;
     };

     // Это мы не реализовали
     Vector& operator =(const Vector& other);

     // Это мы не реализовали
     Vector& operator =(Vector&& other);

     // Это мы не реализовали
     size_t size() const;
     bool empty() const;

     T front() const;
     T& front();
     T back() const;
     T& back();

     T operator [](size_t i) const;
     T& operator [](size_t i);

     // NB: здесь что-то было не дописано.
     void resize(size_t n) {
         if (n <= sz) {
            sz = n;
            return;
         } else if (sz < n && n <= cp) {
             std::fill(data + sz, data + n, 0); // лежит в <algorithm>
             // Этот код эквивалентен такому циклу:
             // for (size_t i = sz; i != n; ++i)
             //     data[i] = 0;
            sz = n;
            return;
         } else {
            T* new_data = new T[n];
            std::copy(data, data + sz, new_data);
            delete[] data;
            data = new_data;
            sz = cp = n;

         }
     }
     void reserve(size_t n);

     void push_back(T elem);
     void pop_back();

};

//==========================================
// Лекция 8 - продолжение. Реализация vector<T>.
// Знакомство с placement new
//==========================================

#include <iostream>

// Сейчас мы попробуем создать "универсальный" вектор - шаблонный класс.
// Для корректности работы будем пользоваться новым оператором: placement new
template <typename T>
class Vector {
 private:
     T * data;
     size_t sz;
     size_t cp;

 public:
     // Конструктор копирования - плохой вариант
     /*
     Vector(const Vector& other) {
         data = new T[other.sz];
         for (size_t i = 0; i != other.sz; ++i)
             data[i] = other.data[i];
         sz = other.sz;
         cp = other.cp;
     } 
     */

     Vector(const Vector& other):
        // Обратите внимание на синтаксис, здесь мы выделяем память под массив объектов,
        // но не инициализируем ее - не вызываем конструктор для объектов класса T.
        // Кстати, у лектора здесь была ошибка. Вроде.
        data (static_cast<T*> (operator new[] (other.sz * sizeof(T)))),
        sz(other.sz),
        cp(other.cp) {
            size_t i = 0;
            // На случай, если на каком-то шаге итерации мы не сможем проинициализировать память
            // (то есть создать объект) нужно исключение перехватить.
            try {
                for (; i != sz; ++i)
                    new (data + i) T(other[i]); //Здесь память не выделяется, но инициализируется.
            } catch (...) {
                for (size_t j = 0; j != i; ++j) {
                    data[j].~T();
                }
                operator delete(data);
                throw; // Пробросили исключение на уровень выше.
            }
        }
     // Но и этот код Лектору(как и аудитории) не нравится. Код слишком громоздкий.
     // Воспользуемся идиомой RAII. Сделаем вспомогательный класс VectorBase.
     // Мы его объявили struct, чтобы подчеркнуть, что все его поля открыты.
};

#include <utility> // Для std::swap

template <typename T>
struct VectorBase {
    T * data;
    size_t cp;

    VectorBase():
        data(nullptr),
        cp(0) {
        }

    VectorBase(size_t n):
        data(static_cast<T*>(operator new(n * sizeof(T)))),
        cp(n) {
        }

    // Копии объекта этого класса нам не нужны, поэтому запретим конструктор копирования
    // и оператор присваивания.
    VectorBase(const VectorBase&) = delete;
    VectorBase& operator =(const VectorBase&) = delete;

    // А вот конструктор перемещения определим:
    VectorBase(VectorBase&& other):
        data(other.data),
        cp(other.cp) {
            other.data = nullptr;
            other.cp = 0;
        }
    // Оператор присваивания с move-семинтикой:
    VectorBase& operator = (VectorBase&& other) {
        // С помощью std::swap обменяемся содержимым наших полей
        std::swap(data, other.data);
        std::swap(cp, other.cp);
        return *this;
    }
    ~VectorBase() {
        operator delete (data);
    }

    // Здесь должна быть реализация swap:
    // void swap(VectorBase other) {}
};

// Реализация конструктора копирования Vector с использованием класса VectorBase:
template <typename T>
class Vector {
 private:
     VectorBase<T> base;
     size_t sz;

 public:
     Vector(const Vector& other):
        base(other.sz),
        sz(other.sz)
         {
            size_t i = 0;
            try {
                for (; i != sz; ++i)
                    new (base.data + i) T(other[i]);
            } catch (...) {
                for (size_t j = 0; j != i; ++j) {
                    base.data[j].~T();
                }
                throw;
            }
        }
     // Этот код можно вынести в отдельную функцию, что мы и сделаем:
};

// Возможная реализация std::uninitialize_copy
template<typename IterIn, typename IterOut>
IterOut uninitialized_copy (IterIn first, IterOut last, IterOut out) {
    using T = decltype(*out);
    IterOut current = out;
        try {
            while (first != last)
                new (&*current++) T(*first++);
            return current;
        } catch (...) {
            while (out != current) {
                out->~T();
                ++out;
            }
            throw;
        }
}
// Возможная реализация std::uninitialize_move
template<typename IterIn, typename IterOut>
IterOut uninitialized_move (IterIn first, IterOut last, IterOut out) {
    using T = decltype(*out);
    IterOut current = out;
        try {
            while (first != last)
                // Если я не ошибаюсь...
                new (&*current++) T(std::move(*first++));
            return current;
        } catch (...) {
            while (out != current) {
                out->~T();
                ++out;
            }
            throw;
        }
}


// Конечная реализация Vector (точнее, конструктора копирования)
template <typename T>
class Vector {
 private:
     VectorBase<T> base;
     size_t sz;

 public:
     // Впрочем, эта функция уже есть в стандартной библиотеке.
     Vector(const Vector& other):
        base(other.sz),
        sz(other.sz)
         {
             std::uninitialized_copy(
                     other.base.data,
                     other.base.data + sz,
                     base.data
                     )
        }
     size_t size() const {
        return sz;
     }

     size_t capacity() const {
        return base.cp;
     }
     
     void destroy() {
          for (size_t i = 0; i != sz; ++i)
             base.data[i].~T();
     }

     void reserve(size_t n) {
         if (n <= capacity())
             return;

         VectorBase<T> vb_new(n);
         uninitialized_move(
                 base.data,
                 base.data + size(),
                 vb_new.data
                 );
         destroy();
             base = std::move(vb_new);
     }

     Vector& operator = (const Vector& other) {
        Vector tmp(other);
        swap(tmp);
        return *this;
     }

     void swap(Vector& other) {
        base.swap(other.base);
        std::swap(sz, other.sz);
     }
     ~Vector() {
         destroy();
     }
};

