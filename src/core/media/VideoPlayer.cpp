#include "VideoPlayer.h"
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <iostream>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

namespace
{
    constexpr int kBlurW = 64;
    constexpr int kBlurH = 36;
    constexpr int kScaleFlags = SWS_BILINEAR | SWS_ACCURATE_RND;

    void boxBlurRGB(std::vector<uint8_t> &img, int w, int h, int radius)
    {
        std::vector<uint8_t> tmp(img.size());

        auto pass = [&](const std::vector<uint8_t> &src, std::vector<uint8_t> &dst, bool horizontal)
        {
            for (int y = 0; y < h; ++y)
            {
                for (int x = 0; x < w; ++x)
                {
                    for (int c = 0; c < 3; ++c)
                    {
                        int sum = 0;
                        int count = 0;
                        for (int k = -radius; k <= radius; ++k)
                        {
                            const int xx = horizontal ? x + k : x;
                            const int yy = horizontal ? y : y + k;
                            if (xx < 0 || xx >= w || yy < 0 || yy >= h)
                                continue;
                            sum += src[static_cast<size_t>(yy * w + xx) * 3 + c];
                            ++count;
                        }
                        dst[static_cast<size_t>(y * w + x) * 3 + c] = static_cast<uint8_t>(sum / std::max(count, 1));
                    }
                }
            }
        };

        pass(img, tmp, true);
        pass(tmp, img, false);
    }

    void uploadTexture(GLuint &tex, int &curW, int &curH, int w, int h, const uint8_t *data)
    {
        if (w <= 0 || h <= 0 || data == nullptr)
            return;

        if (tex == 0)
        {
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, tex);
        }

        if (w != curW || h != curH)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            curW = w;
            curH = h;
        }
        else
        {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
    }
}

VideoPlayer::VideoPlayer() = default;

VideoPlayer::~VideoPlayer()
{
    cleanup();
}

bool VideoPlayer::load(const std::string &filePath)
{
    cleanup();

    if (avformat_open_input(&formatContext, filePath.c_str(), nullptr, nullptr) != 0)
    {
        std::cerr << "No se pudo abrir el video: " << filePath << std::endl;
        formatContext = nullptr;
        return false;
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0)
    {
        std::cerr << "No se pudo leer informacion del video" << std::endl;
        cleanup();
        return false;
    }

    videoStreamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videoStreamIndex < 0)
    {
        std::cerr << "No se encontro stream de video" << std::endl;
        cleanup();
        return false;
    }

    AVStream *stream = formatContext->streams[videoStreamIndex];
    AVCodecParameters *codecPar = stream->codecpar;

    const AVCodec *codec = avcodec_find_decoder(codecPar->codec_id);
    if (!codec)
    {
        std::cerr << "No se encontro decoder" << std::endl;
        cleanup();
        return false;
    }

    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext || avcodec_parameters_to_context(codecContext, codecPar) < 0)
    {
        std::cerr << "No se pudo preparar el decoder" << std::endl;
        cleanup();
        return false;
    }

    codecContext->thread_count = 0;
    codecContext->thread_type = FF_THREAD_FRAME | FF_THREAD_SLICE;

    if (avcodec_open2(codecContext, codec, nullptr) < 0)
    {
        std::cerr << "No se pudo abrir decoder" << std::endl;
        cleanup();
        return false;
    }

    frame = av_frame_alloc();
    packet = av_packet_alloc();
    if (!frame || !packet)
    {
        std::cerr << "Error creando estructuras FFmpeg" << std::endl;
        cleanup();
        return false;
    }

    const AVRational rate = av_guess_frame_rate(formatContext, stream, nullptr);
    fps_ = (rate.num > 0 && rate.den > 0) ? av_q2d(rate) : 30.0;
    if (fps_ < 1.0 || fps_ > 120.0)
        fps_ = 30.0;

    running_ = true;
    thread_ = std::thread(&VideoPlayer::decodeLoop, this);
    return true;
}

void VideoPlayer::restartVideo()
{
    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
    avcodec_flush_buffers(codecContext);
}

bool VideoPlayer::decodeNextFrame()
{
    bool restarted = false;

    while (running_)
    {
        int ret = avcodec_receive_frame(codecContext, frame);
        if (ret == 0)
            return true;

        if (ret != AVERROR(EAGAIN))
        {
            if (restarted)
                return false;
            restartVideo();
            restarted = true;
            continue;
        }

        ret = av_read_frame(formatContext, packet);
        if (ret < 0)
        {
            if (restarted)
                return false;
            restartVideo();
            restarted = true;
            continue;
        }

        if (packet->stream_index == videoStreamIndex)
            avcodec_send_packet(codecContext, packet);
        av_packet_unref(packet);
    }
    return false;
}

