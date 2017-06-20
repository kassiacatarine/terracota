#include "options.h"

#include <core/environment.h>
#include <util/button.h>
#include "gameflow.h"

Options::Options()
	: Level("options")
{
	GameFlow::get_instance()->set_state(GameState::SETTINGS);

	Environment* env = Environment::get_instance();
    double w  = env->canvas->w();
    double h  = env->canvas->h();
    double bx=263,by=76,space=20;
	

	m_background = env->resources_manager->get_texture("/usr/share/terracota/res/images/titlescreen/background_terracota.jpg");

    Button* m_fullscreen = new Button(this,"fullscreen", "/usr/share/terracota/res/images/buttons/menu_buttom/start_idle.png",
                                        "/usr/share/terracota/res/images/buttons/menu_buttom/start_active.png");

    Button* m_window_mode = new Button(this,"window_mode", "/usr/share/terracota/res/images/buttons/menu_buttom/start_idle.png",
                                        "/usr/share/terracota/res/images/buttons/menu_buttom/start_active.png");
    Button* m_back = new Button(this,"back", "/usr/share/terracota/res/images/buttons/menu_buttom/start_idle.png",
                                        "/usr/share/terracota/res/images/buttons/menu_buttom/start_active.png");

	m_back->add_observer(this);
    m_window_mode->add_observer(this);
    m_fullscreen->add_observer(this);
        
    add_child(m_window_mode);
	add_child(m_fullscreen);
	add_child(m_back);

    m_window_mode->set_position((w-bx)/2,h/4+space*0+by*0);
    m_fullscreen->set_position((w-bx)/2,h/4+space*1+by*1);
    m_back->set_position((w-bx)/2,h/4+space*2+by*2);
}

Options::~Options()
{

}

void
Options::draw_self()
{
	Environment* env = Environment::get_instance();
	env->canvas->clear(Color::WHITE);
	env->canvas->draw(m_background.get());
}

bool
Options::on_message(Object* object, MessageID id, Parameters)
{
	if (id != Button::clickedID)
		return false;
	
	Button* button = dynamic_cast<Button* >(object);

	if( not button)
		return false;
	
	if( button->id() == "back"){
		set_next("title");
	    finish();
	}
	if( button->id() == "fullscreen")
	{
		Environment::get_instance()->video->set_fullscreen(true);	
		draw_self();
	}

	if( button->id() == "window_mode")
	{
		Environment::get_instance()->video->set_fullscreen(false);
		draw_self();
	}

	return true;
}
