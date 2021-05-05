#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <windows.h>

using namespace cv;
using namespace std;

#define FOLDER "./data/crawled_images/"

//imgPath 폴더에 존재하는 모든 이미지(.jpg) 파일 이름을 imgNameList에 저장
void getFileList(char* imgPath, vector<char*>& imgNameList) {
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	if ((hFind = FindFirstFile(imgPath, &FindFileData)) != INVALID_HANDLE_VALUE) {
		do {
			char* finename = (char*)malloc(256);
			strncpy_s(finename, 256, FindFileData.cFileName, 256);
			imgNameList.push_back(finename);
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	else {
		printf("INVALID_HANDLE_VALUE");
		exit(1);
	}
}

//어파인 변환에 사용되는 어파인 맵을 반환
Mat getAffineMap(Point2d center, double dgree, double fx = 1, double fy = 1,
	Point2d translate = Point(0, 0))
{
	Mat rot_map = Mat::eye(3, 3, CV_64F);
	Mat center_trans = Mat::eye(3, 3, CV_64F);
	Mat org_trans = Mat::eye(3, 3, CV_64F);
	Mat scale_map = Mat::eye(3, 3, CV_64F);
	Mat trans_map = Mat::eye(3, 3, CV_64F);

	double radian = dgree / 180 * CV_PI;
	rot_map.at<double>(0, 0) = cos(radian);
	rot_map.at<double>(0, 1) = sin(radian);
	rot_map.at<double>(1, 0) = -sin(radian);
	rot_map.at<double>(1, 1) = cos(radian);

	center_trans.at<double>(0, 2) = center.x;
	center_trans.at<double>(1, 2) = center.y;
	org_trans.at<double>(0, 2) = -center.x;
	org_trans.at<double>(1, 2) = -center.y;

	scale_map.at<double>(0, 0) = fx;
	scale_map.at<double>(1, 1) = fy;
	trans_map.at<double>(0, 2) = translate.x;
	trans_map.at<double>(1, 2) = translate.y;

	Mat ret_map = center_trans * rot_map * trans_map * scale_map * org_trans;
	

	ret_map.resize(2);
	return ret_map;
}

//랜덤하게 어파인 변환을 적용하여 이미지를 늘림
void imageAugmentation(vector<char*>& imgNameList, int flag) {//question = "is Non-Fire?"

	for (const auto& a : imgNameList) {
		char img[256] = FOLDER;
		char buf[256];
		char folder[256] = FOLDER;
		sprintf_s(buf, "%d/", flag);
		strncat_s(img, buf, 256);
		strncat_s(folder, buf, 256);
		strncat_s(img, a, 256);
		Mat image = imread(img, IMREAD_COLOR);
		CV_Assert(!image.empty());

		Point center = image.size() / 2;
		double  angle = 90.0;
		Mat dst[5];
		srand(time(NULL));
		Mat aff_map1 = getAffineMap(center, angle);
		Mat aff_map2 = getAffineMap(center, 0, ((float)(rand() % 10) / 10.0) + 0.1, ((float)(rand() % 10) / 10.0) + 0.1);
		Mat aff_map3 = getAffineMap(center, angle, ((float)(rand() % 10) / 10.0) + 0.1, ((float)(rand() % 10) / 10.0) + 0.1);
		Mat aff_map4 = getAffineMap(center, angle * ((float)(rand() % 10) / 3.0));
		Mat aff_map5 = getAffineMap(center, angle * ((float)(rand() % 10) / 3.0));

		warpAffine(image, dst[0], aff_map1, image.size());
		warpAffine(image, dst[1], aff_map2, image.size());
		warpAffine(image, dst[2], aff_map3, image.size());
		warpAffine(image, dst[3], aff_map4, image.size());
		warpAffine(image, dst[4], aff_map5, image.size());

		char fileName[256];
		strncpy_s(fileName,256, a, 256);
		for (int i = 0; i < strlen(fileName); i++) {
			if (fileName[i] == '.')
				fileName[i] = '_';
		}

		for (int i = 0; i < 5; i++) {
			char newImg[256];
			strncpy_s(newImg, folder, 256);
			sprintf_s(newImg, "%s%s_dst%d.jpg", newImg,fileName, i);
			imwrite(newImg, dst[i]);
			printf("%s\n", newImg);
		}
	}
}

int main(void) 
{
	vector<char*> nonFireFileList;
	vector<char*> fireFileList;
	vector<char*> smokeFileList;
	char nonFireDir[256];
	char fireDir[256];
	char smokeDir[256];
	char cwd[256];
	sprintf_s(nonFireDir, "%s/0/*.jpg", FOLDER);
	sprintf_s(fireDir, "%s/1/*.jpg", FOLDER);
	sprintf_s(smokeDir, "%s/2/*.jpg", FOLDER);


	getFileList(nonFireDir, nonFireFileList);
	getFileList(fireDir, fireFileList);
	getFileList(smokeDir, smokeFileList);
	
	imageAugmentation(nonFireFileList, 0);
	imageAugmentation(fireFileList, 1);
	imageAugmentation(smokeFileList, 2);

	return 0;
}