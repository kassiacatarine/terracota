#include "killa.h"
#include <core/environment.h>
#include <core/keyboardevent.h>
#include <core/joystickevent.h>
#include <iostream>
#include "life.h"

using namespace std;

using std::make_pair;
class Killa::Impl
{
public:
    Impl(Killa* killa)
        : m_killa(killa), m_direction(Killa::LEFT), m_moviment(make_pair(0.0, 0.0)),m_life(new Life())
    {
    }

    Direction direction() const { return m_direction; }
    void set_direction(Direction direction) { m_direction = direction; }

    const pair<double, double>& moviment() const { return m_moviment; }

    void set_moviment(double xaxis, double yaxis)
    {
        m_moviment = make_pair(xaxis, yaxis);
    }
    Life* life()
    {
        return m_life;
    }
	bool can_change()
	{
		return true;
	}

private:
    Killa* m_killa;
    Direction m_direction;
    pair<double, double> m_moviment;
    Life* m_life;
};

class Idle : public SpriteState
{
public:
    Idle(Killa *killa)
        : m_killa(killa), m_animation(new Animation("/usr/share/terracota/res/images/characters/killa/idle.png",
            0, 0, 120, 206, 24, 45, true)), m_left(0), m_right(0), m_up(0), m_down(0), m_attack(0), m_interacting(0)
    {
        m_killa->set_dimensions(m_animation->w(), m_animation->h());
    }

    ~Idle() {}

    void enter(int)
    {
        m_right = m_left = m_up = m_down = m_attack = m_interacting = 0;
        m_animation->reset();
    }

    void leave(int)
    {
    }
    void draw()
    {
        m_animation->draw(m_killa->x(), m_killa->y());
    }

    void update(unsigned long elapsed)
    {
        short xres = m_right - m_left;
        short yres = m_down - m_up;

        if (xres < 0)
        {
            m_killa->set_moviment(-1.0, 0.0);
            m_killa->set_direction(Killa::LEFT);
            m_killa->report_event(Killa::MOVED);
        }
        else if (xres > 0)
        {
            m_killa->set_moviment(1.0, 0.0);
            m_killa->set_direction(Killa::RIGHT);
            m_killa->report_event(Killa::MOVED);
        }

        if (yres > 0)
        {
            m_killa->set_moviment(0.0, -1.0);
            m_killa->set_direction(Killa::DOWN);
            m_killa->report_event(Killa::MOVED);
        }

        else if (yres < 0)
        {
            m_killa->set_moviment(0.0, 1.0);
            m_killa->set_direction(Killa::UP);
            m_killa->report_event(Killa::MOVED);
        }
        if ( m_attack > 0)
        {
            m_killa->report_event(Killa::ATTACKED);
        }


        if ( m_interacting > 0)
        {
            m_killa->report_event(Killa::INTERACTED);
        }

        Killa::Direction dir = m_killa->direction();
        int row = dir == Killa::LEFT ? 1 : 0;
        m_animation->set_row(row);
        m_animation->update(elapsed);
    }

    bool on_event(const KeyboardEvent& event)
    {
        switch (event.state())
        {
        case KeyboardEvent::PRESSED:
            switch (event.key())
            {
            case KeyboardEvent::LEFT:
            case KeyboardEvent::A:
                m_left = 1;
                return true;
            case KeyboardEvent::UP:
            case KeyboardEvent::W:
                m_up = 1;
                return true;
            case KeyboardEvent::RIGHT:
            case KeyboardEvent::D:
                m_right = 1;
                return true;
            case KeyboardEvent::DOWN:
            case KeyboardEvent::S:
                m_down = 1;
                return true;
            case KeyboardEvent::SPACE:
                m_attack = 1;
                return true;
            case KeyboardEvent::E:
            case KeyboardEvent::N:
                m_interacting = 1;
                return true;
            default:
                break;
            }
            break;

        case KeyboardEvent::RELEASED:
            switch (event.key())
            {
            case KeyboardEvent::LEFT:
            case KeyboardEvent::A:
                m_left = 0;
                return true;
            case KeyboardEvent::UP:
            case KeyboardEvent::W:
                m_up = 0;
                return true;
            case KeyboardEvent::RIGHT:
            case KeyboardEvent::D:
                m_right = 0;
                return true;
            case KeyboardEvent::DOWN:
            case KeyboardEvent::S:
                m_down = 0;
                return true;
            case KeyboardEvent::SPACE:
                m_attack = 0;
                return true;
            case KeyboardEvent::E:
            case KeyboardEvent::N:
                m_interacting = 0;
            default:
                break;
            }
            break;
        }

        return false;
    }

