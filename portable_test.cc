#include "opencv2/imgproc.hpp"
#include <bits/stdc++.h>
#include <flexr>
#include <nvmpi_portable.h>

using namespace std;

int main(int argc, char **argv)
{
  double st, et;
  int res = 0;
  int width=1920, height=1080;

  nvmpictx* encoder = nvpInitEncoder(NV_VIDEO_CodingH264, width, height, 4000000, 0, 24, 12, 60, 77, 50, 1, 10, 0, 10, 0, 0, 0);
  nvmpictx* decoder = nvpInitDecoder(NV_VIDEO_CodingH264);

  int dec = 0;
  for(int i = 0; i < 100; i++)
  {
    st = getTsNow();
    cv::Mat origFrame, yuvFrame = cv::Mat::zeros(height*1.5, width, CV_8UC1);
    origFrame = cv::imread("/home/jin/Downloads/video_000001.png");
    cv::cvtColor(origFrame, yuvFrame, cv::COLOR_RGB2YUV_YV12);
    et = getTsNow();
    debug_print("rgb2yuv: %f", et-st);

    st = getTsNow();
    nvPacket pkt = nvpEncodeYuvFrame(encoder, yuvFrame, width, height);
    et = getTsNow();
    debug_print("encode: %f", et-st);

    st = getTsNow();
    nvpPutPktToDecoder(decoder, &pkt);
    cv::Mat decodedYuv = nvpGetDecodedYuvFrame(decoder, width, height);
    et = getTsNow();

    if(decodedYuv.rows != 1 && decodedYuv.cols != 1)
    {
      debug_print("decode: %f, %d:%d", et-st, i, dec++);
      st = getTsNow();
      cv::Mat finalFrame;
      cv::cvtColor(decodedYuv, finalFrame, cv::COLOR_YUV420p2RGB);
      et = getTsNow();
      debug_print("yuv2rgb: %f", et-st);
      cv::imshow("test", finalFrame);
      cv::waitKey(0);
    }
  }

  nvpCloseEncoder(encoder);
  nvpCloseDecoder(decoder);

  /* Decoder Close */
	nvmpi_decoder_close(decoder);

  return 0;
}

