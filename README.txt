# 학습 프로그램
소스코드/인공지능 개발 PC
python
     * fire classification을 위한 소스코드 : fire_classification_using_crawled_data.ipynb
     * fire localization 학습을 위한 소스코드 : fire_Localization_using_crawled_data.ipynb
     * image crawling을 위한 소스코드 : image_crawling.ipynb
     * video에서 Classification+localization을 수행하는 소스코드: AI_Module.ipynb

C++
     * crawling한 이미지중 중복되는 이미지 제거 프로그램: DuplicatedImagesRemover.exe
     * crawling한 이미지를 분류하기위한 프로그램: ImageClassifingHelper.exe
     * crawling한 이미지 annotation을 위한 프로그램:ImageAnnotationHelper.exe
     * crawling한 이미지 데이터 증강 프로그램: crawled_image_data_augmentation.exe
     * annotation한 데이터 증강 프로그램: AnnotatedImageDataAugmentation.exe



# 학습 방법
1. 이미지 crawling & augmentation
	1) 크롬드라이버를 현재 폴더에 다운로드 한다.(자세한 사항은 image_crawling.ipynb에서 확인가능
	2) image_crawling.ipynb를 실행한다.
	3) DuplicatedImagesRemover.exe을 실행하여 중복된 이미지를 삭제한다.
	4) ImageClassifingHelper.exe를 실행하여 잘못 crawling된 이미지를 삭제한다.
	5) ImageAnnotationHelper.exe를 실행하고 불의 위치를 드래그하여 이미지의 정보를 xml파일로 저장한다
	6) image_data_augmentation.exe를 실행하여 데이터 증강
	7) AnnotatedImageDataAugmentation.exe를 실행하여 4번에서 Annotation한 데이터 증강

2. Classification 학습
	선행) 1. 이미지 crawling & augmentation
	1) Fire_Classification.ipynb 실행
	결과) "./data/crawled_images/"에 있는 데이터로 학습을 진행하고 그 결과를 "./result" 모델을 "./model"에 저장

3. Localization 학습
	선행) 1. 이미지 crawling & augmentation
	1) Fire_Localization.ipynb 실행
	결과) "./data/annotated"에 있는 데이터로 학습을 진행하고 그 결과를 "./result" 모델을 "./model"에 저장

4. 비디오에 Classification 및 Localization 적용
	선행) 비디오파일을 "./data/video/CCTV/"에 저장한다. , 2. Classification 학습, 3. Localization 학습
	1) AI_Module.ipynb 실행


복제 및 재배포 등의 라이센스 문제로 github에는 이미지를 commit하지 않는다.

# CCTV 프로그램
소스코드/raspberrypi 
	* 동영상 파일에서 데이터를 읽어서 우분투에 있는 CCTV-server로 송신: CCTV-Client.py

소스코드/ubuntu
	* CCTV-Client에서 데이터를 수신받아 화면에 출력: CCTV_Server.py
	* model: 학습된 모델이 저장된 디렉토리
