#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
#include <windows.h>

using namespace cv;
using namespace std;

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

Mat draw_histo(Mat hist)
{
	if (hist.dims != 2) {
		cout << "히스토그램이 2차원 데이터가 아님니다." << endl;
		exit(1);
	}
	float ratio_value = 512;
	float ratio_hue = 180.f / hist.rows;
	float ratio_sat = 256.f / hist.cols;

	Mat graph(hist.size(), CV_32FC3);
	for (int i = 0; i < hist.rows; i++) {
		for (int j = 0; j < hist.cols; j++)
		{
			float value = hist.at<float>(i, j) * ratio_value;
			float hue = i * ratio_hue;
			float sat = j * ratio_sat;
			graph.at<Vec3f>(i, j) = Vec3f(hue, sat, value);
		}
	}

	graph.convertTo(graph, CV_8U);;
	cvtColor(graph, graph, COLOR_HSV2BGR);
	resize(graph, graph, Size(0, 0), 10, 10, INTER_NEAREST);

	return graph;
}

vector<Mat> load_histo(vector<char*>& list, Vec3i bins, Vec3f ranges, int nImages, int flag)
{
	vector<Mat> DB_hists;
	for (int i = 0; i < nImages; i++)
	{
		String  fname = format("./data/crawled_images/%d/%s", flag, list[i]);
		Mat hsv, hist, img = imread(fname, IMREAD_COLOR);
		if (img.empty()) continue;

		cvtColor(img, hsv, COLOR_BGR2HSV);			// HSV 컬러 변환
		calc_histo(hsv, hist, bins, ranges, 2);			// 2차원 히스토그램 계산
		DB_hists.push_back(hist);
	}
	cout << format("%d 개의 파일을 로드 및 히스토그램 계산 완료", DB_hists.size()) << endl;
	return DB_hists;
}


Mat calc_similarity(Mat query_hist, vector<Mat>& DB_hists)
{
	Mat DB_similaritys;

	for (int i = 0; i < (int)DB_hists.size(); i++)
	{
		double compare = compareHist(query_hist, DB_hists[i], HISTCMP_CORREL);
		DB_similaritys.push_back(compare);
	}

	return DB_similaritys;
}

void select_view(vector<char*>& list, double sinc, Mat DB_similaritys, Vec3i bins, Vec3f ranges, int flag, int n)
{
	Mat m_idx, sorted_sim;
	sortIdx(DB_similaritys, m_idx, SORT_EVERY_COLUMN + SORT_DESCENDING);
	cv::sort(DB_similaritys, sorted_sim, SORT_EVERY_COLUMN + SORT_DESCENDING);

	for (int i = n + 1; i < (int)sorted_sim.total(); i++)
	{
		double  sim = sorted_sim.at<double>(i);
		if (sim > sinc)
		{
			int idx = m_idx.at<int>(i);
			char fname[256];
			sprintf_s(fname, "./data/crawled_images/%d/%s", flag, list[idx]);
			Mat img = imread(fname, IMREAD_COLOR);
			if (img.empty()) {
				continue;
			}
			String  title = format("%s", list[idx]);
			cout << title << "유사도 = " << sim << endl;
			imshow(title, img);
			moveWindow(title, 400, img.rows);
			int key = waitKey();
			if (key == 0) {
				remove(fname);
				printf("%s is removed\n", fname);
				Sleep(2);
			}
			if (key == 27) {
				destroyWindow(title);
				return;
			}
			destroyWindow(title);
		}
	}
}

void getFileList(char* path, vector<char*>& list) {
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	if ((hFind = FindFirstFile(path, &FindFileData)) != INVALID_HANDLE_VALUE) {
		do {
			char* finename = (char*)malloc(256);
			strncpy_s(finename, 256, FindFileData.cFileName, 256);
			list.push_back(finename);
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	else {
		printf("INVALID_HANDLE_VALUE");
		exit(1);
	}
}

void removeDuplicatedImageFile(vector<char*>& list, Vec3i & bins, Vec3f & ranges, int flag, double  sinc) {

	vector<Mat> DB_hists = load_histo(list, bins, ranges, list.size(), flag);

	Mat info = Mat(210, 400, CV_8UC3, Scalar(255, 255, 255));
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("조작법\n\n Delete  (현재 띄워진 이미지 삭제) \nSpace  (다음 이미지 보기)\nESC: 종료\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

	putText(info, "is Duplicate?", Point(0, 50), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255));
	putText(info, "YES: Delete", Point(0, 110), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));
	putText(info, "NO: Space", Point(0, 140), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));
	putText(info, "QUIT: ESC", Point(0, 170), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));

	imshow("Infomation", info);
	moveWindow("Infomation", 0, 0);

	int i = 0;
	for (const auto& a : list) {
		char img[256] = "./data/crawled_images/";
		char buf[256];
		sprintf_s(buf, "%d/", flag);
		strncat_s(img, buf, 256);
		strncat_s(img, a, 256);
		printf("\n----%s----\n", a);
		Mat query = imread(img, IMREAD_COLOR);
		if (query.empty()) {
			i++;
			continue;
		}
		imshow("원본", query);
		moveWindow("원본", 400, 0);
		
		Mat hsv, query_hist;
		cvtColor(query, hsv, COLOR_BGR2HSV);					// HSV 컬러 변환
		calc_histo(hsv, query_hist, bins, ranges, 2);
		Mat hist_img = draw_histo(query_hist);

		Mat DB_similaritys = calc_similarity(query_hist, DB_hists);
		
		select_view(list, sinc, DB_similaritys, bins, ranges,flag,i);

		i++;
	}
}

int main()
{
	Vec3i bins(30, 42, 0);
	Vec3f ranges(180, 256, 0);
	vector<char*> nonFireFileList;
	vector<char*> fireFileList;
	vector<char*> smokeFileList;
	char nonFireDir[256] = "./data/crawled_images/0/*.jpg";
	char fireDir[256] = "./data/crawled_images/1/*.jpg";
	char smokeDir[256] = "./data/crawled_images/2/*.jpg";

	double  sinc;
	cout << "   기준 유사도(0~1) 입력: ";
	cin >> sinc;
	

	getFileList(nonFireDir, nonFireFileList);
	removeDuplicatedImageFile(nonFireFileList, bins, ranges , 0, sinc);


	getFileList(fireDir, fireFileList);
	removeDuplicatedImageFile(fireFileList, bins, ranges, 1, sinc);

	getFileList(smokeDir, smokeFileList);
	removeDuplicatedImageFile(smokeFileList, bins, ranges, 2, sinc);

	return 0;
}