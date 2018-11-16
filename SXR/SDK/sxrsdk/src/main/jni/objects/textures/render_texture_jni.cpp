/* Copyright 2015 Samsung Electronics Co., LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/***************************************************************************
 * JNI
 ***************************************************************************/

#include <engine/renderer/renderer.h>
#include <engine/renderer/vulkan_renderer.h>
#include "render_texture.h"
#include "gl/gl_render_texture.h"
namespace sxr {

extern "C" {
JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_ctor(JNIEnv *env, jobject obj, jint width,
                                            jint height);
JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_ctorMSAA(JNIEnv *env, jobject obj,
                                                jint width, jint height, jint sample_count,
                                                jint number_views);
JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_ctorWithParameters(JNIEnv *env,
                                                          jobject obj, jint width, jint height,
                                                          jint sample_count,
                                                          jint color_format, jint depth_format,
                                                          jboolean resolve_depth,
                                                          jintArray parameters, jint);
JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_ctorArray(JNIEnv *env,
                                                 jobject obj, jint width, jint height, jint samples,
                                                 jint numLayers);
JNIEXPORT void JNICALL
Java_com_samsungxr_NativeRenderTexture_beginRendering(JNIEnv *env, jobject obj,
                                                      jlong ptr);
JNIEXPORT void JNICALL
Java_com_samsungxr_NativeRenderTexture_endRendering(JNIEnv *env, jobject obj,
                                                    jlong ptr);

JNIEXPORT void JNICALL
Java_com_samsungxr_NativeRenderTexture_bind(JNIEnv *env, jobject obj, jlong ptr);

JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_makeRenderTextureInfo(JNIEnv *, jclass, int fboId, int fboWidth, int fboHeight);
} //extern "C"


JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_ctor(JNIEnv * env, jobject obj,
                                          jint width, jint height)
{
    RenderTexture* tex = Renderer::getInstance()->createRenderTexture(width, height, 1, ColorFormat::COLOR_8888, DepthFormat::DEPTH_24_STENCIL_8, 0, NULL,1);
    return reinterpret_cast<jlong>(tex);
}

JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_ctorMSAA(JNIEnv* env, jobject obj,
                                              jint width, jint height, jint sample_count, jint number_views)
{
    int depth_format = DepthFormat::DEPTH_24_STENCIL_8;

    if(number_views > 1) // multiview doesn't work with stencil attachment
        depth_format = DepthFormat::DEPTH_24;
    RenderTexture* tex = Renderer::getInstance()->createRenderTexture(width, height, sample_count, ColorFormat::COLOR_8888, depth_format , 0, NULL, number_views);
    return reinterpret_cast<jlong>(tex);
}

JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_ctorWithParameters(JNIEnv* env, jobject obj,
                                                        jint width, jint height,
                                                        jint sample_count, jint color_format,
                                                        jint depth_format, jboolean resolve_depth,
                                                        jintArray j_parameters, jint number_views)
{
    jint* parameters = env->GetIntArrayElements(j_parameters, NULL);
    TextureParameters texparams;
    texparams.setMinFilter(parameters[0]);
    texparams.setMagFilter(parameters[1]);
    texparams.setMaxAnisotropy((float) parameters[2]);
    texparams.setWrapU(parameters[3]);
    texparams.setWrapV(parameters[2]);
    Texture* tex = Renderer::getInstance()->createRenderTexture(width, height, sample_count,
                                                                color_format, depth_format, resolve_depth, &texparams, number_views);
    env->ReleaseIntArrayElements(j_parameters, parameters, 0);
    return reinterpret_cast<jlong>(tex);
}

JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_ctorArray(JNIEnv * env,
          jobject obj, jint width, jint height, jint samples, jint numLayers)
{
    RenderTexture* texarray = Renderer::getInstance()->createRenderTexture(width, height, samples, numLayers, 0);
    return reinterpret_cast<jlong>(texarray);
}

JNIEXPORT void JNICALL
Java_com_samsungxr_NativeRenderTexture_beginRendering(JNIEnv * env, jobject obj,
                                                    jlong ptr) {
    RenderTexture *render_texture = reinterpret_cast<RenderTexture*>(ptr);
    render_texture->beginRendering(gRenderer->getInstance());
}

JNIEXPORT void JNICALL
Java_com_samsungxr_NativeRenderTexture_endRendering(JNIEnv * env, jobject obj,
                                                  jlong ptr) {
    RenderTexture *render_texture = reinterpret_cast<RenderTexture*>(ptr);
    render_texture->endRendering(gRenderer->getInstance());
}

JNIEXPORT void JNICALL
Java_com_samsungxr_NativeRenderTexture_bind(JNIEnv * env, jobject obj, jlong ptr)
{
    RenderTexture *render_texture = reinterpret_cast<RenderTexture*>(ptr);
    render_texture->bind();
}

JNIEXPORT jlong JNICALL
Java_com_samsungxr_NativeRenderTexture_makeRenderTextureInfo(JNIEnv *, jclass, int fboId, int fboWidth, int fboHeight) {
    RenderTextureInfo* renderTextureInfo = new RenderTextureInfo;

    renderTextureInfo->fboWidth = fboWidth;
    renderTextureInfo->fboHeight = fboHeight;
    renderTextureInfo->fboId = fboId;
    renderTextureInfo->viewport[2] = fboWidth;
    renderTextureInfo->viewport[3] = fboHeight;

    renderTextureInfo->multisamples = 1;
    renderTextureInfo->texId = 0;
    renderTextureInfo->useMultiview = false;
    renderTextureInfo->layers = 0;
    renderTextureInfo->viewport[0] = 0;
    renderTextureInfo->viewport[1] = 0;

    return reinterpret_cast<jlong>(renderTextureInfo);
}

}
