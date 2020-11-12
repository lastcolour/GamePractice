#include "Game/GameUtils.hpp"
#include "Game/ETGameScore.hpp"

#include <cassert>

namespace GameUtils {

int ConvertToStarsCount(ObjectiveProgress objectiveProgress) {
    int stars = 0;
    switch(objectiveProgress) {
        case ObjectiveProgress::Fail:
            break;
        case ObjectiveProgress::OneStar:
            stars = 1;
            break;
        case ObjectiveProgress::TwoStars:
            stars = 2;
            break;
        case ObjectiveProgress::ThreeStars:
            stars = 3;
            break;
        default:
            assert(false && "Invalid objective progress");
    }
    return stars;
}

} // namespace GameUtils
