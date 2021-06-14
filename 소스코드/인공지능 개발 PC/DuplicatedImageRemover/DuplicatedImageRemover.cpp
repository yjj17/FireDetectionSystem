#include "histo.hpp"
#include "utils.hpp"

//중복 이미지 삭제를 도와줌
void removeDuplicatedImageFile(vector<char*>& imgNameList, Vec3i & bins, Vec3f & ranges, int flag, double  sinc) {
	
	//모든 이미지의 히스토그램 계산
	vector<Mat> DB_hists = load_histo(imgNameList, bins, ranges, imgNameList.size(), flag);

	//알림판 생성
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
	for (const auto& a : imgNameList) {

		//이미지 열기
		char img[256] = FOLDER;
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
		
		//현재 이미지 히스토그램 계산
		Mat hsv, query_hist;
		cvtColor(query, hsv, COLOR_BGR2HSV);					// HSV 컬러 변환
		calc_histo(hsv, query_hist, bins, ranges, 2);			//히스토그램 계산
		
		//모든 이미지의 히스토그램과 유사도 비교
		Mat DB_similaritys = calc_similarity(query_hist, DB_hists);
		
		//설정값 이상으로 유사도가 높은 이미지만 출력하고 삭제 여부를 질문함
		select_view(imgNameList, sinc, DB_similaritys, bins, ranges,flag,i);

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
	
	printf("유사도 %f", sinc);
	removeN = 0;
	getFileList(nonFireDir, nonFireFileList);
	removeDuplicatedImageFile(nonFireFileList, bins, ranges , 0, sinc);
	printf("삭제된 일반 이미지 : %d\n", removeN);

	removeN = 0;
	getFileList(fireDir, fireFileList);
	removeDuplicatedImageFile(fireFileList, bins, ranges, 1, sinc);
	printf("삭제된 화재 이미지 : %d\n", removeN);

	removeN = 0;
	getFileList(smokeDir, smokeFileList);
	removeDuplicatedImageFile(smokeFileList, bins, ranges, 2, sinc);
	printf("삭제된 연기 이미지 : %d\n", removeN);

	return 0;
}