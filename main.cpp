#include <iostream>
#include <cmath>

#include <boost/format.hpp>
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


struct FramesDrawer {
    void draw(const Simulation& s, int i) {
        RefPtr<ImageSurface> img = ImageSurface::create(FORMAT_RGB24, int(s.width()), int(s.height()));
        RefPtr<Context> ctx = Context::create(img);
        ctx->set_source_rgb(.9, .9, .9);
        ctx->paint();
        ctx->set_source_rgb(0, 0, 0);
        for(auto m: s.marbles()) {
            ctx->arc(m->p(s.t()).x, m->p(s.t()).y, m->r(), 0, 2 * M_PI);
            ctx->close_path();
        }
        ctx->fill();
        img->write_to_png((boost::format("frames/%08d.png") % i).str());
    }
};

int main() {
    std::vector<boost::shared_ptr<Marble>> marbles;
    Position pM(320, 240);
    marbles.push_back(boost::make_shared<Marble>("M", 50, 10, pM, Velocity(0, 0)));
    boost::random::mt19937 mt(42);
    boost::random::uniform_01<boost::random::mt19937> gen(mt);
    for(int x = 20; x < 640; x += 50) {
        for(int y = 15; y < 480; y += 50) {
            Position p(x, y);
            if((p - pM).length() > 70) {
                marbles.push_back(boost::make_shared<Marble>("m", 7, 1, p, Velocity((200 * gen() - 100), (200 * gen() - 100))));
            }
        }
    }
    Simulation s(640, 480, marbles);
    FramesDrawer d;
    const int duration = 30;
    std::cout << "Simulating" << std::flush;
    for(int i = 0; i != duration * 25 + 1; ++i) {
        s.runUntil(Date(i / 25.));
        d.draw(s, i);
        if(i % 25 == 0) {
            std::cout << "." << std::flush;
        }
    }
    std::cout << std::endl;
}
