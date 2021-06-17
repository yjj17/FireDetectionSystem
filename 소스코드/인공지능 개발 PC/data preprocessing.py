import os
import re
from tqdm.notebook import tqdm
from PIL import Image

curPath = os.curdir
dataPath = os.path.join(curPath,'data')

crawlPath = os.path.join(dataPath,'crawled_images')
crawl0Path = os.path.join(crawlPath,'0')
crawl1Path = os.path.join(crawlPath,'1')
crawl2Path = os.path.join(crawlPath,'2')

IMG_SIZE = 224

def removeFile(s, path): #s가 이름에 포함된 파일 삭제
    List = os.listdir(path = path)
    i = 0
    a = re.compile(s)
    for name in tqdm(List):
        if a.search(name) != None:
            p = os.path.join(path,name)
            os.remove(p)

#removeFile('jpg_dst', crawl0Path)
#removeFile('jpg_dst', crawl1Path)
#removeFile('jpg_dst', crawl2Path)

def imageSizeDown(path):
    List = os.listdir(path = path)
    for name in tqdm(List):
        img_path = os.path.join(path,name)
        image = Image.open(img_path)
        image = image.resize((IMG_SIZE, IMG_SIZE))
        image.save(img_path,'JPEG')

#imageSizeDown(crawl0Path)
#imageSizeDown(crawl1Path)
#imageSizeDown(crawl2Path)

def imageSizeDown_test(path):
    List = os.listdir(path = path)
    for name in tqdm(List):
        img_path = os.path.join(path,name)
        new_img_path = os.path.join(path,'t_'+name)
        image = Image.open(img_path)
        image = image.resize((IMG_SIZE, IMG_SIZE))
        image.save(new_img_path, 'JPEG')
        os.remove(img_path)

#testPath = os.path.join(crawlPath,'test')
#test0Path = os.path.join(testPath,'0')
#test1Path = os.path.join(testPath,'1')
#test2Path = os.path.join(testPath,'2')


#imageSizeDown_test(test0Path)
#imageSizeDown_test(test1Path)
#imageSizeDown_test(test2Path)#######