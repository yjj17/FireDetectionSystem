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

//모든 이미지를 한 장씩 출력하면서 삭제 여부를 질의 함
void removeImageFile(vector<char*>& imgNameList,const char * question, int flag) {//question = "is Non-Fire?"

	Mat info = Mat(230, 800, CV_8UC3, Scalar(255, 255, 255));
	
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("조작법\n\n Delete: NO  (현재 띄워진 이미지 삭제) \nSpace: YES  (다음 이미지 보기) \n Backspace: 이전 이미지 보기\nESC: 종료\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

	putText(info, question, Point(0, 50), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255));
	putText(info, "No: Delete", Point(0, 110), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));
	putText(info, "YES: Space", Point(0, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));
	putText(info, "Back: Backspace", Point(0, 180), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));
	putText(info, "QUIT: ESC", Point(0, 210), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));

	imshow(question, info);
	moveWindow(question, 0, 0);
	for (int i = 0; i < imgNameList.size();i++) {		
		char img[256] = FOLDER;
		char buf[256];
		sprintf_s(buf, "%d/", flag);
		strncat_s(img, buf, 256);
		strncat_s(img, imgNameList[i], 256);
		Mat image = imread(img, IMREAD_COLOR);
		resize(image, image, Size(400, 400));
		imshow(img, image);
		moveWindow(img, 400, 100);
		int key = waitKey();
		if (key == 0) {
			remove(img);
			printf("%s is removed\n", img);
			imgNameList.erase(imgNameList.begin() + i);
			i--;
			Sleep(2);
		}
		if (key == 27) {
			break;
		}
		if (key == 8) {
			if (i > 0)
				i -= 2;
			else if (i == 0)
				i -= 1;
		}
		destroyWindow(img);
	}
	destroyWindow(question);
}


int main(void) {
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

	removeImageFile(nonFireFileList, "is Non-Fire?",0);
	removeImageFile(fireFileList, "is Fire?",1);
	removeImageFile(smokeFileList, "is Smoke?",2);
}