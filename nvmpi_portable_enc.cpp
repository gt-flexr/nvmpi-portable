#include "nvmpi_portable.h"


nvmpictx* nvpInitEncoder(nvCodingType enc,
                      int width,
                      int height,
                      int bitrate,
                      int vbr,
                      int idrInterval,
                      int iInterval,
                      int fps,
                      int profile,
                      int logLevel,
                      int hwPreset,
                      int capturePkt,
                      int qMin,
                      int qMax,
                      int bFrames,
                      int refs,
                      bool lossless)
{
  nvmpictx *encoder;
  nvEncParam param = {0};
  param.width             = width;
  param.height            = height;
  param.bitrate           = bitrate;
  param.mode_vbr          = vbr;
  param.idr_interval      = idrInterval;
  param.iframe_interval   = iInterval;
  param.fps_n             = fps;
  param.fps_d             = 1;
  // 77 : FF_PROFILE_H264_MAIN
  // 66 : FF_PROFILE_H264_BASELINE
  // 100: FF_PROFILE_H264_HIGH
  param.profile           = profile;
  param.level             = logLevel; // default 50
  param.hw_preset_type    = hwPreset;  // ultrafast
  param.capture_num       = capturePkt; // effect?
  param.qmin              = qMin;
  param.qmax              = qMax;
  param.max_b_frames      = bFrames;
  param.refs              = refs; // effect?
  param.enableLossless    = lossless;

  if(enc == nvCodingType::NV_VIDEO_CodingH264)
  {
    uint8_t *dst[4];
    int linesize[4];
    nvFrame initFrame={0};
    nvPacket initPacket={0};
    int i;
    int ret;
    nvmpictx* initCtx;
    av_image_alloc(dst, linesize, param.width, param.height, AV_PIX_FMT_YUV420P, 1);

    initCtx=nvmpi_create_encoder(NV_VIDEO_CodingH264, &param);
    i=0;
    while(1)
    {
      initFrame.payload[0]=dst[0];
      initFrame.payload[1]=dst[1];
      initFrame.payload[2]=dst[2];
      initFrame.payload_size[0]=linesize[0]*param.height;
      initFrame.payload_size[1]=linesize[1]*param.height/2;
      initFrame.payload_size[2]=linesize[2]*param.height/2;

      nvmpi_encoder_put_frame(initCtx, &initFrame);
      ret = nvmpi_encoder_get_packet(initCtx, &initPacket);
      if(ret<0)
        continue;

      //find idr index 0x0000000165
      while((initPacket.payload[i]!=0||initPacket.payload[i+1]!=0||
             initPacket.payload[i+2]!=0||initPacket.payload[i+3]!=0x01||
             initPacket.payload[i+4]!=0x65))
      {
        i++;
      }

      break;
    }
    nvmpi_encoder_close(initCtx);
  }

	if(enc == nvCodingType::NV_VIDEO_CodingH264)
    encoder = nvmpi_create_encoder(nvCodingType::NV_VIDEO_CodingH264, &param);
	else if(enc == nvCodingType::NV_VIDEO_CodingHEVC){
    encoder = nvmpi_create_encoder(nvCodingType::NV_VIDEO_CodingHEVC, &param);
	}
	if(!encoder)
  {
    fprintf(stderr, "Encoder init failed...\n");
    return NULL;
  }

  return encoder;
}


void nvpCloseEncoder(nvmpictx *encoder)
{
  nvmpi_encoder_close(encoder);
}


nvPacket nvpEncodeYuvFrame(nvmpictx *encoder, cv::Mat yuvFrame, int rgbWidth, int rgbHeight)
{
  AVFrame *encFrame = av_frame_alloc();
  encFrame->width  = rgbWidth;
  encFrame->height = rgbHeight;
  encFrame->format = AV_PIX_FMT_YUV420P;
  av_image_fill_arrays(encFrame->data, encFrame->linesize, yuvFrame.data,
                       static_cast<AVPixelFormat>(encFrame->format),
                       encFrame->width, encFrame->height, 1);

  nvFrame nvEncFrame;
  nvPacket nvEncPkt;
  nvEncFrame.payload[0] = encFrame->data[0];
  nvEncFrame.payload[1] = encFrame->data[1];
  nvEncFrame.payload[2] = encFrame->data[2];
	nvEncFrame.payload_size[0]=encFrame->linesize[0]*encFrame->height;
	nvEncFrame.payload_size[1]=encFrame->linesize[1]*encFrame->height/2;
	nvEncFrame.payload_size[2]=encFrame->linesize[2]*encFrame->height/2;
	nvEncFrame.linesize[0]=encFrame->linesize[0];
	nvEncFrame.linesize[1]=encFrame->linesize[1];
	nvEncFrame.linesize[2]=encFrame->linesize[2];
  nvEncFrame.timestamp=encFrame->pts;

  int res = nvmpi_encoder_put_frame(encoder, &nvEncFrame);
  if(res < 0)
  {
    return {0};
  }

  while(1)
  {
    res = nvmpi_encoder_get_packet(encoder, &nvEncPkt);
    if(res == 0) break;
  }

  av_frame_free(&encFrame);
  return nvEncPkt;
}


nvPacket* nvpEncodeYuvFrameToBuf(nvmpictx *encoder, cv::Mat yuvFrame, int rgbWidth, int rgbHeight)
{
  nvPacket pkt = nvpEncodeYuvFrame(encoder, yuvFrame, rgbWidth, rgbHeight);
  nvPacket *retPkt = new nvPacket;
  *retPkt = pkt;
  retPkt->payload = new uint8_t[pkt.payload_size];
  memcpy(retPkt->payload, pkt.payload, pkt.payload_size);
  return retPkt;
}

