#ifndef __OBJECTIVE_TARGET_HPP__
#define __OBJECTIVE_TARGET_HPP__

class ReflectContext;

class ObjectiveTarget {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ObjectiveTarget();
    ~ObjectiveTarget();

public:

    int minScore;
};

#endif /* __OBJECTIVE_TARGET_HPP__ */