from selenium import webdriver
from bs4 import BeautifulSoup
from urllib.request import urlopen
from urllib.parse import quote_plus
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
import time
import os

NUM = 1000 #크롤링할 이미지 최대 개수(이미지는 NUM이하로 크롤링됨)

driver = webdriver.Chrome('./chromedriver.exe')
driver.implicitly_wait(3)

def loadHtml(path):
    driver.get(path)
    driver.implicitly_wait(3)
    html = driver.page_source
    soup = BeautifulSoup(html, 'html.parser')
    return soup.find_all('img')


def crawlingImage(n, savepath, failStack):
    html = driver.page_source
    soup = BeautifulSoup(html, 'html.parser')
    img = soup.find_all('img')
    print(len(img), n)
    if len(img) <= n:
        # chrome 창 스크롤을 내려서 이미지가 렌더링 되도록 함
        driver.execute_script("window.scrollTo(0,document.body.scrollHeight)")

        # 결과 더보기 버튼이 있으면 클릭
        result = driver.execute_script(
            '''
            if(document.getElementsByClassName("mye4qd")[0].getAttribute("value")==="결과 더보기")
                document.getElementsByClassName("mye4qd")[0].click()
            '''
        )
        failStack += 1
        if failStack > 5:
            return n, -1
        else:
            return n, failStack

    for i in img[n:]:
        print(n)
        try:
            imgUrl = i['src']
        except KeyError:
            try:
                imgUrl = i['data-src']
            except KeyError:
                continue

        #url에서 이미지를 읽어서 지정된 경로에 저장
        with urlopen(imgUrl) as f:
            with open(savepath + str(n) + '.jpg', 'wb') as h:  # w - write b - binary
                img = f.read()
                h.write(img)

        #저장된 이미지 출력
        '''
        with Image.open(savepath + str(n) + '.jpg') as image:
            plt.imshow(image)
            plt.show()
        '''
        n += 1



        if n > NUM:
            break
    failStack = 0
    return n, failStack

#google image에서 'fire' 검색 후 그 url을 복사
img = loadHtml('https://www.google.co.kr/search?hl=ko&tbm=isch&source=hp&biw=1036&bih=674&ei=Dlw2YJLRH5fr-Qbrx7SQDg&q=fire&oq=fire&gs_lcp=CgNpbWcQAzIFCAAQsQMyAggAMgIIADICCAAyBQgAELEDMgIIADICCAAyAggAMggIABCxAxCDATICCABQ0wdYzgpg9gtoAHAAeACAAYsBiAHwA5IBAzAuNJgBAKABAaoBC2d3cy13aXotaW1n&sclient=img&ved=0ahUKEwiSkLKx14LvAhWXdd4KHesjDeIQ4dUDCAc&uact=5')
n = 0
failStack = 0
while n<NUM:
    n, failStack = crawlingImage(n,'./data/crawled_images/1/img',failStack)
    if failStack==-1:
        print("{}개의 이미지 크롤링 성공".format(n))
        break

#google image에서 '"fire smoke in city" OR "fire smoke"' 검색 후 그 url을 복사
img = loadHtml('https://www.google.com/search?q=%22fire+smoke+in+city%22+OR+%22fire+smoke%22&tbm=isch&ved=2ahUKEwj4pIjJ15HvAhVC6pQKHf-6CK0Q2-cCegQIABAA&oq=%22fire+smoke+in+city%22+OR+%22fire+smoke%22&gs_lcp=CgNpbWcQA1CsK1iQLGD5LmgAcAB4AIABWYgBqQGSAQEymAEAoAEBqgELZ3dzLXdpei1pbWfAAQE&sclient=img&ei=hDk-YPjSDsLU0wT_9aLoCg&bih=754&biw=1536&rlz=1C1OKWM_koKR870KR870')
n = 0
failStack = 0
while n<NUM :
    n, failStack = crawlingImage(n,'./data/crawled_images/2/img', failStack)
    if failStack==-1:
        print("{}개의 이미지 크롤링 성공".format(n))
        break

#google image에서 'school OR factory OR school OR street OR landscape' 검색 후 그 url을 복사
img = loadHtml('https://www.google.com/search?as_st=y&tbm=isch&hl=ko&as_q=school+OR+factory+OR+school+OR+street+OR+landscape&as_epq=&as_oq=&as_eq=&cr=&as_sitesearch=&safe=active&tbs=itp:photo,ic:color,ift:jpg')
n = 0
failStack = 0
while n<NUM:
    n, failStack = crawlingImage(n,'./data/crawled_images/0/img', failStack)
    if failStack==-1:
        print("{}개의 이미지 크롤링 성공".format(n))
        break



#RGB가 아닌 이미지 삭제
cur_dir = os.getcwd()
data_dir = os.path.join(cur_dir, 'data')
crawled_dir = os.path.join(data_dir, 'crawled_images')
class0_dir = os.path.join(crawled_dir, '0')
class1_dir = os.path.join(crawled_dir, '1')
class2_dir = os.path.join(crawled_dir, '2')

normal_img_files = os.listdir(class0_dir)
fire_img_files = os.listdir(class1_dir)
smoke_img_files = os.listdir(class2_dir)

for img in normal_img_files:
    img_path = os.path.join(class0_dir, img)
    image = Image.open(img_path)
    image_mode = image.mode
    if image_mode != 'RGB':
        print(img, image_mode)
        image = np.asarray(image)
        print(image.shape)
        os.remove(img_path)

for img in fire_img_files:
    img_path = os.path.join(class1_dir, img)
    image = Image.open(img_path)
    image_mode = image.mode
    if image_mode != 'RGB':
        print(img, image_mode)
        image = np.asarray(image)
        print(image.shape)
        os.remove(img_path)

for img in smoke_img_files:
    img_path = os.path.join(class2_dir, img)
    image = Image.open(img_path)
    image_mode = image.mode
    if image_mode != 'RGB':
        print(img, image_mode)
        image = np.asarray(image)
        print(image.shape)
        os.remove(img_path)