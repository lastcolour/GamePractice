#ifndef __PROGRESSION_STARS_HPP__
#define __PROGRESSION_STARS_HPP__

class ProgressionStars {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ProgressionStars();
    ~ProgressionStars();

public:

    EntityId fristId;
    EntityId secondId;
    EntityId thirdId;
};

#endif /* __PROGRESSION_STARS_HPP__ */