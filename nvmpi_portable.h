#pragma once

#include <bits/stdc++.h>
#include <opencv4/opencv2/opencv.hpp>

extern "C" {
#include <nvmpi.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/common.h>
}


/************** Encoder ************/
nvmpictx* nvpInitEncoder(nvCodingType enc, int width, int height, int bitrate, int vbr, int idrInterval, int iInterval, int fps, int profile, int logLevel, int hwPreset, int capturePkt, int qMin, int qMax, int bFrames, int refs, bool lossless);
void nvpCloseEncoder(nvmpictx *encoder);
nvPacket  nvpEncodeYuvFrame(nvmpictx *encoder, cv::Mat yuvFrame, int rgbWidth, int rgbHeight);
nvPacket* nvpEncodeYuvFrameToBuf(nvmpictx *encoder, cv::Mat yuvFrame, int rgbWidth, int rgbHeight);


/************** Decoder ************/
nvmpictx* nvpInitDecoder(nvCodingType dec);
void nvpCloseDecoder(nvmpictx *decoder);
void nvpPutPktToDecoder(nvmpictx *decoder, nvPacket *encodedPkt);
cv::Mat nvpGetDecodedYuvFrame(nvmpictx *decoder, int rgbWidth, int rgbHeight);

