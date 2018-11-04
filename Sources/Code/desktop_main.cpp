#include "Application.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"

int main(int argc, char* argv[]) {
    Application app(new DesktopPlatform(argc, argv));
    return app.run();
}