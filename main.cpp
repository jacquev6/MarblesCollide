#include <iostream>
#include <cmath>

#include <boost/format.hpp>
#include <cairomm/cairomm.h>

#include "collide.hpp"

#include <boost/assign.hpp>
#include <boost/make_shared.hpp>

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
        RefPtr<ImageSurface> img = ImageSurface::create(FORMAT_RGB24, int(_s->width() / meter), int(_s->height() / meter));
        RefPtr<Context> ctx = Context::create(img);
        ctx->set_source_rgb(.9, .9, .9);
        ctx->paint();
        ctx->set_source_rgb(0, 0, 0);
        for(const Marble::Ptr m: _s->marbles()) {
            ctx->arc(m->x() / meter, m->y() / meter, m->r() / meter, 0, 2 * M_PI);
        }
        ctx->fill();
        img->write_to_png((boost::format("frames/%08d.png") % _i).str());
        _s->scheduleTickIn(1 / 60. * second);
        ++_i;
    }

    Simulation* _s;
    int _i;
};

int main() {
    auto m1 = Marble::create(50 * meter, 0, 50 * meter, 240 * meter, 200 * meter_per_second, 0);
    auto m2 = Marble::create(50 * meter, 0, 590 * meter, 240 * meter, 200 * meter_per_second, 0);
    auto s = Simulation::create(640 * meter, 480 * meter, ba::list_of(m1)(m2), boost::make_shared<FramesDrawer>());
    s->advanceTo(20 * second);
}