void VideoPlayer::decodeLoop()
{
    using clock = std::chrono::steady_clock;
    const auto frameDuration =
        std::chrono::duration_cast<clock::duration>(std::chrono::duration<double>(1.0 / fps_));
    auto next = clock::now();

    while (running_)
    {
        if (!decodeNextFrame())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        const int w = frame->width;
        const int h = frame->height;
        const AVPixelFormat srcFormat = static_cast<AVPixelFormat>(frame->format);

        swsContext = sws_getCachedContext(swsContext, w, h, srcFormat, w, h, AV_PIX_FMT_RGB24, kScaleFlags, nullptr, nullptr, nullptr);
        blurSwsContext = sws_getCachedContext(blurSwsContext, w, h, srcFormat, kBlurW, kBlurH, AV_PIX_FMT_RGB24, SWS_AREA, nullptr, nullptr, nullptr);
        if (!swsContext || !blurSwsContext)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        scratch_.resize(static_cast<size_t>(w) * static_cast<size_t>(h) * 3);
        uint8_t *dst[4] = {scratch_.data(), nullptr, nullptr, nullptr};
        int dstStride[4] = {w * 3, 0, 0, 0};
        sws_scale(swsContext, frame->data, frame->linesize, 0, h, dst, dstStride);

        blurScratch_.resize(static_cast<size_t>(kBlurW) * kBlurH * 3);
        uint8_t *blurDst[4] = {blurScratch_.data(), nullptr, nullptr, nullptr};
        int blurStride[4] = {kBlurW * 3, 0, 0, 0};
        sws_scale(blurSwsContext, frame->data, frame->linesize, 0, h, blurDst, blurStride);
        boxBlurRGB(blurScratch_, kBlurW, kBlurH, 2);
        boxBlurRGB(blurScratch_, kBlurW, kBlurH, 2);

        {
            std::lock_guard<std::mutex> lock(mutex_);
            pixels_.swap(scratch_);
            blurPixels_.swap(blurScratch_);
            frameW_ = w;
            frameH_ = h;
            newFrame_ = true;
        }

        next += frameDuration;
        const auto now = clock::now();
        if (next < now)
            next = now;
        std::this_thread::sleep_until(next);
    }
}

void VideoPlayer::update()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!newFrame_)
        return;
    newFrame_ = false;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    uploadTexture(textureID, texW_, texH_, frameW_, frameH_, pixels_.data());
    uploadTexture(blurTextureID, blurTexW_, blurTexH_, kBlurW, kBlurH, blurPixels_.data());
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    ready_ = (textureID != 0 && blurTextureID != 0);
}

VideoPlayer::UVRect VideoPlayer::getCoverUV(float regionWidth, float regionHeight) const
{
    UVRect r;
    if (texW_ > 0 && texH_ > 0 && regionWidth > 0.0f && regionHeight > 0.0f)
    {
        const float videoAspect = static_cast<float>(texW_) / static_cast<float>(texH_);
        const float regionAspect = regionWidth / regionHeight;
        if (regionAspect > videoAspect)
        {
            const float h = videoAspect / regionAspect;
            r.v0 = (1.0f - h) * 0.5f;
            r.v1 = (1.0f + h) * 0.5f;
        }
        else
        {
            const float w = regionAspect / videoAspect;
            r.u0 = (1.0f - w) * 0.5f;
            r.u1 = (1.0f + w) * 0.5f;
        }
    }
    if (flipVertical_)
        std::swap(r.v0, r.v1);
    return r;
}

void VideoPlayer::cleanup()
{
    running_ = false;
    if (thread_.joinable())
        thread_.join();

    if (textureID != 0)
    {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
    if (blurTextureID != 0)
    {
        glDeleteTextures(1, &blurTextureID);
        blurTextureID = 0;
    }

    if (swsContext)
    {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }
    if (blurSwsContext)
    {
        sws_freeContext(blurSwsContext);
        blurSwsContext = nullptr;
    }
    if (frame)
        av_frame_free(&frame);
    if (packet)
        av_packet_free(&packet);
    if (codecContext)
        avcodec_free_context(&codecContext);
    if (formatContext)
        avformat_close_input(&formatContext);

    videoStreamIndex = -1;
    texW_ = texH_ = blurTexW_ = blurTexH_ = 0;
    frameW_ = frameH_ = 0;
    newFrame_ = false;
    ready_ = false;
    pixels_.clear();
    blurPixels_.clear();
}