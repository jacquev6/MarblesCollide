#include <iostream>
#include <cmath>

#include <boost/format.hpp>
#include <cairomm/cairomm.h>

#include "collide.hpp"

using namespace Cairo;

int main() {
    for(int i = 0; i != 75; ++i) {
        RefPtr<ImageSurface> img = ImageSurface::create(FORMAT_RGB24, 800, 600);
        RefPtr<Context> ctx = Context::create(img);
        ctx->set_source_rgb(1, 1, 1);
        ctx->paint();
        ctx->set_source_rgb(0, 0, 0);
        ctx->arc(100 + 2 * i, 100 + i, 50, 0, 2 * M_PI);
        ctx->fill();
        img->write_to_png((boost::format("frames/%08d.png") % i).str());
    }
}
