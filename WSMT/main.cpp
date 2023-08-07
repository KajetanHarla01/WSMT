#include <iostream>
#include <fstream>
#include "cYUV.h"
#include <vector>
#include <algorithm>
#include <unordered_set>
using namespace std;

int main()
{
#pragma region Variables
	cYUV<UChar> out(1920, 1080, 8, 420);
	cYUV<UChar> yuv(1920, 1080, 8, 420);
	cYUV<UShort> depth(1920, 1080, 16, 400);
	cYUV<UShort> outDepth(1920, 1080, 16, 400);
	vector<vector<int>> outY;
	vector<vector<int>> outU;
	vector<vector<int>> outV;
	unordered_set<int> correctPixels;
	vector<int> minD;
	bool repeatPixelsFill = true;
	bool getAllPixels = false;
#pragma endregion

#pragma region Params
	int _frames = 1500;
	int _frameDensity = 20;
	int _minPixelsCount = 50;
	int _startLatitude = 100;
	int _deltaLatitude = 100;
	int _maxLatitude = 500;
#pragma endregion

	for (int frame = 0; frame < _frames; frame += _frameDensity) {
		depth.frameReader("DepthMaps/_gopro_hala_depth_cam22_1920x1080_cf400_16bps.yuvbf.yuv", frame);
		for (int h = 0; h < 1080; h++) {
			for (int w = 0; w < 1920; w++) {
				int luma = h * 1920 + w;
				if (frame == 0) {
					minD.push_back(depth.m_atY[luma]);
				}
				else {
					if (depth.m_atY[luma] < minD.at(luma)) {
						minD.at(luma) = depth.m_atY[luma];
					}
				}
			}
		}
	}
	for (int h = 0; h < 1080; h++) {
		for (int w = 0; w < 1920; w++) {
			int luma = h * 1920 + w;
			outDepth.m_atY[luma] = minD.at(luma);
		}
	}
	//outDepth.frameWriter("test_1920x1080_cf400_16bps.yuv", 0);
	while (repeatPixelsFill) {
		repeatPixelsFill = false;
		for (int frame = 0; frame < _frames; frame += _frameDensity) {
			yuv.frameReader("Cams/basketball_cam22_1920x1080.yuvdist.yuv_600.yuv", frame);
			depth.frameReader("DepthMaps/_gopro_hala_depth_cam22_1920x1080_cf400_16bps.yuvbf.yuv", frame);
			for (int h = 0; h < 1080; h++) {
				for (int w = 0; w < 1920; w++) {
					int luma = h * 1920 + w;
					if (correctPixels.find(luma) == correctPixels.end()) {
						if (frame == 0) {
							vector<int> tmpY;
							outY.push_back(tmpY);
							if (h % 2 == 0 && w % 2 == 0) {
								vector<int> tmpU;
								vector<int> tmpV;
								outU.push_back(tmpU);
								outV.push_back(tmpV);
							}
						}
						if (getAllPixels || depth.m_atY[luma] <= _startLatitude + minD.at(luma)) {
							outY.at(luma).push_back(yuv.m_atY[luma]);
							if (h % 2 == 0 && w % 2 == 0) {
								int chroma = h / 2 * 960 + w / 2;
								outU.at(chroma).push_back(yuv.m_atU[chroma]);
								outV.at(chroma).push_back(yuv.m_atV[chroma]);
							}
						}
					}
				}
			}
		}
		for (int h = 0; h < 1080; h++) {
			for (int w = 0; w < 1920; w++) {
				int luma = h * 1920 + w;
				if (correctPixels.find(luma) == correctPixels.end()) {
					if (outY.at(luma).size() >= _minPixelsCount) {
						correctPixels.insert(luma);
						//cout << correctPixels.size() << endl;
					}
					else {
						repeatPixelsFill = true;
						outY.at(luma).clear();
						if (h % 2 == 0 && w % 2 == 0) {
							int chroma = h / 2 * 960 + w / 2;
							outU.at(chroma).clear();
							outV.at(chroma).clear();
						}
					}
				}
			}
		}
		_startLatitude += _deltaLatitude;
		if (_startLatitude >= _maxLatitude) {
			getAllPixels = true;
		}
		std::cout << _startLatitude << " " << correctPixels.size() << endl;
	}
	for (int h = 0; h < 1080; h++) {
		for (int w = 0; w < 1920; w++) {
			int luma = h * 1920 + w;
			sort(outY.at(luma).begin(), outY.at(luma).end());
			int medianY = outY.at(luma).size() % 2 == 1 ? outY.at(luma).at((int)outY.at(luma).size() / 2) : (outY.at(luma).at((int)outY.at(luma).size() / 2) + outY.at(luma).at((int)(outY.at(luma).size() - 1) / 2)) / 2;
			out.m_atY[luma] = medianY;
			//out.m_atY[luma] = outY.at(luma).size();
			if (h % 2 == 0 && w % 2 == 0) {
				int chroma = h / 2 * 960 + w / 2;
				sort(outU.at(chroma).begin(), outU.at(chroma).end());
				sort(outV.at(chroma).begin(), outV.at(chroma).end());
				int medianU = outU.at(chroma).size() % 2 == 1 ? outU.at(chroma).at((int)outU.at(chroma).size() / 2) : (outU.at(chroma).at((int)outU.at(chroma).size() / 2) + outU.at(chroma).at((int)(outU.at(chroma).size() - 1) / 2)) / 2;
				int medianV = outV.at(chroma).size() % 2 == 1 ? outV.at(chroma).at((int)outV.at(chroma).size() / 2) : (outV.at(chroma).at((int)outV.at(chroma).size() / 2) + outV.at(chroma).at((int)(outV.at(chroma).size() - 1) / 2)) / 2;
				out.m_atU[chroma] = medianU;
				out.m_atV[chroma] = medianV;
			}
		}
	}
	out.frameWriter("Cams/cam22_background_depthFixed_1920x1080.yuv", 0);
	return 0;
}
