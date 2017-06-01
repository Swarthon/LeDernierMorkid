#ifndef ORC_H
#define ORC_H

// Morkidios includes
#include <Morkidios.h>

struct MazeNavigationMemories : Morkidios::Memories {
public:
};

class Orc : public Morkidios::AI {
public:
	// Construction methodes
	Orc();
	virtual ~Orc();
	void init(Ogre::SceneManager* smgr, btDynamicsWorld* world);

	// Various methodes
	virtual void synchronize();

	// AI stuff
	virtual void animation(const Morkidios::AnimationInformations*);
	virtual void navigation(Morkidios::NavigationAlgorithm*);
	virtual void interact();

	// Animation methodes
	virtual void setUpAnimations();
	virtual void enableAnimation(std::string name, bool b);
	virtual void setLoopAnimation(std::string name, bool b);
private:
};

#endif // ORC_H
