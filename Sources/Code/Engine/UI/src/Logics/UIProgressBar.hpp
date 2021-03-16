#ifndef __UI_PROGRESS_BAR_HPP__
#define __UI_PROGRESS_BAR_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIProgressBar.hpp"

class UIProgressBar : public EntityLogic,
    public ETNode<ETUIProgressBar> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIProgressBar();
    virtual ~UIProgressBar();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUIProgressBar
    void ET_setProgress(float newProgress) override;
    float ET_getProgress() const override;

private:

    float currProgress;
};

#endif /* __UI_PROGRESS_BAR_HPP__ */