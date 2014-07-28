#ifndef collide_hpp
#define collide_hpp

#include <vector>
#include <queue>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/optional.hpp>


namespace collide {

struct Position {
    Position(float x_, float y_);
    float x;
    float y;
};

struct Displacement {
    Displacement(float dx_, float dy_);
    float dx;
    float dy;
    float length2() const;
    float length() const;
};

struct Date {
    Date(float t_);
    float t;
};

struct Duration {
    Duration(float dt_);
    float dt;
};

struct Velocity {
    Velocity(float vx_, float vy_);
    float vx;
    float vy;
};

Displacement operator-(const Position&, const Position&);
Position operator+(const Position&, const Displacement&);
Displacement operator*(float, const Displacement&);
Displacement operator/(const Displacement&, float);
Velocity operator/(const Displacement&, const Duration&);
Velocity operator*(float, const Velocity&);
Velocity operator/(const Velocity&, float);
Velocity operator+(const Velocity&, const Velocity&);
Velocity operator-(const Velocity&, const Velocity&);
Displacement operator*(const Velocity&, const Duration&);
Duration operator-(const Date&, const Date&);
Duration operator*(float, const Duration&);
Duration operator/(const Duration&, float);
Date operator+(const Date&, const Duration&);
bool operator>(const Date&, const Date&);
bool operator<(const Date&, const Date&);

class Marble;

namespace collisions {
    boost::optional<Date> nextCollisionDate(const Date& after, const Marble&, const Marble&);
    boost::optional<Date> collisionDate(const Marble&, const Marble&);
    void performCollision(const Date&, Marble&, Marble&);
}

class Marble {
public:
    Marble(std::string name, float r, float m, Position p, Velocity v);

    std::string name() const;
    float r() const;
    float m() const;
    Position p(Date) const;
    Date t0() const;
    Velocity v() const;

    void setVelocity(Date, Velocity);

private:
    std::string _name;
    float _r;
    float _m;
    Position _p0;
    Date _t0;
    Velocity _v;
};


class Simulation {
public:
    Simulation(float width, float height, const std::vector<boost::shared_ptr<Marble>>&);

    float width() const;
    float height() const;
    const std::vector<boost::shared_ptr<Marble>>& marbles() const;

public:
    void scheduleTickAt(const Date&);
    void runUntil(const Date&);
    Date t() const;

private:
    float _w;
    float _h;
    std::vector<boost::shared_ptr<Marble>> _marbles;
    Date _t;

private:
    class Event {
    public:
        Event(const Date&, std::vector<boost::shared_ptr<Marble>>);
        virtual ~Event() {};

        Date t() const;

        void apply(Simulation&);

    private:
        virtual void doApply(Simulation&) = 0;

    private:
        struct ImpactedMarble {
            ImpactedMarble(boost::shared_ptr<Marble> marble_) : marble(marble_), _t0(marble->t0()) {}
            bool check() const {return marble->t0().t == _t0.t;}
            boost::shared_ptr<Marble> marble;
            Date _t0;
        };
        Date _t;
        std::vector<ImpactedMarble> _marbles;
    };
    struct EventComparer {
        bool operator()(boost::shared_ptr<Event> a, boost::shared_ptr<Event> b) {
            return a->t() > b-> t(); // Comparison reversed to make a min-priority_queue
        }
    };
    std::priority_queue<boost::shared_ptr<Event>, std::vector<boost::shared_ptr<Event>>, EventComparer> _events;

    class MarblesCollision;
    class WallCollision;

    void scheduleInitialEvents();
    void scheduleNextEvents(boost::shared_ptr<Marble>);
    void scheduleNextWallCollision(boost::shared_ptr<Marble>);
};

} // Namespace

#endif // Include guard
