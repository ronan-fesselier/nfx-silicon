#include <nfx/Silicon.h>

#include <iostream>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;

int main()
{
    // Half adder: S = A ^ B, C = A & B
    primitive::Xor sumGate{ primitive::Xor::Descriptor{ .name = "SUM" } };
    primitive::And carryGate{ primitive::And::Descriptor{ .name = "CARRY" } };

    // Input pins
    Pin pinA{ Pin::Descriptor{ .name = "A", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
    Pin pinB{ Pin::Descriptor{ .name = "B", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

    // Wiring
    Wire wireA{ Wire::Descriptor{ .name = "A", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireB{ Wire::Descriptor{ .name = "B", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireS{ Wire::Descriptor{ .name = "S", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireC{ Wire::Descriptor{ .name = "C", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };

    wireA.attach(pinA);
    wireA.attach(sumGate.pin("A"));
    wireA.attach(carryGate.pin("A"));

    wireB.attach(pinB);
    wireB.attach(sumGate.pin("B"));
    wireB.attach(carryGate.pin("B"));

    wireS.attach(sumGate.pin("Y"));
    wireC.attach(carryGate.pin("Y"));

    const auto bit = [](Level l) { return l == Level::High ? 1 : 0; };

    const auto print = [&](Level a, Level b) {
        pinA.drive<Level>(a);
        pinB.drive<Level>(b);
        std::cout << "  " << bit(a) << "   " << bit(b) << "  |  " << bit(wireS.read<Level>()) << "  "
                  << bit(wireC.read<Level>()) << "\n";
    };

    std::cout << "Half Adder - binary addition of two 1-bit values\n"
              << "\n"
              << "  A   B  |  S  C\n"
              << "---------+--------\n";
    print(Level::Low, Level::Low);
    print(Level::Low, Level::High);
    print(Level::High, Level::Low);
    print(Level::High, Level::High);

    return 0;
}
