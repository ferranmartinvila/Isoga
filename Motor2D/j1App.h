#ifndef __j1APP_H__
#define __j1APP_H__

#include "p2List.h"
#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"

#include "PugiXml\src\pugixml.hpp"


// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1FileSystem;
class j1Scene;
class j1Map;
class j1Pathfinding;

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(j1Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame(const char* file);
	void SaveGame(const char* file) const;
	void GetSaveGames(p2List<p2SString>& list_to_fill) const;

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Modules
	j1Window*			win;
	j1Input*			input;
	j1Render*			render;
	j1Textures*			tex;
	j1Audio*			audio;
	j1Scene*			scene;
	j1FileSystem*		fs;
	j1Map*				map;
	j1Pathfinding*		pathfinding;

private:

	p2List<j1Module*>	modules;
	int					argc;
	char**				args;

	p2SString			title;
	p2SString			organization;

	mutable bool		want_to_save;
	bool				want_to_load;
	p2SString			load_game;
	mutable p2SString	save_game;

private:

	//Count the time of the last grame
	j1Timer		last_frame_timer;
	uint32		last_frame_time;
	
	//Used to log the times of app process
	j1PerfTimer process_timer;
	
	//Count the seconds for the fps
	j1Timer		fps_timer;
	uint32		current_frames = 0;
	uint32		current_fps;
	float		avg_fps;
	uint		frames_count = 0;

	//Determinate a framerate
	uint32		framerate_cap = 0;


	//Count the delay error in ms
	j1PerfTimer delay_error;

	//Differential time since the last frame
	float				dt;

	//Timer data
	j1Timer timer;

public:

	uint	GetTime()const;
	uint32	GetFPS() const;
	float	GetAvgFPS() const;
	uint	GetFramesCount() const;

};

extern j1App* App; //App external pointer to point all the included modules 

#endif