    bool on_event(const JoyStickEvent& event)
    {
        switch (event.state())
        {
        case JoyStickEvent::PRESSED:
            switch (event.button())
            {
            case JoyStickEvent::UP:
                m_up = 1;
                return true;
            case JoyStickEvent::LEFT:
                m_left = 1;
                return true;
            case JoyStickEvent::RIGHT:
                m_right = 1;
                return true;
            case JoyStickEvent::DOWN:
                m_down = 1;
                return true;
            case JoyStickEvent::SQUARE:
                m_attack = 1;
                return true;
            case JoyStickEvent::X:
                m_interacting = 1;
            default:
                break;
            }
            break;

        case KeyboardEvent::RELEASED:
            switch (event.button())
            {
            case JoyStickEvent::UP:
                m_up = 0;
                return true;
            case JoyStickEvent::LEFT:
                m_left = 0;
                return true;
            case JoyStickEvent::RIGHT:
                m_right = 0;
                return true;
            case JoyStickEvent::DOWN:
                m_down = 0;
                return true;
            case JoyStickEvent::SQUARE:
                m_attack = 0;
                return true;
            case JoyStickEvent::X:
                m_interacting = 0;
            default:
                break;
            }
            break;
        }

        return false;
    }
	bool can_change()
	{
		return true;
	}

private:
    Killa *m_killa;

    unique_ptr<Animation> m_animation;
    int m_left, m_right, m_up, m_down;
    int m_attack,m_interacting;
};

class Attacking: public SpriteState
{
public:
    Attacking(Killa *killa)
        : m_killa(killa), m_animation(
              new Animation("/usr/share/terracota/res/images/characters/killa/attack.png", 0, 0, 207, 260, 12, 50, true)),
          m_left(0), m_right(0), m_down(0), m_up(0), m_attack(0), m_last(0)
    {
    }

    ~Attacking() {}

    void enter(int from)
    {
        Killa::Direction dir = m_killa->direction();

        m_right = dir == Killa::RIGHT ? 1 : 0;
        m_left = dir == Killa::LEFT ? 1 : 0;
        m_up = dir == Killa::UP ? 1 : 0;
        m_down = dir == Killa::DOWN ? 1 : 0;
        m_attack = 1;
        m_last = 0;
        m_animation->reset();
    }

    void leave(int)
    {
    }

    void draw()
    {
        Killa::Direction dir = m_killa->direction();
        if (dir == Killa::LEFT)
            m_animation->draw(m_killa->x()-70, m_killa->y()-65);
        else
            m_animation->draw(m_killa->x()-10, m_killa->y()-65);
    }

    bool on_event(const KeyboardEvent& event)
    {
        switch (event.state())
        {
        case KeyboardEvent::PRESSED:
            switch (event.key())
            {
            case KeyboardEvent::SPACE:
                m_attack = 1;
                return true;
            default:
                break;
            }
            break;

        case KeyboardEvent::RELEASED:
            switch (event.key())
            {
            case KeyboardEvent::SPACE:
                m_attack = 0;
                return true;
            default:
                break;
            }
            break;
        }

        return false;
    }

    bool on_event(const JoyStickEvent& event)
    {
        switch (event.state())
        {
        case JoyStickEvent::PRESSED:
            switch (event.button())
            {
            case JoyStickEvent::SQUARE:
                m_attack = 1;
                return true;
            default:
                break;
            }
            break;

        case JoyStickEvent::RELEASED:
            switch (event.button())
            {
            case JoyStickEvent::SQUARE:
                m_attack = 0;
                return true;
            default:
                break;
            }
            break;
        }

        return false;
    }

    void update(unsigned long elapsed)
    {
        if (not m_attack )
        {
            m_killa->report_event(Killa::STOPPED);
        }
        Killa::Direction dir = m_killa->direction();
        int row = dir == Killa::LEFT ? 1 : 0;
        m_animation->set_row(row);
        m_animation->update(elapsed);
    }
	bool can_change()
	{
		return false;
	}

private:
    Killa *m_killa;
    unique_ptr<Animation> m_animation;
    short m_left, m_right, m_down, m_up, m_attack;
    unsigned long m_last;
};




