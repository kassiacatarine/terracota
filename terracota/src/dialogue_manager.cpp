#include "dialogue_manager.h"
#include "gamecontrol.h"
#include "inti.h"
#include <core/environment.h>
#include <iostream>
using namespace std;

static DialogueManager* instance=nullptr;

DialogueManager* 
DialogueManager::get_instance()
{
	if (not instance)
	{
		instance = new DialogueManager(); 
	}
	return instance;
}

DialogueManager::DialogueManager()
	:m_settings(nullptr),m_map(nullptr)
{
	Environment *env = Environment::get_instance();
  
    m_settings = env->resources_manager->get_settings("res/conf/dialogue.conf");
}

DialogueManager::~DialogueManager()
{
}

void 
DialogueManager::generate_dialogue()
{
	list<string> dialogues;
	string list = m_settings->read<string>("list","dialogues","null");
	if ( list != "null" )	
	{
		dialogues = make_list(list);
    	for( auto d: dialogues){
		    add_dialogue(d)	;
		}
	}
}

void
DialogueManager::add_dialogue(const string& id)
{
	string file = m_settings->read<string>(id,"file","null");
	string pos = m_settings->read<string>(id,"pos","0x0");
	string speaking = m_settings->read<string>(id,"speaking","null");
    double x,y;
    sscanf(pos.c_str(),"%lfx%lf",&x,&y);
	string next = m_settings->read<string>(id,"next","null");
	int time = m_settings->read<int>(id,"time",5000);
	Object* speaker = nullptr;
	if (speaking != "null")
	{
		if (speaking == "inti")
		{
			speaker = (Object*) GameControl::get_instance()->get_inti();
		}
		else
		{
			speaker = (Object*) GameControl::get_instance()->get_killa();
		}
	}
	Dialogue* dialogue = new Dialogue(m_map,id,file,time,next,speaker,x,y);
    m_dialogues[id] = dialogue;
}

list<string> 
DialogueManager::make_list(const string& text)
{
    list<string> objects;
    string name;
    for (unsigned int i=0;i<text.size();i++)
    {
        if ( ',' != text[i]){
            name += text[i];
        }
        else
        {
            objects.push_back(name);
            name = "";
        }
    }
    if (text.size() > 1)
        objects.push_back(name);
    return objects;
}


void 
DialogueManager::add_children()
{
	for (auto x: m_dialogues)
	{
		m_map->add_child(x.second);
	}
}

void 
DialogueManager::remove_children()
{
	for (auto x: m_dialogues)
	{
		m_map->remove_child(x.second);
	}
}

void 
DialogueManager::next_dialogue(const string& id)
{
	if (id != "null")
	{
		m_dialogues[id]->show_quest();
	}
}

void 
DialogueManager::set_map(Map* map)
{
	m_dialogues.clear();
	m_map = map;
	generate_dialogue();
}
