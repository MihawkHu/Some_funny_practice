# copyright (c) 2016 Mihawk Hu
# An easy web spider to download meizi picture from http://www.mztu.com
# Just for fun~

import os
import re
from urllib.request import Request
from urllib.request import urlretrieve
from urllib.request import urlopen
from bs4 import BeautifulSoup

download = "hhh"
baseUrl = "http://www.mzitu.com/mm"
criticalStr = "http://pic.mmfile.net"
head = {'User-Agent':'Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.6) Gecko/20091201 Firefox/3.5.6'}


pages = set()
def getMainLinks(pageUrl):
    global pages
    result = list()
    req_tmp = Request(url = pageUrl, headers = head)
    html_tmp = urlopen(req_tmp)
    bsObj_tmp = BeautifulSoup(html_tmp)
    for link in bsObj_tmp.findAll("a", {"class" : "page-numbers"}):
        if 'href' in link.attrs:
            if link.attrs['href'] not in pages:
                newpage = link.attrs['href']
                result.append(newpage)
                pages.add(newpage)
    return result


def getLinks(pageUrl):
    global pages
    result = list()
    req_tmp = Request(url = pageUrl, headers = head)
    html_tmp = urlopen(req_tmp)
    bsObj_tmp = BeautifulSoup(html_tmp)
    if re.match("^("+baseUrl+")/page/[0-9]+", pageUrl):
        pageUrl = pageUrl[0 : 20]
    if re.match("^("+baseUrl[0 : 20]+")/[0-9]+/[0-9]+", pageUrl):
        pageUrl = re.match("^("+baseUrl[0 : 20]+")/[0-9]+", pageUrl).group()
    for link in bsObj_tmp.findAll("a", href = re.compile("^("+pageUrl+")/[0-9]+")):
        if 'href' in link.attrs:
            if link.attrs['href'] not in pages:
                newpage = link.attrs['href']
                result.append(newpage)
                pages.add(newpage)
    return result

    
def getLinks_dfs(pageUrl, choose):
    pageLink = list()
    if choose == "sub":
        pageLink_temp = getLinks(pageUrl)
    elif choose == "main":
        pageLink_temp = getMainLinks(pageUrl)
    pageLink.extend(pageLink_temp)
    if len(pageLink_temp) != 0:
        for link in pageLink_temp:
            if choose == "sub":
                pageLink.extend(getLinks_dfs(link, "sub"))
            elif choose == "main":
                pageLink.extend(getLinks_dfs(link, "main"))
    return pageLink


def getAbsoluteUrl(baseUrl, source):
    if source.startswith("http://www."):
        url = "http://" + source[11:]
    elif source.startswith("http://"):
        url = source
    elif source.startswith("www."):
        url = "http://" + source[4:]
    else:
        url = baseUrl + "/" + source
    return url


def getPath(baseUrl, abUrl, download, alt):
    path_head = re.match("^("+criticalStr+")/[0-9]+/", abUrl).group()
    path_tail = re.search("/[0-9a-z]+\.jpg+$", abUrl).group()
    path = path_head + alt + path_tail
    path = path.replace(criticalStr, "")
    path = path.replace(baseUrl, "")
    path = download + path
    directory = os.path.dirname(path)
    if not os.path.exists(directory):
        os.makedirs(directory)
    return path


req = Request(url = baseUrl, headers = head)
html = urlopen(req)
bsObj = BeautifulSoup(html)

page_cnt = 0
mainPageUrls = getLinks_dfs(baseUrl, "main")
for curPageUrl in mainPageUrls:
    group_cnt = 0
    print(curPageUrl)
    pageEntries = getLinks(curPageUrl)
    print(pageEntries)
    for linkUrl in pageEntries:
        if group_cnt > 28:
            break
        pageLink = getLinks_dfs(linkUrl, "sub")
        for link in pageLink:
            req_tmp = Request(url = link, headers = head)
            html_tmp = urlopen(req_tmp)
            bsObj_tmp = BeautifulSoup(html_tmp)
            
            downList = bsObj_tmp.findAll("img", src = re.compile("^("+criticalStr+"/)[0-9]+.*\.jpg"))
            for dl in downList:
                fileUrl = getAbsoluteUrl(baseUrl, dl["src"])
                if fileUrl is not None:
                    print(fileUrl)
                    urlretrieve(fileUrl, getPath(baseUrl, fileUrl, download, dl["alt"]))
        group_cnt = group_cnt + 1
        print("Done page", page_cnt, "group", group_cnt)
    page_cnt = page_cnt + 1

print("All done!")
