#ifndef __ET_UI_PROGRESS_BAR_HPP__
#define __ET_UI_PROGRESS_BAR_HPP__

struct ETUIProgressBar {
    virtual ~ETUIProgressBar() = default;
    virtual void ET_setProgress(float newProgress) = 0;
    virtual float ET_getProgress() const = 0;
};

#endif /* __ET_UI_PROGRESS_BAR_HPP__ */