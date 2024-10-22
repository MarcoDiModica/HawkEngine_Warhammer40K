#include "App.h"



App::App() {};


bool App::Awake() { return true; }
bool App::Start() { return true; }
bool App::Update() { return true; }
bool App::CleanUP() { return true; }

// Add a new module to handle
void App::AddModule(Module* module, bool activate) { ; }