class Interacting: public SpriteState
{
public:
    Interacting(Killa *killa)
        : m_killa(killa), m_animation(
              new Animation("/usr/share/terracota/res/images/characters/killa/interact.png", 0, 0, 128, 177, 18, 50, true)),
          m_left(0), m_right(0), m_down(0), m_up(0), m_interacting(0), m_last(0)
    {
    }

    ~Interacting() {}

    void enter(int from)
    {
        Killa::Direction dir = m_killa->direction();

        m_right = dir == Killa::RIGHT ? 1 : 0;
        m_left = dir == Killa::LEFT ? 1 : 0;
        m_up = dir == Killa::UP ? 1 : 0;
        m_down = dir == Killa::DOWN ? 1 : 0;
        m_interacting = 1;
        m_last = 0;
        m_animation->reset();
    }

    void leave(int)
    {
    }

    void draw()
    {
        m_animation->draw(m_killa->x(), m_killa->y());
    }

    bool on_event(const KeyboardEvent& event)
    {
        switch (event.state())
        {
        case KeyboardEvent::PRESSED:
            switch (event.key())
            {
            case KeyboardEvent::E:
            case KeyboardEvent::N:
                m_interacting = 1;
                return true;
            default:
                break;
            }
            break;

        case KeyboardEvent::RELEASED:
            switch (event.key())
            {
            case KeyboardEvent::E:
            case KeyboardEvent::N:
                m_interacting = 0;
                return true;
            default:
                break;
            }
            break;
        }

        return false;
    }

    bool on_event(const JoyStickEvent& event)
    {
        switch (event.state())
        {
        case JoyStickEvent::PRESSED:
            switch (event.button())
            {
            case JoyStickEvent::X:
                m_interacting = 1;
                return true;
            default:
                break;
            }
            break;

        case JoyStickEvent::RELEASED:
            switch (event.button())
            {
            case JoyStickEvent::X:
                m_interacting = 0;
                return true;
            default:
                break;
            }
            break;
        }

        return false;
    }

    void update(unsigned long elapsed)
    {
        if (not m_interacting )
        {
            m_killa->report_event(Killa::STOPPED);
        }
        Killa::Direction dir = m_killa->direction();
        int row = dir == Killa::LEFT ? 1 : 0;
        m_animation->set_row(row);
        m_animation->update(elapsed);
    }

	bool can_change()
	{
		return false;
	}
private:
    Killa *m_killa;
    unique_ptr<Animation> m_animation;
    short m_left, m_right, m_down, m_up, m_interacting;
    unsigned long m_last;
};




class Walking : public SpriteState
{
public:
    Walking(Killa *killa)
        : m_killa(killa), m_animation(
              new Animation("/usr/share/terracota/res/images/characters/killa/walking.png", 0, 0, 114, 208, 24, 50, true)),
          m_left(0), m_right(0), m_down(0), m_up(0), m_last(0)
    {
    }

    ~Walking() {}

    const double speed = 160.0;

    void enter(int from)
    {
        Killa::Direction dir = m_killa->direction();

        m_right = dir == Killa::RIGHT ? 1 : 0;
        m_left = dir == Killa::LEFT ? 1 : 0;
        m_up = dir == Killa::UP ? 1 : 0;
        m_down = dir == Killa::DOWN ? 1 : 0;
        m_last = 0;
        m_animation->reset();

        if (from == Killa::IDLE)
        {
            auto moviment = m_killa->moviment();
            double x = moviment.first * speed;
            double y = moviment.second * speed;
            m_killa->set_moviment(x, y);
        }
    }

    void leave(int)
    {
    }

    void draw()
    {
        m_animation->draw(m_killa->x(), m_killa->y());
    }

    bool on_event(const KeyboardEvent& event)
    {
        switch (event.state())
        {
        case KeyboardEvent::PRESSED:
            switch (event.key())
            {
            case KeyboardEvent::LEFT:
            case KeyboardEvent::A:
                m_left = 1;
                return true;
            case KeyboardEvent::UP:
            case KeyboardEvent::W:
                m_up = 1;
                return true;
            case KeyboardEvent::RIGHT:
            case KeyboardEvent::D:
                m_right = 1;
                return true;
            case KeyboardEvent::DOWN:
            case KeyboardEvent::S:
                m_down = 1;
                return true;
            default:
                break;
            }
            break;

        case KeyboardEvent::RELEASED:
            switch (event.key())
            {
            case KeyboardEvent::LEFT:
            case KeyboardEvent::A:
                m_left = 0;
                return true;
            case KeyboardEvent::UP:
            case KeyboardEvent::W:
                m_up = 0;
                return true;
            case KeyboardEvent::RIGHT:
            case KeyboardEvent::D:
                m_right = 0;
                return true;
            case KeyboardEvent::DOWN:
            case KeyboardEvent::S:
                m_down = 0;
                return true;
            default:
                break;
            }
            break;
        }

        return false;
    }

