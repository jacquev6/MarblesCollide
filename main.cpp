#include <iostream>
#include <cmath>

#include <cairomm/cairomm.h>

#include "collide.hpp"

using namespace Cairo;

int main() {
    RefPtr<ImageSurface> img = ImageSurface::create(FORMAT_RGB24, 800, 600);
    RefPtr<Context> ctx = Context::create(img);
    ctx->set_source_rgb(1, 1, 1);
    ctx->paint();
    ctx->set_source_rgb(0, 0, 0);
    ctx->arc(400, 300, 50, 0, 2 * M_PI);
    ctx->fill();
    img->write_to_png("image.png");
}
