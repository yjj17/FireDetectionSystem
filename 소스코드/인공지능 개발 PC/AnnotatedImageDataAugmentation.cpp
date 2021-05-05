#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <windows.h>

using namespace cv;
using namespace std;

#define FOLDER "./data/crawled_images/annotated/train/" //이미지(.jpg)와 이미지정보(xml) 파일이 저장된 경로
#define IMGNAMEPATH "./data/crawled_images/annotated/train/*.jpg" //이미지 이름을 저장하기 위해 필요한 path 정보

//imgPath 폴더에 존재하는 모든 이미지(.jpg) 파일 이름을 imgList에 저장
void getFileList(const char* imgPath, vector<char*>& imgList) {
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	if ((hFind = FindFirstFile(imgPath, &FindFileData)) != INVALID_HANDLE_VALUE) {
		do {
			char* finename = (char*)malloc(256);
			strncpy_s(finename, 256, FindFileData.cFileName, 256);
			imgList.push_back(finename);
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	else {
		printf("INVALID_HANDLE_VALUE");
		exit(1);
	}

}

//이미지(Mat)와 두 개의 포인트(p1, p2)를 토대로 이미지정보(.xml)파일과 이미지(.jpg) 파일 생성
void writeXML(Mat& image,char * imgName,Point pt1, Point pt2) {
	char img[256];
	char dstImgName[256];
	char xml[256] = ".xml";
	char dstXmlName[256];
	strncpy_s(img, 256, imgName, 256);
	sprintf_s(dstImgName, "%s%s", FOLDER, img);

	for (int i = 0; i < strlen(img); i++) {
		if (img[i] == '.') {
			img[i] = '\0';
			break;
		}
	}
	sprintf_s(dstXmlName, "%s%s%s", FOLDER, img, xml);
	printf("imwrite: %s\nfprintf: %s\n", dstImgName, dstXmlName);
	imwrite(dstImgName, image);
	FILE* fp;
	fopen_s(&fp, dstXmlName, "wt");
	fprintf(fp, "<annotation>\n"
		"\t<folder>annotated</folder>\n"
		"\t<filename>%s</filename>\n"
		"\t<path>%s</path>\n"
		"\t<source>\n"
		"\t\t<database>Unknown</database>\n"	
		"\t</source>\n"
		"\t<size>\n"
		"\t\t<width>%d</width>\n"
		"\t\t<height>%d</height>\n"
		"\t\t<depth>%d</depth>\n"
		"\t</size>\n"
		"\t<segmented>1</segmented>\n"
		"\t<object>\n"
		"\t\t<name>firew</name>\n"
		"\t\t<pose>Unspecified</pose>\n"
		"\t\t<truncated>0</truncated>\n"
		"\t\t<difficult>0</difficult>\n"
		"\t\t<bndbox>\n"
		"\t\t\t<xmin>%d</xmin>\n"
		"\t\t\t<ymin>%d</ymin>\n"
		"\t\t\t<xmax>%d</xmax>\n"
		"\t\t\t<ymax>%d</ymax>\n"
		"\t\t</bndbox>\n"
		"\t</object>\n"
		"</annotation>", imgName, dstImgName, image.cols, image.rows, image.channels(), pt1.x, pt1.y, pt2.x, pt2.y);
}

//srcXmlName 파일에서 정수를 원소로 가지는 특정 tag를 찾아서 원소를 정수로 반환함
int readTag(const char *srcXmlName, const char * tag) {

	FILE* fp;
	fopen_s(&fp, srcXmlName, "rt");
	if (fp == NULL) {
		printf("%s 파일 open 실패", srcXmlName);
		exit(1);
	}

	char c;
	bool ok;
	char cx[100];
	while (!feof(fp)) {
		c = fgetc(fp);
		if (c != '<')
			continue;
		ok = true;
		for (int i = 0; i < strlen(tag); i++) {
			c = fgetc(fp);
			if (c != tag[i]) {
				while (!feof(fp)&&(c = fgetc(fp)) != '>') {  }
				ok = false;
				break;
			}
		}
		if (ok) {
			int i = 0;
			fgetc(fp);
			while (!feof(fp)&&(c = fgetc(fp)) != '/') {
				cx[i] = c;
				i++;
			}
			cx[i - 1] = '\0';
			
			fclose(fp);
			return atoi(cx);
		}
	}
	fclose(fp);
	return -1;
}

//src_xml 파일에서 태그 <ymin> , <xmin>, <xmax>, <ymax> 를 읽어서 두 개의 점 p1, p2에 저장함
void readXML(char * srcImgName, Mat& image, Point & pt1, Point & pt2) {
	char img[256];
	char dstImgName[256];
	char xml[256] = ".xml";
	char srcXmlName[256];
	strncpy_s(img, 256, srcImgName, 256);
	sprintf_s(dstImgName, "%s%s", FOLDER, img);

	for (int i = 0; i < strlen(img); i++) {
		if (img[i] == '.') {
			img[i] = '\0';
			break;
		}
	}
	sprintf_s(srcXmlName, "%s%s%s", FOLDER, img, xml);
	printf("imread: %s\nfscanf: %s\n", dstImgName, srcXmlName);
	
	pt1.y = readTag(srcXmlName, "ymin");
	pt1.x = readTag(srcXmlName, "xmin");
	pt2.x = readTag(srcXmlName, "xmax");
	pt2.y = readTag(srcXmlName, "ymax");

	image = imread(dstImgName, IMREAD_COLOR);
	CV_Assert(!image.empty());
}

//이미지(.jpg)와 이미지정보(.xml)을 읽어서 원근 투시 변환하고 새로운 이미지와 이미지정보를 저장한다.
void annotatedImageAugmentation(vector<char*>& imgList, int flag) {
	Point pt1, pt2;
	Mat image;
	for (const auto & img_name : imgList) {
		readXML(img_name, image, pt1, pt2);
		Point2f inputQuad[4];
		Point2f outputQuad[4];

		inputQuad[0] = Point2f(pt1.x, pt1.y);	//왼쪽위
		inputQuad[1] = Point2f(pt2.x, pt1.y);	//오른쪽위
		inputQuad[2] = Point2f(pt2.x, pt2.y);	//오른쪽아래
		inputQuad[3] = Point2f(pt1.x, pt2.y);	//왼쪽아래

		srand(time(NULL));
		int centerX = (pt1.x + pt2.x) / 2;
		int centerY = (pt1.y + pt2.y) / 2;
		int left = (rand() % centerX);
		int top = (rand() % centerY);
		int right = image.cols - (rand() % (image.cols - centerX));
		int bottom = image.rows - (rand() % (image.rows - centerY));
		outputQuad[0] = Point2f(left, top);	//왼쪽위
		outputQuad[1] = Point2f(right,top );	//오른쪽위
		outputQuad[2] = Point2f(right,bottom);	//오른쪽아래
		outputQuad[3] = Point2f(left,bottom);	//왼쪽아래

		rectangle(image, pt1, pt2, Scalar(0, 255, 0), 2);
		imshow("원본", image);

		warpPerspective(image,image,getPerspectiveTransform(inputQuad, outputQuad),image.size());

		rectangle(image, outputQuad[0], outputQuad[2], Scalar(0, 0, 255), 2);
		imshow("원근 투시 변환", image);

		waitKey();
		char augImgName[256] = "augmented_";
		sprintf_s(augImgName, "%s%s", augImgName, img_name);
		writeXML(image, augImgName, outputQuad[0], outputQuad[2]);
		
	}
}

int main(void)
{
	//
	vector<char*> imgList;
	getFileList(IMGNAMEPATH, imgList);

	annotatedImageAugmentation(imgList, 1);

	return 0;
}