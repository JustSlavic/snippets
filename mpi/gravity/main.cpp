#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <random>

const double G = 1;
const double density = 1000;
const double h = 0.001;


class Point {
public:
    Point() = default;
    Point(double x, double y) :x(x), y(y) {}

    double x = 0;
    double y = 0;
};


class Body {
public:
    Body() = default;
    Body(Point position, Point velocity, double mass)
      : position(position)
      , velocity(velocity)
      , mass(mass) {}

// private:
    Point position{0, 0};
    Point velocity{0, 0};
    double mass{0};
};

using System = std::vector<Body>;

System make_random_system(size_t count = 3) {
    System result;
    result.reserve(count);

    std::random_device rd;  // will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<double> dis(0, 10);

    for (size_t i = 0; i < count; ++i) {
        result.emplace_back(
            Point{dis(gen), dis(gen)},
            Point{dis(gen), dis(gen)},
            dis(gen));
    }

    return result;
}

void print_out_system(System& system) {
    for (auto body : system) {
        std::cout << "x: (" 
                  << body.position.x << ", " 
                  << body.position.y << "); v: ("
                  << body.velocity.x << ", "
                  << body.velocity.y << "); m: "
                  << body.mass << ';' 
                  << std::endl;
    }
}

int main(int argc, char** argv) {
    System system = make_random_system();
    print_out_system(system);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();





    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time duration = " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() 
              << "[µs]" 
              << std::endl;

    return 0;
}
