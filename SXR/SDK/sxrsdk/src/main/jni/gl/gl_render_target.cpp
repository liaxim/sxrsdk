#include "gl_render_target.h"
#include "../objects/textures/render_texture.h"
#include "../engine/renderer/renderer.h"
#include "../util/sxr_log.h"
#include "gl_headers.h"

namespace sxr{

void GLRenderTarget::readRenderResult(uint8_t *readback_buffer, int eye) {
    RenderTexture* renderTexture = getTexture();
    if (nullptr != renderTexture) {
        renderTexture->setLayerIndex(eye);
    }
    glReadPixels(0, 0, mRenderState.viewportWidth, mRenderState.viewportHeight, GL_RGBA, GL_UNSIGNED_BYTE, readback_buffer);
    checkEGLError("glReadPixels");
}

}