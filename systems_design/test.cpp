#include <bits/stdc++.h>
using namespace std;

struct T {
    T() {
        cout << "default constructor" << endl;
    }

    T(const T&) {
        cout << "copy constructor" << endl;
    }

    T(T&&) {
        cout << "move constructor" << endl;
    }

    T& operator=(const T& t) {
        cout << "assignment operator" << endl;
        return *this;
    }
};

inline T get() {
    return T();
}

inline void use(const T& t) {
    T a = t;
}

inline void use(T&& t) {
    T a = move(t);
}

int main() {
    T a = get();
    use(a);

    T b = get();
    use(move(b));

    a = b;
}