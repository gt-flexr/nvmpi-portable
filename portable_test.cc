#include <bits/stdc++.h>
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
    cv::Mat origFrame, yuvFrame = cv::Mat::zeros(height*1.5, width, CV_8UC1);
    origFrame = cv::imread("/home/jin/Downloads/video_000001.png");
    cv::cvtColor(origFrame, yuvFrame, cv::COLOR_RGB2YUV_YV12);

    nvPacket pkt = nvpEncodeYuvFrame(encoder, yuvFrame, width, height);

    nvpPutPktToDecoder(decoder, &pkt);
    cv::Mat decodedYuv = nvpGetDecodedYuvFrame(decoder, width, height);

    if(decodedYuv.rows != 1 && decodedYuv.cols != 1)
    {
      printf("decode: %d:%d\n", i, dec++);
      cv::Mat finalFrame;
      cv::cvtColor(decodedYuv, finalFrame, cv::COLOR_YUV420p2RGB);
      cv::imshow("test", finalFrame);
      cv::waitKey(0);
    }

    delete pkt.payload;
  }

  nvpCloseEncoder(encoder);
  nvpCloseDecoder(decoder);

  /* Decoder Close */
	nvmpi_decoder_close(decoder);

  return 0;
}

