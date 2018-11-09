#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include "Application.hpp"

class Environment;
class Surface;

Environment* GetEnv();

class Environment {
private:

    friend class Application;
    friend Environment* GetEnv();

private:

    static Environment* ENV;

private:

    static void InitEnv(Application& application);
    static void DeinitEnv();

private:

    Environment(Application& application);
    ~Environment();

public:

    Application& getApp();
    Surface* getSurface();

    void registerSurface(Surface* surf);

private:

    Application& app;
    Surface* surface;
};

#endif /* __ENVIRONMENT_HPP__ */