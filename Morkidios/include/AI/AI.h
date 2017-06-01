#ifndef AI_H
#define AI_H

#include "Character.h"
#define SPEED 0.025
#define POSITION_PRECISION 0.1 // Define the place precision for Task
#define VISION_ZONE 100

namespace Morkidios {

	class AI;

	struct _MorkidiosExport AnimationInformations {
		double timeSinceLastFrame;
	};

	struct _MorkidiosExport Task {
		enum Action {
			Chasing,
			Moving,
			None,
		};
		Action action = None;
		Ogre::Vector2 position;
		Character* target;
	};

	struct _MorkidiosExport Memories {
		std::vector<Ogre::Vector2> oldPos;
		std::deque<Task> oldTasks;
	};

	class _MorkidiosExport NavigationAlgorithm {
	public:
		virtual void navigate(AI*){};
		virtual void searchForEnnemy(AI*){}
	};

	class _MorkidiosExport AI : public Character {
	public:
		// AI stuff
		virtual void animation(const AnimationInformations*) = 0;
		virtual void navigation(NavigationAlgorithm*) = 0;
		virtual void interact() = 0;

		// Animation methodes
		virtual void setUpAnimations() = 0;
		virtual void enableAnimation(std::string name, bool b) = 0;
		virtual void setLoopAnimation(std::string name, bool b) = 0;

		// Navigation methodes
		virtual void goTo(Ogre::Vector2 v);
		virtual void stop();

		// Return value methodes
		virtual std::string getName();
		virtual Memories* getMemories();
		virtual std::deque<Task>* getTasks();
		virtual std::vector<Character*> getEnemy();

		// Various methodes
		virtual void setDead();

		// Construction methodes
		virtual void addEnemy(Character*);
	protected:
		Memories* mMemories;
		std::deque<Task> mTasks;

		std::string mName;
		std::vector<Character*> mEnemy;
	};

}

#endif // AI_H
