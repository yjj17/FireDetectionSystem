# 프로그램
* 데이터 전처리를 위한 소스코드 : data preprocessing1.ipynb	
* fire classification을 위한 소스코드 : fire_classification.ipynb
* fire localization 학습을 위한 소스코드 : fire_Localization.ipynb
* video에서 fire localization을 수행하는 소스코드: Video_Fire_Localization.ipynb
* image crawling을 위한 소스코드 : image_crawling.ipynb
* crawling한 이미지를 분류하기위한 프로그램: ImageClassifingHelper.exe
* crawling한 이미지 annotation을 위한 프로그램:ImageAnnotationHelper.exe
* crawling한 이미지 데이터 증강 프로그램: crawled_image_data_augmentation.exe
* annotation한 데이터 증강 프로그램: AnnotatedImageDataAugmentation.exe

# 실행방법
1. 이미지 crawling & augmentation
	1) 크롬드라이버를 현재 폴더에 다운로드 한다.(자세한 사항은 image_crawling.ipynb에서 확인가능
	2) image_crawling.ipynb를 실행한다.
	3) ImageClassifingHelper.exe를 실행하여 잘못 crawling된 파일을 삭제한다.
	4) ImageAnnotationHelper.exe를 실행하여 불의 위치 정보를 xml파일로 저장한다.
	5) crawled_image_data_augmentation.exe를 실행하여 데이터 증강
	6) AnnotatedImageDataAugmentation.exe를 실행하여 4번에서 Annotation한 데이터 증강

2. Classification 학습
	1) fire_classification.ipynb 실행

3. Localization 학습
	1) fire_Localization.ipynb 실행
	2) Video_Fire_Localization.ipynb 실행

kaggle등에서 이미지를 다운받을 수 있다.
복제 및 재배포 등의 라이센스 문제로 github에는 이미지를 commit하지 않는다.