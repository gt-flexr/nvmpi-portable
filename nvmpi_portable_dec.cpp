#include "nvmpi_portable.h"


nvmpictx* nvpInitDecoder(nvCodingType dec)
{
  nvmpictx *decoder;
	if(dec == nvCodingType::NV_VIDEO_CodingH264)
    decoder = nvmpi_create_decoder(nvCodingType::NV_VIDEO_CodingH264, NV_PIX_YUV420);
	else if(dec == nvCodingType::NV_VIDEO_CodingHEVC){
    decoder = nvmpi_create_decoder(nvCodingType::NV_VIDEO_CodingHEVC, NV_PIX_YUV420);
	}
	if(!decoder)
  {
    fprintf(stderr, "Decoder init failed...\n");
    return NULL;
  }

  return decoder;
}


void nvpCloseDecoder(nvmpictx *decoder)
{
  nvmpi_decoder_close(decoder);
}

void nvpPutPktToDecoder(nvmpictx *decoder, nvPacket *encodedPkt)
{
  int res = nvmpi_decoder_put_packet(decoder, encodedPkt);
  if(res < 0)
  {
    fprintf(stderr, "Put packet failed...\n");
  }
}


cv::Mat nvpGetDecodedYuvFrame(nvmpictx *decoder, int rgbWidth, int rgbHeight)
{
	AVFrame *decFrame = av_frame_alloc();
  decFrame->width  = rgbWidth;
  decFrame->height = rgbHeight;
  decFrame->format = AV_PIX_FMT_YUV420P;
  av_image_fill_arrays(decFrame->data, decFrame->linesize, NULL,
                       static_cast<AVPixelFormat>(decFrame->format),
                       decFrame->width, decFrame->height, 1);

	nvFrame nvDecFrame={0};
	int linesize[3];
  cv::Mat yuvFrameDec = cv::Mat::zeros(1, 1, CV_8UC1);

  int res = nvmpi_decoder_get_frame(decoder, &nvDecFrame, false);
  if(res != 0)
  {
    fprintf(stderr, "no frame is retrieved from decoder...\n");
    return yuvFrameDec;
  }

  yuvFrameDec = cv::Mat::zeros(rgbHeight*1.5, rgbWidth, CV_8UC1);
	linesize[0]=nvDecFrame.linesize[0];
	linesize[1]=nvDecFrame.linesize[1];
	linesize[2]=nvDecFrame.linesize[2];

	av_image_copy_to_buffer(yuvFrameDec.data, yuvFrameDec.total(), nvDecFrame.payload,
	    linesize, static_cast<AVPixelFormat>(AV_PIX_FMT_YUV420P), nvDecFrame.width, nvDecFrame.height, 1);

	return yuvFrameDec;
}

