#include <nfx/Silicon.h>

#include <iostream>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;

int main()
{
    // Full adder: S = A ^ B ^ Cin, Cout = (A & B) | (Cin & (A ^ B))
    primitive::Xor xor1{ primitive::Xor::Descriptor{ .name = "XOR1" } }; // A ^ B
    primitive::Xor xor2{ primitive::Xor::Descriptor{ .name = "XOR2" } }; // (A ^ B) ^ Cin -> S
    primitive::And and1{ primitive::And::Descriptor{ .name = "AND1" } }; // A & B
    primitive::And and2{ primitive::And::Descriptor{ .name = "AND2" } }; // Cin & (A ^ B)
    primitive::Or or1{ primitive::Or::Descriptor{ .name = "OR1" } };     // (A & B) | (Cin & (A ^ B)) -> Cout

    // Input pins
    Pin pinA{ Pin::Descriptor{ .name = "A", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
    Pin pinB{ Pin::Descriptor{ .name = "B", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
    Pin pinCin{ Pin::Descriptor{ .name = "Cin", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

    // Wiring
    Wire wireA{ Wire::Descriptor{ .name = "A", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireB{ Wire::Descriptor{ .name = "B", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireCin{ Wire::Descriptor{
        .name = "Cin", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireAB{ Wire::Descriptor{
        .name = "AB", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireS{ Wire::Descriptor{ .name = "S", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireAndAB{ Wire::Descriptor{
        .name = "AndAB", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireAndCinAB{ Wire::Descriptor{
        .name = "AndCinAB", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };
    Wire wireCout{ Wire::Descriptor{
        .name = "Cout", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };

    wireA.attach(pinA);
    wireA.attach(xor1.pin("A"));
    wireA.attach(and1.pin("A"));

    wireB.attach(pinB);
    wireB.attach(xor1.pin("B"));
    wireB.attach(and1.pin("B"));

    wireAB.attach(xor1.pin("Y")); // A ^ B
    wireAB.attach(xor2.pin("A"));
    wireAB.attach(and2.pin("B"));

    wireCin.attach(pinCin);
    wireCin.attach(xor2.pin("B"));
    wireCin.attach(and2.pin("A"));

    wireS.attach(xor2.pin("Y")); // S

    wireAndAB.attach(and1.pin("Y")); // A & B
    wireAndAB.attach(or1.pin("A"));

    wireAndCinAB.attach(and2.pin("Y")); // Cin & (A ^ B)
    wireAndCinAB.attach(or1.pin("B"));

    wireCout.attach(or1.pin("Y")); // Cout

    const auto bit = [](Level l) { return l == Level::High ? 1 : 0; };

    const auto print = [&](Level a, Level b, Level cin) {
        pinA.drive<Level>(a);
        pinB.drive<Level>(b);
        pinCin.drive<Level>(cin);
        std::cout << "  " << bit(a) << "  " << bit(b) << "    " << bit(cin) << "  |  " << bit(wireS.read<Level>())
                  << "     " << bit(wireCout.read<Level>()) << "\n";
    };

    std::cout << "Full Adder - binary addition of two 1-bit values with carry in\n"
              << "\n"
              << "  A  B  Cin  |  S  Cout\n"
              << "---------------+---------\n";
    print(Level::Low, Level::Low, Level::Low);
    print(Level::Low, Level::Low, Level::High);
    print(Level::Low, Level::High, Level::Low);
    print(Level::Low, Level::High, Level::High);
    print(Level::High, Level::Low, Level::Low);
    print(Level::High, Level::Low, Level::High);
    print(Level::High, Level::High, Level::Low);
    print(Level::High, Level::High, Level::High);

    return 0;
}
