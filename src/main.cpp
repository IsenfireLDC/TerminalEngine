/*
 * TerminalGame.cpp
 *
 *  Created on: Sep 4, 2021
 *      Author: Ben Klemp
 *
 *  Engine for creating game on the windows terminal
 */

//includes
#include "render.hpp"
#include "entity.hpp"
#include "input.hpp"
#include "events.hpp"
#include "log.hpp"

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <cstdio>

#include <string>

#include <sstream>

#include <random>

using LogType = Log::Entry::LogType;

static bool running = true;
static Entity *primary;

static std::string message;
/*
static void aExit(Entity *target, int input) {
	if(input == Input::Key::Escape) running = false;
};*/

static void inputHandler(Event *event) {
	InputEvent *inputEvent = (InputEvent*)event;

	message.append(std::to_string(inputEvent->key)+" ");
};

static void actionHandler(Event *event) {
	ActionEvent *actionEvent = (ActionEvent*)event;

	actionEvent->action(primary, actionEvent->input);
};

static void quitHandler(Event *event) {
	QuitEvent *quitEvent = (QuitEvent*)event;

	printf("Received QuitEvent from %p\n", quitEvent->input);
	quitEvent->input->runThread(false);

	running = false;
};

static int exHandler(Entity *target, EntityAction action) {
	EntityData d = action.sender->getData();
	d.state += 1;
	action.sender->setData(d);

	d = target->getData();
	d.state += 2;
	target->setData(d);

	return 0;
};

std::random_device rd;
std::default_random_engine gen;
std::uniform_int_distribution<int> randMove(0, 3);
int inputs[4] = {Input::Key::W, Input::Key::A, Input::Key::S, Input::Key::D};

void getKBCodes() {
	int kb_code = 0;

	std::cout << "Hit a key" << std::endl;

	while(true) {
		kb_code = Input::getInputKey();

		if(kb_code != 0) std::cout << kb_code << std::endl;

		if(kb_code == 27) break;

		Sleep(100);
	}
};

int main() {
	gen.seed(rd());

	//Create player
	const char nEntType[20] = "Entity";
	EntityType tEntity = EntityType(0, nEntType);
	tEntity.handlers[1] = exHandler;
	const char nPlayer[20] = "Player";
	Entity player = Entity(&tEntity, Entity::origin, nPlayer);
	Model mPlayer = Model('~', 0b1010);
	player.setModel(&mPlayer);

	//Set player as primary
	primary = &player;

	//Create non-player
	const char nNPC[20] = "NPC";
	Entity npc = Entity(&tEntity, {3,4}, nNPC);
	Model mNPC = Model('!', 0b1111100);
	npc.setModel(&mNPC);

	//Create field
	Field field = Field(Coord(25, 20), Coord(2,1));

	//Create player manager
	EntityManager manager = EntityManager(&field);

	//Create window
	const std::vector<IModelable*> *modelable = reinterpret_cast<const std::vector<IModelable*>*>(manager.getEntitiesList());
	Window window = Window(&field, modelable);

	//Create input
	Input input = Input();
	//Action ExitAction = aExit;
	//input.addActionMapping(Input::Key::Escape, ExitAction);

	//Add player to manager
	manager.registerEntity(&player);
	manager.registerEntity(&npc);

	//Render window
	window.render();

	//Test log
	Log testLog{"Test", "./logs/test.log"};
	LogType testLogType = LogType::Debug;
	testLog.log("Testing nested log", LogType::Info, "Main");
	testLog << "Testing log insertion operator for message";
	Log::Entry testLogEntry = Log::makeEntry("BaseEntry", testLogType, "Main");
	testLog << Log::makeEntry(testLogEntry, "Test log insertion operator for Entry");
	Engine::log.log("Initialization complete", LogType::Info, "Main");

	//Print out manager
	std::stringstream sstr{};
	sstr << manager;
	testLog.log(sstr.str(), testLogType, "Manager");

	//Attempt to move player
	Coord testCoord = {2,5};
	manager.moveEntity(&player, testCoord);


	if(manager.getEntityAt(testCoord)) {
		window.setMsg("Moved player succesfully");
	} else {
		window.setMsg("Failed to move player");
	};

	Sleep(1000);

	//Render window
	window.render();

	//Print out list of entities
	std::vector<Entity*> playerList = manager.getEntities();

	testLog.log("Printing entities", testLogType, "Main");
	for(unsigned int i = 0; i < playerList.size(); ++i) {
		sstr.str("\t");
		sstr << *playerList[i];
		testLog.log(sstr.str(), testLogType, "Main");
	};

	//Get inputs
	testLog.log("Spawning thread...", LogType::Info, "Main");
	if(!input.spawnThread()) {
		testLog.log("Failed, exiting", LogType::Fatal, "Main");
		return 1;
	};

	//Setup event handling
	//InputEvent
	InputEvent ieType = InputEvent(Input::Key::Null);
	int typeIEID = Engine::eventBus.registerEventType(&ieType);
	Engine::eventBus.registerEventHandler(typeIEID, &inputHandler);
	//ActionEvent
	ActionEvent aeType = ActionEvent(0, 0);
	int typeAEID = Engine::eventBus.registerEventType(&aeType);
	Engine::eventBus.registerEventHandler(typeAEID, &actionHandler);
	//QuitEvent
	QuitEvent qeType = QuitEvent(0);
	int typeQEID = Engine::eventBus.registerEventType(&qeType);
	Engine::eventBus.registerEventHandler(typeQEID, &quitHandler);

	while(running) {
		//Event* first = Engine::eventBus.getFirstEvent();
		//const char* info = 0;
		//if(first) info = first->getInfo().c_str();

		Engine::eventBus.handleEvents();
		//if(info) window.setMsg(info);
		window.setMsg(message.c_str());

		input.callAction(&npc, inputs[randMove(gen)]);
		window.render();

		Sleep(100);
	};

	return 0;
}
