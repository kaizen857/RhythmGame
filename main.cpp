#include "includes/Game.hpp"

int main(int, char *[])
{
    Game game;
    game.Init("./res/test.dat");
    game.ShowMenu();
    SDL_Event Event;
    bool Quit = false;
    while (!Quit)
    {
        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_QUIT)
            {
                Quit = true;
            }
            else if (Event.type == SDL_KEYDOWN)
            {
                game.Run();
                return 0;
            }
        }
    }
    return 0;
}
