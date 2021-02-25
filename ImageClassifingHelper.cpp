#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <windows.h>

using namespace cv;
using namespace std;

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

void removeImageFile(vector<char*>& list,const char * question, int flag) {//question = "is Non-Fire?"

	Mat info = Mat(210, 800, CV_8UC3, Scalar(255, 255, 255));
	
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("조작법\n\n Delete: NO  (현재 띄워진 이미지 삭제) \nSpace: YES  (다음 이미지 보기)\nESC: 종료\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

	putText(info, question, Point(0, 50), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255));
	putText(info, "No: Delete", Point(0, 110), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));
	putText(info, "YES: Space", Point(0, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));
	putText(info, "QUIT: ESC", Point(0, 180), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));

	imshow(question, info);
	moveWindow(question, 0, 0);
	for (const auto& a : list) {
		char img[256] = "./data/crawled_images/";
		char buf[256];
		sprintf_s(buf, "%d/", flag);
		strncat_s(img, buf, 256);
		strncat_s(img, a, 256);
		Mat image = imread(img, IMREAD_COLOR);
		resize(image, image, Size(400, 400));
		imshow(a, image);
		moveWindow(a, 400, 100);
		int key = waitKey();
		if (key == 0) {
			remove(img);
			printf("%s is removed\n", img);
			Sleep(2);
		}
		if (key == 27) {
			break;
		}
		destroyWindow(a);
	}
	destroyWindow(question);
}


int main(void) {
	vector<char*> nonFireFileList;
	vector<char*> fireFileList;
	vector<char*> smokeFileList;
	char nonFireDir[256] = "./data/crawled_images/0/*.jpg";
	char fireDir[256] = "./data/crawled_images/1/*.jpg";
	char smokeDir[256] = "./data/crawled_images/2/*.jpg";
	char cwd[256];

	
	GetCurrentDirectoryA(256, cwd);
	
	getFileList(nonFireDir, nonFireFileList);
	getFileList(fireDir, fireFileList);
	getFileList(smokeDir, smokeFileList);

	removeImageFile(nonFireFileList, "is Non-Fire?",0);
	removeImageFile(fireFileList, "is Fire?",1);
	removeImageFile(smokeFileList, "is Smoke?",2);
}