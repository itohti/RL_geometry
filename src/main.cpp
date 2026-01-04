// Ezimet Uyghur and Izdar Tohti

/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

/**
 * `$modify` lets you extend and modify GD's classes.
 * To hook a function in Geode, simply $modify the class
 * and write a new function definition with the signature of
 * the function you want to hook.
 *
 * Here we use the overloaded `$modify` macro to set our own class name,
 * so that we can use it for button callbacks.
 *
 * Notice the header being included, you *must* include the header for
 * the class you are modifying, or you will get a compile error.
 *
 * Another way you could do this is like this:
 *
 * struct MyMenuLayer : Modify<MyMenuLayer, MenuLayer> {};
 */
#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	/**
	 * Typically classes in GD are initialized using the `init` function, (though not always!),
	 * so here we use it to add our own button to the bottom menu.
	 *
	 * Note that for all hooks, your signature has to *match exactly*,
	 * `void init()` would not place a hook!
	*/
	bool init() {
		if (!MenuLayer::init()) 
			return false;

		auto winSize = CCDirector::get()->getWinSize();
		
		auto label = CCLabelBMFont::create("WE ARE UP!", "bigFont.fnt");
		label->setPosition(winSize / 2);
		this->addChild(label);

		return true;
	}

	/**
	 * This is the callback function for the button we created earlier.
	 * The signature for button callbacks must always be the same,
	 * return type `void` and taking a `CCObject*`.
	*/
	void onMyButton(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}
};

#include <Geode/modify/PlayLayer.hpp>
class $modify(MyPlayLayer, PlayLayer) {
	void startGame() {
		PlayLayer::startGame();

		log::info("started a level");
	}

	void postUpdate(float dt) {
		PlayLayer::postUpdate(dt);
		auto* p = this->m_player1;
        if (!p) return;
		
		log::info("{}, {}", 
			p->getRealPosition().x, p->getRealPosition().y);
		
		log::info("current percentage: {}", this->getCurrentPercent());
	}
};