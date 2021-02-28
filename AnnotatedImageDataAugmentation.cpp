#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <windows.h>

using namespace cv;
using namespace std;

void getFileList(char* imgPath, vector<char*>& imgList) {
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
void writeXML(Mat& image,char * imgName,Point pt1, Point pt2) {
	char folder[256] = "./data/crawled_images/annotated/";
	char img[256];
	char dst_img[256];
	char xml[256] = ".xml";
	char dst_xml[256];
	strncpy_s(img, 256, imgName, 256);
	sprintf_s(dst_img, "%s%s", folder, img);

	for (int i = 0; i < strlen(img); i++) {
		if (img[i] == '.') {
			img[i] = '\0';
			break;
		}
	}
	sprintf_s(dst_xml, "%s%s%s", folder, img, xml);
	printf("imwrite: %s\nfprintf: %s\n", dst_img, dst_xml);
	imwrite(dst_img, image);
	FILE* fp;
	fopen_s(&fp, dst_xml, "wt");
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
		"</annotation>", imgName, dst_img, image.cols, image.rows, image.channels(), pt1.x, pt1.y, pt2.x, pt2.y);
}

int readTag(const char *dst_xml, const char * tag) {

	FILE* fp;
	fopen_s(&fp, dst_xml, "rt");
	if (fp == NULL) {
		printf("%s 파일 open 실패", dst_xml);
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
void readXML(char * src_img, Mat& image, Point & pt1, Point & pt2) {
	char folder[256] = "./data/crawled_images/annotated/";
	char img[256];
	char dst_img[256];
	char xml[256] = ".xml";
	char dst_xml[256];
	strncpy_s(img, 256, src_img, 256);
	sprintf_s(dst_img, "%s%s", folder, img);

	for (int i = 0; i < strlen(img); i++) {
		if (img[i] == '.') {
			img[i] = '\0';
			break;
		}
	}
	sprintf_s(dst_xml, "%s%s%s", folder, img, xml);
	printf("imread: %s\nfscanf: %s\n", dst_img, dst_xml);
	
	pt1.y = readTag(dst_xml, "ymin");
	pt1.x = readTag(dst_xml, "xmin");
	pt2.x = readTag(dst_xml, "xmax");
	pt2.y = readTag(dst_xml, "ymax");

	image = imread(dst_img, IMREAD_COLOR);
	CV_Assert(!image.empty());
}
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

		warpPerspective(image,image,getPerspectiveTransform(inputQuad, outputQuad),image.size());
		
		char augImgName[256] = "augmented_";
		sprintf_s(augImgName, "%s%s", augImgName, img_name);
		writeXML(image, augImgName, outputQuad[0], outputQuad[2]);
		
	}
}

int main(void)
{
	vector<char*> imgList;
	char imgDir[256] = "./data/crawled_images/annotated/*.jpg";
	
	getFileList(imgDir, imgList);
	annotatedImageAugmentation(imgList, 1);

	return 0;
}