    bool on_event(const JoyStickEvent& event)
    {
        switch (event.state())
        {
        case JoyStickEvent::PRESSED:
            switch (event.button())
            {
            case JoyStickEvent::UP:
                m_up = 1;
                return true;
            case JoyStickEvent::LEFT:
                m_left = 1;
                return true;
            case JoyStickEvent::RIGHT:
                m_right = 1;
                return true;
            case JoyStickEvent::DOWN:
                m_down = 1;
                return true;
            default:
                break;
            }
            break;

        case KeyboardEvent::RELEASED:
            switch (event.button())
            {
            case JoyStickEvent::UP:
                m_up = 0;
                return true;
            case JoyStickEvent::LEFT:
                m_left = 0;
                return true;
            case JoyStickEvent::RIGHT:
                m_right = 0;
                return true;
            case JoyStickEvent::DOWN:
                m_down = 0;
                return true;
            default:
                break;
            }
            break;
        }

        return false;
    }

    void update(unsigned long elapsed)
    {
        short xres = m_right - m_left;
        short yres = m_down -  m_up ;

        if (xres < 0)
        {
            m_killa->set_direction(Killa::LEFT);
        } else if (xres > 0)
        {
            m_killa->set_direction(Killa::RIGHT);
        }


        m_killa->set_moviment(xres * speed, yres * speed);

        if (xres == 0 and  yres == 0)
        {
            m_killa->report_event(Killa::STOPPED);
        }

        Killa::Direction dir = m_killa->direction();
        int row = dir == Killa::LEFT ? 0 : 1;
        m_animation->set_row(row);

        if (not m_last)
        {
            m_last = elapsed;
        }

        auto moviment = m_killa->moviment();
        unsigned long delta = elapsed - m_last;
        double x = m_killa->x() + (moviment.first * delta) / 1000.0;
        double y = m_killa->y() + (moviment.second * delta) / 1000.0;
        m_killa->set_x(x);
        m_killa->set_y(y);

        m_last = elapsed;
        m_animation->update(elapsed);
    }
	bool can_change()
	{
		return false;
	}

private:
    Killa *m_killa;
    unique_ptr<Animation> m_animation;
    short m_left, m_right, m_down, m_up;
    unsigned long m_last;
};

Killa::Killa(Object *parent, const string& id)
    : Sprite(parent, id), m_impl(new Killa::Impl(this))
{
    add_state(IDLE, new Idle(this));
    add_state(WALKING, new Walking(this));
//    add_state(ATTACKING, new Attacking(this));
//    add_state(INTERACTING, new Interacting(this));

    /* to action*/
    add_transition(MOVED, IDLE, WALKING);
//    add_transition(ATTACKED, IDLE, ATTACKING);
//    add_transition(INTERACTED, IDLE, INTERACTING);

    /* to stop*/
    add_transition(STOPPED, WALKING, IDLE);
//    add_transition(STOPPED, ATTACKING, IDLE);
//    add_transition(STOPPED, INTERACTED, IDLE);

    change_state(IDLE, NONE);

    Environment *env = Environment::get_instance();
    env->events_manager->register_listener(this);
}

Killa::~Killa()
{
    Environment *env = Environment::get_instance();
    env->events_manager->unregister_listener(this);
}

Killa::Direction
Killa::direction() const
{
    return m_impl->direction();
}

void
Killa::set_direction(Direction direction)
{
    m_impl->set_direction(direction);
}

const pair<double, double>&
Killa::moviment() const
{
    return m_impl->moviment();
}

void
Killa::set_moviment(double xaxis, double yaxis)
{
    m_impl->set_moviment(xaxis, yaxis);
}

Life*
Killa::life()
{
    return m_impl->life();
}

bool 
Killa::can_change()
{
	return m_impl->can_change();
}


