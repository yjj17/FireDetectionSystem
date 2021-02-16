데이터 전처리를 위한 소스코드 : data preprocessing1.ipynb	
fire classification을 위한 소스코드 : fire_classification.ipynb
fire localization을 위한 소스코드 : fire_Localization.ipynb
	

위 소스코드를 실행하기 위해서는 data 폴더에는 다음과 같은 구조로 데이터가 들어있어야 한다.

<data>
	
	<annotated>
		<test>
			localization을 테스트할 이미지1 파일.jpg
			이미지1의 정보를 담고 있는 파일.xml
			...
		</test>
		<train>
			localization을 학습시킬 이미지1 파일.jpg
			이미지1의 정보를 담고 있는 파일.xml
			...
		</train>
	</annotated>
	
	<CAMERA>
		<0> 불이 없는 사진1.jpg, ... </0>
		<1> 불이 있는 사진1.jpg, ...</1>
		<2> 불은 없고 연기만 있는 사진1.jgp, ... </2>
		<test> 테스트를 위한 사진1.jgp, ... </test>
	</CAMERA>

	<CCTV>
		<0> 불이 없는 CCTV 사진1.jpg, ... </0>
		<1> 불이 있는 CCTV 사진1.jpg, ... </1>
		<2> 불은 없고 연기만 있는 CCTV 사진1.jgp, ... </2>
		<test> 테스트를 위한 CCTV 사진1.jgp, ... </test>
	</CCTV>

	<video>
		<CCTV> CCTV 영상들 </CCTV>
	</video>

</data>

kaggle등에서 이미지를 다운받을 수 있다.
복제 및 재배포 등의 라이센스 문제로 github에는 이미지를 commit하지 않는다.