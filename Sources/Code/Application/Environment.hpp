#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

class Platform;
class ETSystem;

class Environment {
private:

    friend class Application;
    friend Environment* GetEnv();

private:

    static Environment* ENV;

private:

    static bool InitEnv(Application& application);
    static void DeinitEnv();

private:

    Environment(Application& application);
    ~Environment();

public:

    ETSystem* getETSystem();
    Platform* getPlatform();

private:

    Application* app;
};

Environment* GetEnv();

#endif /* __ENVIRONMENT_HPP__ */