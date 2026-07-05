#include <nfx/Silicon.h>

#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;

// ---------------------------------------------------------------------------
// N-bit ripple carry adder built from primitive gates.
//
// Each bit cell is a full adder: S = A ^ B ^ Cin, Cout = (A&B)|(Cin&(A^B))
// Carry ripples from bit 0 (LSB) to bit N-1 (MSB).
//
//   bit 0          bit 1                bit N-1
//   +--------+     +--------+           +--------+
//   |   FA   |-+-->|   FA   |--> ... -->|   FA   |--> Cout (overflow)
//   +--------+ |   +--------+           +--------+
//   A0 B0 S0   carry                    AN-1 BN-1 SN-1
// ---------------------------------------------------------------------------

struct BitCell
{
    primitive::Xor xorAB;
    primitive::Xor xorS;
    primitive::And andAB;
    primitive::And andCinAB;
    primitive::Or orCout;

    Wire wireA;
    Wire wireB;
    Wire wireAB;
    Wire wireAndAB;
    Wire wireAndCinAB;

    Pin pinA;
    Pin pinB;

    explicit BitCell()
        : xorAB{ { .name = "XOR_AB" } },
          xorS{ { .name = "XOR_S" } },
          andAB{ { .name = "AND_AB" } },
          andCinAB{ { .name = "AND_CIN_AB" } },
          orCout{ { .name = "OR_COUT" } },
          wireA{ { .name = "A", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } },
          wireB{ { .name = "B", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } },
          wireAB{ { .name = "AB", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } },
          wireAndAB{ { .name = "AND_AB", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } },
          wireAndCinAB{
              { .name = "AND_CIN_AB", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull }
          },
          pinA{ { .name = "A", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
          pinB{ { .name = "B", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } }

    {
        wireA.attach(pinA);
        wireA.attach(xorAB.pin("A"));
        wireA.attach(andAB.pin("A"));

        wireB.attach(pinB);
        wireB.attach(xorAB.pin("B"));
        wireB.attach(andAB.pin("B"));

        wireAB.attach(xorAB.pin("Y"));
        wireAB.attach(xorS.pin("A"));
        wireAB.attach(andCinAB.pin("B"));

        wireAndAB.attach(andAB.pin("Y"));
        wireAndAB.attach(orCout.pin("A"));

        wireAndCinAB.attach(andCinAB.pin("Y"));
        wireAndCinAB.attach(orCout.pin("B"));
    }
};

struct RippleCarryAdder
{
    std::vector<std::unique_ptr<BitCell>> cells;
    std::vector<std::unique_ptr<Wire>> carries; // carries[i] = Cin of cell i
    std::vector<std::unique_ptr<Wire>> sums;

    Pin pinCarryIn;

    explicit RippleCarryAdder(std::size_t n)
        : pinCarryIn{ { .name = "C0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } }
    {
        for (std::size_t i = 0; i < n; ++i)
        {
            cells.push_back(std::make_unique<BitCell>());
            carries.push_back(std::make_unique<Wire>(Wire::Descriptor{
                .name = "Carry", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull }));
            sums.push_back(std::make_unique<Wire>(Wire::Descriptor{
                .name = "Sum", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull }));
        }
        carries.push_back(std::make_unique<Wire>(
            Wire::Descriptor{ .name = "Cout", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull }));

        carries[0]->attach(pinCarryIn);

        for (std::size_t i = 0; i < n; ++i)
        {
            auto& c = *cells[i];
            carries[i]->attach(c.xorS.pin("B"));
            carries[i]->attach(c.andCinAB.pin("A"));
            carries[i + 1]->attach(c.orCout.pin("Y"));
            sums[i]->attach(c.xorS.pin("Y")); // S = A ^ B ^ Cin
        }

        // drive after all wiring is complete so callbacks propagate correctly
        pinCarryIn.drive<Level>(Level::Low);
    }

    void setA(std::size_t i, Level l) { cells[i]->pinA.drive<Level>(l); }
    void setB(std::size_t i, Level l) { cells[i]->pinB.drive<Level>(l); }

    // sum wire is driven by xorS.pin("Y"), read directly here
    Level sum(std::size_t i) const { return sums[i]->read<Level>(); }
    Level cout() const { return carries[cells.size()]->read<Level>(); }

    void load(unsigned int a, unsigned int b)
    {
        // set B first so carry chain resolves correctly when A triggers xorAB
        for (std::size_t i = 0; i < cells.size(); ++i)
        {
            setB(i, ((b >> i) & 1u) ? Level::High : Level::Low);
        }
        for (std::size_t i = 0; i < cells.size(); ++i)
        {
            setA(i, ((a >> i) & 1u) ? Level::High : Level::Low);
        }
    }

    unsigned int result() const
    {
        unsigned int r = 0;
        for (std::size_t i = 0; i < cells.size(); ++i)
        {
            if (sum(i) == Level::High)
                r |= (1u << i);
        }
        if (cout() == Level::High)
        {
            r |= (1u << cells.size());
        }
        return r;
    }
};

static void printBin(unsigned int v, std::size_t bits)
{
    for (int i = static_cast<int>(bits) - 1; i >= 0; --i)
    {
        std::cout << ((v >> i) & 1u);
    }
}

int main()
{
    constexpr std::size_t N = 4;
    constexpr unsigned int MAX = (1u << N) - 1;

    RippleCarryAdder adder{ N };

    std::cout << N << "-bit ripple carry adder\n\n"
              << "  A          +  B          =  Result\n"
              << "  -----------+-------------+----------\n";

    for (unsigned int a = 0; a <= MAX; ++a)
    {
        for (unsigned int b = 0; b <= MAX; ++b)
        {
            adder.load(a, b);
            const unsigned int r = adder.result();
            std::cout << "  ";
            printBin(a, N);
            std::cout << " (" << std::setw(2) << a << ")  +  ";
            printBin(b, N);
            std::cout << " (" << std::setw(2) << b << ")  =  ";
            printBin(r, N + 1);
            std::cout << " (" << std::setw(2) << r << ")\n";
        }
    }
}
