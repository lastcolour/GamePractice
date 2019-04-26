#include "Application.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"

int main(int argc, char* argv[]) {
    Application app(std::unique_ptr<Platform>(new DesktopPlatform(argc, argv)));
    return app.run();
}