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

using namespace std;

int main(int argc, char **argv)
{
  int res = 0;
  int width=1920, height=1080;
  std::string codecName = "h264_nvmpi";

  /* Encoder Setting */
  nvmpictx *encoder;
  nvEncParam param = {0};
  param.width             = width;
  param.height            = height;
  param.bitrate           = 4000000;
  param.mode_vbr          = 0;
  param.idr_interval      = 24;
  param.iframe_interval   = 12;
	param.peak_bitrate      = 0;
  param.fps_n             = 60;
  param.fps_d             = 1;
  // 77 : FF_PROFILE_H264_MAIN
  // 66 : FF_PROFILE_H264_BASELINE
  // 100: FF_PROFILE_H264_HIGH
  param.profile           = 77 ;
  param.level             = 10; // default 50
  param.hw_preset_type    = 1;  // ultrafast
  param.capture_num       = 10; // effect?
  param.qmax              = 10;
  param.qmin              = 0;
  param.max_b_frames      = 0;
  param.refs              = 0; // effect?
  param.enableLossless    = false;

  if(codecName == "h264_nvmpi")
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

      //avctx->extradata_size=i;
      //avctx->extradata	= av_mallocz( avctx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE );
      //memcpy( avctx->extradata, packet.payload,avctx->extradata_size);
      //memset( avctx->extradata + avctx->extradata_size, 0, AV_INPUT_BUFFER_PADDING_SIZE );
      break;
    }
    nvmpi_encoder_close(initCtx);
  }

	if(codecName == "h264_nvmpi")
    encoder = nvmpi_create_encoder(nvCodingType::NV_VIDEO_CodingH264, &param);
	else if(codecName == "hevc_nvmpi"){
    encoder = nvmpi_create_encoder(nvCodingType::NV_VIDEO_CodingHEVC, &param);
	}
	if(!encoder)
  {
    printf("encoder failed to create.\n");
    exit(1);
  }

  /* Encode a OpenCV frame */
  AVFrame *encodingFrame;
  cv::Mat origFrame, yuvFrame = cv::Mat::zeros(height*1.5, width, CV_8UC1);
  origFrame = cv::imread("/home/jin/Downloads/video_000001.png");

  encodingFrame         = av_frame_alloc();
  encodingFrame->width  = width;
  encodingFrame->height = height;
  encodingFrame->format = AV_PIX_FMT_YUV420P;
  av_image_fill_arrays(encodingFrame->data, encodingFrame->linesize, NULL,
                       static_cast<AVPixelFormat>(encodingFrame->format),
                       encodingFrame->width, encodingFrame->height, 1);


  cv::cvtColor(origFrame, yuvFrame, cv::COLOR_RGB2YUV_YV12);
  av_image_fill_arrays(encodingFrame->data, encodingFrame->linesize, yuvFrame.data,
                       static_cast<AVPixelFormat>(encodingFrame->format),
                       encodingFrame->width, encodingFrame->height, 1);

  nvFrame frame;
  nvPacket encodedPacket;
  frame.payload[0] = encodingFrame->data[0];
  frame.payload[1] = encodingFrame->data[1];
  frame.payload[2] = encodingFrame->data[2];
	frame.payload_size[0]=encodingFrame->linesize[0]*encodingFrame->height;
	frame.payload_size[1]=encodingFrame->linesize[1]*encodingFrame->height/2;
	frame.payload_size[2]=encodingFrame->linesize[2]*encodingFrame->height/2;
	frame.linesize[0]=encodingFrame->linesize[0];
	frame.linesize[1]=encodingFrame->linesize[1];
	frame.linesize[2]=encodingFrame->linesize[2];
  frame.timestamp=encodingFrame->pts;

  res = nvmpi_encoder_put_frame(encoder, &frame);
  if(res < 0)
  {
    printf("nvmpi_encoder_put_frame error\n");
    exit(1);
  }

  while(1)
  {
    res = nvmpi_encoder_get_packet(encoder, &encodedPacket);
    if(res == 0) break;
  }
  printf("pktSize: %ld\n", encodedPacket.payload_size);

  /* Encoder Close */
  nvmpi_encoder_close(encoder);

  /* Decoder Setting */
  nvmpictx *decoder;
	if(codecName == "h264_nvmpi")
    decoder = nvmpi_create_decoder(nvCodingType::NV_VIDEO_CodingH264, NV_PIX_YUV420);
	else if(codecName == "hevc_nvmpi"){
    decoder = nvmpi_create_decoder(nvCodingType::NV_VIDEO_CodingHEVC, NV_PIX_YUV420);
	}
	if(!decoder)
  {
    printf("decoder failed to create.\n");
    exit(1);
  }

  /* Decode an Encoded Frame */
	AVFrame *decodedFrame = av_frame_alloc();
  decodedFrame->width  = width;
  decodedFrame->height = height;
  decodedFrame->format = AV_PIX_FMT_YUV420P;
  av_image_fill_arrays(decodedFrame->data, decodedFrame->linesize, NULL,
                       static_cast<AVPixelFormat>(decodedFrame->format),
                       decodedFrame->width, decodedFrame->height, 1);

	nvFrame decNvFrame={0};
	nvPacket decPacket;
	int linesize[3];
  cv::Mat finalFrame, yuvFrameDec = cv::Mat::zeros(height*1.5, width, CV_8UC1);

  decPacket.payload      = encodedPacket.payload;
  decPacket.payload_size = encodedPacket.payload_size;
  decPacket.pts          = encodedPacket.pts;

  res = nvmpi_decoder_put_packet(decoder, &encodedPacket);
  res = nvmpi_decoder_put_packet(decoder, &encodedPacket);
  res = nvmpi_decoder_put_packet(decoder, &encodedPacket);
  res = nvmpi_decoder_put_packet(decoder, &encodedPacket);
  res = nvmpi_decoder_put_packet(decoder, &encodedPacket);
  res = nvmpi_decoder_put_packet(decoder, &encodedPacket);
  res = nvmpi_decoder_put_packet(decoder, &encodedPacket);
  res = nvmpi_decoder_put_packet(decoder, &encodedPacket);
  if(res < 0)
  {
    printf("nvmpi_decoder_put_packet error\n");
    exit(1);
  }

  while(1)
  {
    res = nvmpi_decoder_get_frame(decoder, &decNvFrame, true);
    if(res == 0) break;
  }

	linesize[0]=decNvFrame.linesize[0];
	linesize[1]=decNvFrame.linesize[1];
	linesize[2]=decNvFrame.linesize[2];

	av_image_copy_to_buffer(yuvFrameDec.data, yuvFrameDec.total(), decNvFrame.payload,
	    linesize, static_cast<AVPixelFormat>(AV_PIX_FMT_YUV420P), decNvFrame.width, decNvFrame.height, 1);

  cv::cvtColor(yuvFrameDec, finalFrame, cv::COLOR_YUV420p2RGB);
  cv::imshow("test", finalFrame);
  cv::waitKey(0);


  /* Decoder Close */
	nvmpi_decoder_close(decoder);

  return 0;
}

