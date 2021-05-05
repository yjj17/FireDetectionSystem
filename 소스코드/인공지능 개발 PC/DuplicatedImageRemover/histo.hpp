#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
#include <windows.h>

using namespace cv;
using namespace std;

//한 이미지(img)의 히스토그램을 계산하여 hist에 저장
void calc_histo(const Mat& img, Mat& hist, Vec3i bins, Vec3f range, int _dims)
{
	int dims = (_dims <= 0) ? img.channels() : _dims;// 히스토그램 차원수
	int		channels[] = { 0, 1, 2 };
	int		histSize[] = { bins[0], bins[1], bins[2] };

	float  range1[] = { 0, range[0] };
	float  range2[] = { 0, range[1] };
	float  range3[] = { 0, range[2] };
	const float* ranges[] = { range1, range2, range3 };

	calcHist(&img, 1, channels, Mat(), hist, dims, histSize, ranges);
	normalize(hist, hist, 0, 1, NORM_MINMAX);			// 정규화
}

//imgNameList에 있는 모든 이미지의 히스토그램 계산하여 DB_hists에 저장하고 반환
vector<Mat> load_histo(vector<char*>& imgNameList, Vec3i bins, Vec3f ranges, int nImages, int flag)
{
	vector<Mat> DB_hists;
	for (int i = 0; i < nImages; i++)
	{
		String  fname = format("./data/crawled_images/%d/%s", flag, imgNameList[i]);
		Mat hsv, hist, img = imread(fname, IMREAD_COLOR);
		if (img.empty()) continue;

		cvtColor(img, hsv, COLOR_BGR2HSV);			// HSV 컬러 변환
		calc_histo(hsv, hist, bins, ranges, 2);			// 2차원 히스토그램 계산
		DB_hists.push_back(hist);
	}
	cout << format("%d 개의 파일을 로드 및 히스토그램 계산 완료", DB_hists.size()) << endl;
	return DB_hists;
}

