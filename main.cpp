#include <iostream>
#include <cmath>

#include <boost/format.hpp>
#include <cairomm/cairomm.h>

#include "collide.hpp"

using namespace Cairo;
using namespace collide;
using namespace bu;

void draw(int i, const Marble& m) {
    RefPtr<ImageSurface> img = ImageSurface::create(FORMAT_RGB24, 800, 600);
    RefPtr<Context> ctx = Context::create(img);
    ctx->set_source_rgb(.9, .9, .9);
    ctx->paint();
    ctx->set_source_rgb(0, 0, 0);
    ctx->arc(m.x() / meter, m.y() / meter, 50, 0, 2 * M_PI);
    ctx->fill();
    img->write_to_png((boost::format("frames/%08d.png") % i).str());
}

int main() {
    Marble m(0, 0, 50 * meter, 50 * meter, 7 * meter_per_second, 10 * meter_per_second);
    for(int i = 0; i != 51; ++i) {
        m.advanceTo(i * second);
        draw(i, m);
    }
    m.setSpeed(7 * meter_per_second, -10 * meter_per_second);
    for(int i = 51; i != 101; ++i) {
        m.advanceTo(i * second);
        draw(i, m);
    }
}
