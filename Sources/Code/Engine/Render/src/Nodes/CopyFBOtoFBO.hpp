#ifndef __COPY_FBO_TO_FBO_HPP__
#define __COPY_FBO_TO_FBO_HPP__

class RenderFramebuffer;
class RenderShader;
class RenderGeometry;

class CopyFBOtoFBO {
public:

    CopyFBOtoFBO();
    ~CopyFBOtoFBO();

    bool init();
    bool copy(RenderFramebuffer& from, RenderFramebuffer& to);
    bool copyToDefault(RenderFramebuffer& from);

private:

    bool canCopy() const;

private:

    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __COPY_FBO_TO_FBO_HPP__ */