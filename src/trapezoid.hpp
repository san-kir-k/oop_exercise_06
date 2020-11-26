// М8О-206Б-19 Киреев А.К.
#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

// вспомогательные функции для сортировки по полярному углу
template <class T>
std::pair<T, T> get(const std::pair<T, T>& lhs, const std::pair<T, T> rhs) { // получаем вектор по двум точкам
    return std::make_pair(rhs.first-lhs.first, rhs.second-lhs.second);
}

template <class T>
T cross(const std::pair<T, T>& lhs, const std::pair<T, T> rhs) { // cross product
    return lhs.first * rhs.second - lhs.second * rhs.first;
} // конец вспомогательных функций

template <class T>
class Trapezoid {
    public:
        using vertex = std::pair<T, T>;
        vertex a, b, c, d;
        bool valid;
        Trapezoid() = default;
        Trapezoid(std::istream& is);
        friend std::ostream& operator<<(std::ostream& os, const Trapezoid& t);
        double area();
        ~Trapezoid() = default;
}; 

template <class T>
Trapezoid<T>::Trapezoid(std::istream& is) {
    is >> a.first >> a.second
    >> b.first >> b.second
    >> c.first >> c.second
    >> d.first >> d.second;
    std::vector<vertex> points = {a, b, c, d};
    std::sort(points.begin(), points.end(), [](auto& lhs, auto& rhs) {
    if (lhs.first == static_cast<T>(0) &&
        lhs.second == static_cast<T>(0)) {
        return true;
    }
    if (rhs.first == static_cast<T>(0) &&
        rhs.second == static_cast<T>(0)) {
        return false;
    }
    std::pair<T, T> p0 = std::make_pair(static_cast<T>(0), static_cast<T>(0));
    std::pair<T, T> l = get(p0, lhs);
    std::pair<T, T> r = get(p0, rhs);
    return cross(l, r) < 0;
});
    a = points[0];
    b = points[1];
    c = points[2];
    d = points[3];
    double diag1 = (c.first - a.first)*(c.first - a.first) + 
    (c.second - a.second)*(c.second - a.second); 
    double diag2 = (d.first - b.first)*(d.first - b.first) + 
    (d.second - b.second)*(d.second - b.second); 
    double segm1 = (c.first - b.first)*(c.first - b.first) + 
    (c.second - b.second)*(c.second - b.second); 
    double segm2 = (d.first - a.first)*(d.first - a.first) + 
    (d.second - a.second)*(d.second - a.second);
    double segm3 = (a.first - b.first)*(a.first - b.first) + 
    (a.second - b.second)*(a.second - b.second); 
    double segm4 = (d.first - c.first)*(d.first - c.first) + 
    (d.second - c.second)*(d.second - c.second);  
    if ((diag1 == diag2) && ((segm1 != segm2 && segm3 == segm4) || 
    (segm1 == segm2 && segm3 != segm4))) {
        valid = true;
    } else {
        valid = false;
    }
}

template <class T>
std::ostream& operator<<(std::ostream& os, Trapezoid<T>& t) {
    if (t.valid) {
        os << "["
        << " (" << t.a.first << ", " << t.a.second << "), "
        << "(" << t.b.first << ", " << t.b.second << "), "
        << "(" << t.c.first << ", " << t.c.second << "), "
        << "(" << t.d.first << ", " << t.d.second << ") "
        << "]\n";
    }
    return os;
}

template <class T>
double Trapezoid<T>::area() {
    return 0.5 * abs(a.first * b.second + b.first * c.second + c.first * d.second + d.first * a.second - 
    b.first * a.second - c.first * b.second - d.first * c.second - a.first * d.second);
}
