#ifndef __PROFILE_POINT_HPP__
#define __PROFILE_POINT_HPP__

class ProfilePoint {
public:

    ProfilePoint(const char* name);
    ~ProfilePoint();

private:

    ProfilePoint() = delete;
    ProfilePoint(const ProfilePoint&) = delete;
    ProfilePoint& operator=(const ProfilePoint) = delete;
};

#endif /* __PROFILE_POINT_HPP__ */