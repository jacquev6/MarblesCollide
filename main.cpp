#include <iostream>
#include <cmath>

#include <boost/format.hpp>
#include <boost/units/io.hpp>
#include <boost/assign.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <cairomm/cairomm.h>

#include "collide.hpp"

using namespace Cairo;
using namespace collide;
using namespace bu;

namespace ba = boost::assign;


class FramesDrawer : public EventsHandler {
public:
    FramesDrawer() :
        _i(0)
    {}

private:
    void begin(Simulation* s) {
        _s = s;
        tick();
    }

    void tick() {
        //std::cout << _s->t() << ": tick" << std::endl;
        RefPtr<ImageSurface> img = ImageSurface::create(FORMAT_RGB24, int(_s->width() / meter), int(_s->height() / meter));
        RefPtr<Context> ctx = Context::create(img);
        ctx->set_source_rgb(.9, .9, .9);
        ctx->paint();
        ctx->set_source_rgb(0, 0, 0);
        for(const Marble::Ptr m: _s->marbles()) {
            ctx->arc(m->x() / meter, m->y() / meter, m->r() / meter, 0, 2 * M_PI);
            ctx->close_path();
        }
        ctx->fill();
        img->write_to_png((boost::format("frames/%08d.png") % _i).str());
        _s->scheduleTickIn(1 / 60. * second);
        ++_i;
    }

    void collision(Marble::Ptr m1, Marble::Ptr m2) {
        std::cout << _s->t() << ": collision between " << m1->name() << " and " << m2->name() << std::endl;
    }

    Simulation* _s;
    int _i;
};

int main() {
    std::vector<Marble::Ptr> marbles;
    marbles.push_back(Marble::create("M", 50 * meter, 10 * kilogram, 960 * meter, 540 * meter, 0 * meter_per_second, 0 * meter_per_second));
    boost::random::mt19937 mt(42);
    boost::random::uniform_01<boost::random::mt19937> gen(mt);
    for(int x = 0; x < 931; x += 50) {
        for(int y = 0; y < 511; y += 50) {
            if(x * x + y * y > 70 * 70) {
                marbles.push_back(Marble::create("m", 7 * meter, 1 * kilogram, (960 + x) * meter, (540 + y) * meter, (200 * gen() - 100) * meter_per_second, (200 * gen() - 100) * meter_per_second));
                if(x != 0)
                marbles.push_back(Marble::create("m", 7 * meter, 1 * kilogram, (960 - x) * meter, (540 + y) * meter, (200 * gen() - 100) * meter_per_second, (200 * gen() - 100) * meter_per_second));
                if(y != 0)
                marbles.push_back(Marble::create("m", 7 * meter, 1 * kilogram, (960 + x) * meter, (540 - y) * meter, (200 * gen() - 100) * meter_per_second, (200 * gen() - 100) * meter_per_second));
                if(x != 0 && y != 0)
                marbles.push_back(Marble::create("m", 7 * meter, 1 * kilogram, (960 - x) * meter, (540 - y) * meter, (200 * gen() - 100) * meter_per_second, (200 * gen() - 100) * meter_per_second));
            }
        }
    }
    auto s = Simulation::create(1920 * meter, 1080 * meter, marbles, boost::make_shared<FramesDrawer>());
    s->advanceTo(20 * second);
}
