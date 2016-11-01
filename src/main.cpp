#include <cstdlib>

#include "engine.hpp"

int main(int argc, char *argv[])
{
    Engine *engine;
    engine = Engine::getInstance();
    
    engine->start();

    return 0;
}
