#pragma once

#include <GL/glew.h>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;

class VideoPlayer
{
public:
    struct UVRect
    {
        float u0 = 0.0f, v0 = 0.0f, u1 = 1.0f, v1 = 1.0f;
    };

    VideoPlayer();
    ~VideoPlayer();
    VideoPlayer(const VideoPlayer &) = delete;
    VideoPlayer &operator=(const VideoPlayer &) = delete;

    bool load(const std::string &filePath);
    void update();

    bool isReady() const { return ready_; }

    GLuint getTexture() const { return textureID; }
    GLuint getBlurTexture() const { return blurTextureID; }

    int getWidth() const { return texW_; }
    int getHeight() const { return texH_; }

    UVRect getCoverUV(float regionWidth, float regionHeight) const;

    void setFlipVertical(bool flip) { flipVertical_ = flip; }

private:
    void decodeLoop();
    bool decodeNextFrame();
    void restartVideo();
    void cleanup();

    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;
    AVFrame *frame = nullptr;
    AVPacket *packet = nullptr;
    SwsContext *swsContext = nullptr;
    SwsContext *blurSwsContext = nullptr;
    int videoStreamIndex = -1;
    double fps_ = 30.0;

    std::thread thread_;
    std::mutex mutex_;
    std::atomic<bool> running_{false};
    std::vector<uint8_t> pixels_;
    std::vector<uint8_t> blurPixels_;
    std::vector<uint8_t> scratch_;
    std::vector<uint8_t> blurScratch_;
    int frameW_ = 0, frameH_ = 0;
    bool newFrame_ = false;

    GLuint textureID = 0;
    GLuint blurTextureID = 0;
    int texW_ = 0, texH_ = 0;
    int blurTexW_ = 0, blurTexH_ = 0;
    bool ready_ = false;
    bool flipVertical_ = false;
};