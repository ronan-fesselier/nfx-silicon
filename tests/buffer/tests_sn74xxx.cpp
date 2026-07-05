#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::buffer::sn74xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("buffer::sn74xxx::LS245")
{
    TEST_CASE("Construction creates 20 pins")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        CHECK(u.pins().size() == 20);
    }

    TEST_CASE("Pin names match DIP-20 datasheet")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        CHECK(u.pin("DIR").descriptor().name == std::string_view("DIR"));
        CHECK(u.pin("A1").descriptor().name == std::string_view("A1"));
        CHECK(u.pin("A2").descriptor().name == std::string_view("A2"));
        CHECK(u.pin("A3").descriptor().name == std::string_view("A3"));
        CHECK(u.pin("A4").descriptor().name == std::string_view("A4"));
        CHECK(u.pin("A5").descriptor().name == std::string_view("A5"));
        CHECK(u.pin("A6").descriptor().name == std::string_view("A6"));
        CHECK(u.pin("A7").descriptor().name == std::string_view("A7"));
        CHECK(u.pin("A8").descriptor().name == std::string_view("A8"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("B8").descriptor().name == std::string_view("B8"));
        CHECK(u.pin("B7").descriptor().name == std::string_view("B7"));
        CHECK(u.pin("B6").descriptor().name == std::string_view("B6"));
        CHECK(u.pin("B5").descriptor().name == std::string_view("B5"));
        CHECK(u.pin("B4").descriptor().name == std::string_view("B4"));
        CHECK(u.pin("B3").descriptor().name == std::string_view("B3"));
        CHECK(u.pin("B2").descriptor().name == std::string_view("B2"));
        CHECK(u.pin("B1").descriptor().name == std::string_view("B1"));
        CHECK(u.pin("/OE").descriptor().name == std::string_view("/OE"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        CHECK(&u.pin(LS245::Terminal::DIR) == &u.pin("DIR"));
        CHECK(&u.pin(LS245::Terminal::A1) == &u.pin("A1"));
        CHECK(&u.pin(LS245::Terminal::A2) == &u.pin("A2"));
        CHECK(&u.pin(LS245::Terminal::A3) == &u.pin("A3"));
        CHECK(&u.pin(LS245::Terminal::A4) == &u.pin("A4"));
        CHECK(&u.pin(LS245::Terminal::A5) == &u.pin("A5"));
        CHECK(&u.pin(LS245::Terminal::A6) == &u.pin("A6"));
        CHECK(&u.pin(LS245::Terminal::A7) == &u.pin("A7"));
        CHECK(&u.pin(LS245::Terminal::A8) == &u.pin("A8"));
        CHECK(&u.pin(LS245::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS245::Terminal::B8) == &u.pin("B8"));
        CHECK(&u.pin(LS245::Terminal::B7) == &u.pin("B7"));
        CHECK(&u.pin(LS245::Terminal::B6) == &u.pin("B6"));
        CHECK(&u.pin(LS245::Terminal::B5) == &u.pin("B5"));
        CHECK(&u.pin(LS245::Terminal::B4) == &u.pin("B4"));
        CHECK(&u.pin(LS245::Terminal::B3) == &u.pin("B3"));
        CHECK(&u.pin(LS245::Terminal::B2) == &u.pin("B2"));
        CHECK(&u.pin(LS245::Terminal::B1) == &u.pin("B1"));
        CHECK(&u.pin(LS245::Terminal::nOE) == &u.pin("/OE"));
        CHECK(&u.pin(LS245::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: A and B are HighZ")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);
        src.drive<Level>(Level::High);

        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("OE=High: isolation, B is HighZ regardless of A")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        u.pin("DIR").drive<Level>(Level::High);
        u.pin("/OE").drive<Level>(Level::High);
        src.drive<Level>(Level::High);

        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("OE=HighZ: isolation")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        u.pin("DIR").drive<Level>(Level::High);
        src.drive<Level>(Level::High);

        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("DIR=High (A->B): A drives B")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        src.drive<Level>(Level::High);
        CHECK(u.pin("B1").read<Level>() == Level::High);

        src.drive<Level>(Level::Low);
        CHECK(u.pin("B1").read<Level>() == Level::Low);
    }

    TEST_CASE("DIR=Low (B->A): B drives A")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::Low);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wb{ Wire::Descriptor{ .name = "WB", .kind = Pin::Kind::Digital } };
        wb.attach(src);
        wb.attach(u.pin("B1"));

        Pin obs{ Pin::Descriptor{ .name = "OBS", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(u.pin("A1"));
        wa.attach(obs);

        src.drive<Level>(Level::High);
        CHECK(obs.read<Level>() == Level::High);

        src.drive<Level>(Level::Low);
        CHECK(obs.read<Level>() == Level::Low);
    }

    TEST_CASE("DIR change reverses data flow")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);

        Pin srcA{ Pin::Descriptor{ .name = "SRCA", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin srcB{ Pin::Descriptor{ .name = "SRCB", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin obsA{ Pin::Descriptor{ .name = "OBSA", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Pin obsB{ Pin::Descriptor{ .name = "OBSB", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };

        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        Wire wb{ Wire::Descriptor{ .name = "WB", .kind = Pin::Kind::Digital } };
        wa.attach(srcA);
        wa.attach(u.pin("A1"));
        wa.attach(obsA);
        wb.attach(srcB);
        wb.attach(u.pin("B1"));
        wb.attach(obsB);

        // DIR=High: A->B
        u.pin("DIR").drive<Level>(Level::High);
        srcA.drive<Level>(Level::High);
        CHECK(obsB.read<Level>() == Level::High);

        srcA.release();
        u.pin("DIR").drive<Level>(Level::Low);
        srcB.drive<Level>(Level::Low);
        CHECK(obsA.read<Level>() == Level::Low);
    }

    TEST_CASE("A input HighZ -> B output HighZ (DIR=High)")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);

        // A1 has no driver -> B1 should be HighZ
        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC removed after operation: outputs return to HighZ")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        src.drive<Level>(Level::High);
        CHECK(u.pin("B1").read<Level>() == Level::High);

        u.pin("VCC").release();
        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("All 8 channels propagate independently (DIR=High)")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);

        const char* aPins[8] = { "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8" };
        const char* bPins[8] = { "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8" };
        const Level levels[8] = { Level::High, Level::Low,  Level::High, Level::Low,
                                  Level::Low,  Level::High, Level::Low,  Level::High };

        Pin src[8] = {
            Pin{ Pin::Descriptor{ .name = "S0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S3", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S4", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S5", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S6", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S7", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } }
        };
        Wire wa[8] = { Wire{ Wire::Descriptor{ .name = "W0", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W1", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W2", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W3", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W4", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W5", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W6", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W7", .kind = Pin::Kind::Digital } } };
        for (int i = 0; i < 8; ++i)
        {
            wa[i].attach(src[i]);
            wa[i].attach(u.pin(aPins[i]));
            src[i].drive<Level>(levels[i]);
        }
        for (int i = 0; i < 8; ++i)
        {
            CHECK(u.pin(bPins[i]).read<Level>() == levels[i]);
        }
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        CHECK(u.name() == std::string_view("U1"));
    }
}

TEST_SUITE("buffer::sn74xxx::LVC1G17")
{
    TEST_CASE("Construction creates 5 pins")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        CHECK(u.pins().size() == 5);
    }

    TEST_CASE("Pin names match SOT-23-5 datasheet")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        CHECK(u.pin("NC").descriptor().name == std::string_view("NC"));
        CHECK(u.pin("A").descriptor().name == std::string_view("A"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));
        CHECK(u.pin("Y").descriptor().name == std::string_view("Y"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        CHECK(&u.pin(LVC1G17::Terminal::NC) == &u.pin("NC"));
        CHECK(&u.pin(LVC1G17::Terminal::A) == &u.pin("A"));
        CHECK(&u.pin(LVC1G17::Terminal::GND) == &u.pin("GND"));
        CHECK(&u.pin(LVC1G17::Terminal::Y) == &u.pin("Y"));
        CHECK(&u.pin(LVC1G17::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: Y is HighZ")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("A").drive<Level>(Level::High);
        CHECK(u.pin("Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("A=High -> Y=High")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(3.3f);
        u.pin("A").drive<Level>(Level::High);
        CHECK(u.pin("Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=Low -> Y=Low")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(3.3f);
        u.pin("A").drive<Level>(Level::Low);
        CHECK(u.pin("Y").read<Level>() == Level::Low);
    }

    TEST_CASE("A=HighZ -> Y=HighZ")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(3.3f);
        CHECK(u.pin("Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Y follows A transitions")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("A").drive<Level>(Level::Low);
        CHECK(u.pin("Y").read<Level>() == Level::Low);

        u.pin("A").drive<Level>(Level::High);
        CHECK(u.pin("Y").read<Level>() == Level::High);

        u.pin("A").drive<Level>(Level::Low);
        CHECK(u.pin("Y").read<Level>() == Level::Low);
    }

    TEST_CASE("VCC removed after operation: Y returns to HighZ")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(3.3f);
        u.pin("A").drive<Level>(Level::High);
        CHECK(u.pin("Y").read<Level>() == Level::High);

        u.pin("VCC").release();
        CHECK(u.pin("Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC at lower bound (1.65V) is valid")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(1.65f);
        u.pin("A").drive<Level>(Level::High);
        CHECK(u.pin("Y").read<Level>() == Level::High);
    }

    TEST_CASE("VCC at upper bound (5.5V) is valid")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.5f);
        u.pin("A").drive<Level>(Level::High);
        CHECK(u.pin("Y").read<Level>() == Level::High);
    }

    TEST_CASE("VCC below lower bound: Y is HighZ")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(1.64f);
        u.pin("A").drive<Level>(Level::High);
        CHECK(u.pin("Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC above upper bound: Y is HighZ")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.51f);
        u.pin("A").drive<Level>(Level::High);
        CHECK(u.pin("Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LVC1G17 u{ LVC1G17::Descriptor{ .name = "U1" } };
        CHECK(u.name() == std::string_view("U1"));
    }
}
