# 프로그램
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



# 실행방법
1. 이미지 crawling & augmentation
	1) 크롬드라이버를 현재 폴더에 다운로드 한다.(자세한 사항은 image_crawling.ipynb에서 확인가능
	2) image_crawling.ipynb를 실행한다.
	3) DuplicatedImagesRemover.exe을 실행하여 중복된 이미지를 삭제한다.
	4) ImageClassifingHelper.exe를 실행하여 잘못 crawling된 이미지를 삭제한다.
	5) ImageAnnotationHelper.exe를 실행하고 불의 위치를 드래그하여 이미지의 정보를 xml파일로 저장한다
	6) crawled_image_data_augmentation.exe를 실행하여 데이터 증강
	7) AnnotatedImageDataAugmentation.exe를 실행하여 4번에서 Annotation한 데이터 증강

2. Classification 학습
	선행) 1. 이미지 crawling & augmentation
	1) fire_classification_using_crawled_data.ipynb 실행

3. Localization 학습
	선행) 1. 이미지 crawling & augmentation
	1) fire_Localization_using_crawled_data.ipynb 실행

4. 비디오에 Classification 및 Localization 적용
	선행) 비디오파일을 "./data/video/CCTV/"에 저장한다.
	1) AI_Module.ipynb 실행

kaggle등에서 이미지를 다운받을 수 있다.
복제 및 재배포 등의 라이센스 문제로 github에는 이미지를 commit하지 않는다.
