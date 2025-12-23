#include <iostream>
#include <memory>

struct Shape {
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

struct Circle : Shape {
    double r;
    explicit Circle(double r) : r(r) {}
    double area() const override {
        return 3.14159 * r * r;
    }
};

int main() {
    std::unique_ptr<Shape> s = std::make_unique<Circle>(2.0);
    std::cout << s->area() << "\n"; // always dispatches correctly
}