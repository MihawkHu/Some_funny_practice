# -*- coding: utf-8 -*-
# File name: weather.py
# Fuction: main code of Weather Report App
# Author: Hu Hu
# Email: mihawkhu@gmail.com
# How to use: $ python weather.py

import urllib
import urllib2
import json
import sys
import socket
import tkMessageBox

from Tkinter import *
from PIL import ImageTk, Image
from pprint import pprint
from Info import *


# return the name of the city by ip
def get_current_city():
    # get public IP address of current PC
    ip_html = urllib2.urlopen('http://jsonip.com')
    ip_addr = json.load(ip_html)['ip']
    # get city name from current IPv4
    try:
        socket.inet_aton(ip_addr)
        IP_URL = "http://ip.taobao.com/service/getIpInfo.php?ip=" + ip_addr
        json_data = json.load(urllib2.urlopen(IP_URL))
        city_name = json_data['data']['city']
    except:
        city_name = '上海'
    return city_name


# Update according to new weather information
def update_weather():
    update_time_label.config(text=curr_weather['basic']['update_time'])
    img_path = "./pics/"+curr_weather['current']['weather']+'.png'
    weather_img = ImageTk.PhotoImage(Image.open(img_path))
    weather_pic_label.config(image = weather_img)
    weather_pic_label.image = weather_img
    weather_cond_label.config(text=curr_weather['current']['weather_txt'])
    temp_label.config(text=curr_weather['current']['temp']+u'度')
    humi_label.config(text=curr_weather['current']['humidity']+u'%')
    wind_label.config(text=curr_weather['current']['wind_dir']+\
                            curr_weather['current']['wind_lev']+u'级')


def click_change_city(current_city, city_name, window):
    global curr_weather
    for city in weather.city_list:
        if city[2].decode('utf8') == city_name:
            current_city.set(city_name)
            curr_weather = weather.query_city(city_name)
            update_weather()
            window.destroy()
            return
    tkMessageBox.showinfo('Error', '无效的城市名称')
    

def change_city(current_city):
    cc_window = Toplevel()
    cc_window.geometry('400x100')
    cc_window.resizable(width=FALSE, height=FALSE)
    city_var = StringVar()
    Label(cc_window, text='切换到', font='-size 12', ).place(x=120, y=20, anchor=NW)
    Entry(cc_window, font='-size 12', textvariable=city_var).place(x=180, y=20, width=100, anchor=NW)
    change_button = Button(cc_window, text='确定',\
                command=lambda: click_change_city(current_city, city_var.get(), cc_window))
    change_button.place(x=170, y=60)
    cancel_button = Button(cc_window, text='取消',\
                command=cc_window.destroy)
    cancel_button.place(x=230, y=60)


def display_help():
    help_window = Toplevel()
    help_window.title('帮助')
    help_window.geometry('400x200')
    help_info = '\n天气预报软件\n'\
                '默认显示当前城市天气\n'\
                '点击 切换城市 查询其他城市天气\n'\
                '由于API限制，每天最多支持3000次查询\n\n'\
                'Weather Report 2.0\n'\
                'Copyright (c) 2017, Hu Hu'
    text_label = Label(help_window, text=help_info, font='-size 12')
    text_label.pack()


def display_author():
    author_window = Toplevel()
    author_window.title('Author')
    author_window.geometry('400x150')
    author_window.resizable(width=FALSE, height=FALSE)
    author_info = '\n\n\n'\
                  'Author: Hu Hu, SJTU\n'\
                  'E-mail: mihawkhu@gmail.com\n'
    text_label = Label(author_window, text=author_info, font='-size 12')
    text_label.pack()



def click_aqi():
    aqi_window = Toplevel()
    aqi_window.title('当前空气质量')
    Label(aqi_window, text='空气质量指数', font='-size 11').place(x=20, y=20)
    Label(aqi_window, text=curr_weather['aqi']['aqi'], font='-size 11').place(x=140, y=20)
    Label(aqi_window, text='综合空气质量', font='-size 11').place(x=20, y=40)
    Label(aqi_window, text=curr_weather['aqi']['quality'], font='-size 11').place(x=140, y=40)
    Label(aqi_window, text='一氧化碳浓度', font='-size 11').place(x=20, y=60)
    Label(aqi_window, text=curr_weather['aqi']['co'], font='-size 11').place(x=140, y=60)
    Label(aqi_window, text='二氧化氮浓度', font='-size 11').place(x=20, y=80)
    Label(aqi_window, text=curr_weather['aqi']['no2'], font='-size 11').place(x=140, y=80)
    Label(aqi_window, text='臭氧浓度', font='-size 11').place(x=20, y=100)
    Label(aqi_window, text=curr_weather['aqi']['o3'], font='-size 11').place(x=140, y=100)
    Label(aqi_window, text='二氧化硫浓度', font='-size 11').place(x=20, y=120)
    Label(aqi_window, text=curr_weather['aqi']['so2'], font='-size 11').place(x=140, y=120)
    Label(aqi_window, text='PM 2.5', font='-size 11').place(x=20, y=140)
    Label(aqi_window, text=curr_weather['aqi']['pm25'], font='-size 11').place(x=140, y=140)
    Label(aqi_window, text='PM 10', font='-size 11').place(x=20, y=160)
    Label(aqi_window, text=curr_weather['aqi']['pm10'], font='-size 11').place(x=140, y=160)


def click_forecast():
    forecast_window = Toplevel()
    forecast_window.title('未来三天天气预报')
    forecast_window.geometry('220x300')
    forecast_window.resizable(width=FALSE, height=FALSE)
    Label(forecast_window, text='明天', font='-size 11').place(x=10, y=30)
    img_path = "./pics/"+curr_weather['forecast']['1']['weather']+'.png'
    img = ImageTk.PhotoImage(Image.open(img_path))
    pic_label_1 = Label(forecast_window, image = img)
    pic_label_1.image = img
    pic_label_1.place(x=55, y=-10)
    temp_range = curr_weather['forecast']['1']['min_temp'] + \
                '-' + curr_weather['forecast']['1']['max_temp'] + u'度'
    Label(forecast_window, text=temp_range, font='-size 11').place(x=155, y=30)
    
    Label(forecast_window, text='后天', font='-size 11').place(x=10, y=130)
    img_path = "./pics/"+curr_weather['forecast']['1']['weather']+'.png'
    img = ImageTk.PhotoImage(Image.open(img_path))
    pic_label_2 = Label(forecast_window, image = img)
    pic_label_2.image = img
    pic_label_2.place(x=55, y=90)
    temp_range = curr_weather['forecast']['2']['min_temp'] + \
                '-' + curr_weather['forecast']['2']['max_temp'] + u'度'
    Label(forecast_window, text=temp_range, font='-size 11').place(x=155, y=130)
    
    Label(forecast_window, text='大后天', font='-size 11').place(x=10, y=230)
    img_path = "./pics/"+curr_weather['forecast']['1']['weather']+'.png'
    img = ImageTk.PhotoImage(Image.open(img_path))
    pic_label_3 = Label(forecast_window, image = img)
    pic_label_3.image = img
    pic_label_3.place(x=55, y=190)
    temp_range = curr_weather['forecast']['3']['min_temp'] + \
                '-' + curr_weather['forecast']['3']['max_temp'] + u'度'
    Label(forecast_window, text=temp_range, font='-size 11').place(x=155, y=230)
    
    
def filter_city(city_name):
    last_char = city_name[-1]
    if last_char == u'市' or last_char == u'县':
        return city_name[:-1]
    return city_name


# main routine

# global varaibles
default_city = get_current_city()
default_city = filter_city(default_city)
weather = Weather()
curr_weather = weather.query_city(default_city)


# configure root   
root = Tk()
root.title("天气预报")
root.geometry('250x300')
root.resizable(width=FALSE, height=FALSE)


# display current weather condition
# current city information
Label(root, text='当前城市:', font='-size 11').place(x=65, y=20)
current_city = StringVar()
current_city.set(default_city)
curr_weather_label = Label(root, textvariable=current_city, font='-size 11')
curr_weather_label.place(x=135, y=20)


# last update time
Label(root, text='更新时间:', font='-size 11').place(x=25, y=50)
update_time_label = Label(root, text=curr_weather['basic']['update_time'], font='-size 11')
update_time_label.place(x=95, y=50)


# current weather pic
img_path = "./pics/"+curr_weather['current']['weather']+'.png'
weather_img = ImageTk.PhotoImage(Image.open(img_path))
weather_pic_label = Label(root, image = weather_img)
weather_pic_label.image = weather_img
weather_pic_label.place(y=95)


# current weather info
Label(root, text='天气', font='-size 11').place(x=100, y=100)
weather_cond_label = Label(root, text=curr_weather['current']['weather_txt'],  font='-size 11')
weather_cond_label.place(x=150, y=100)
Label(root, text='温度', font='-size 11').place(x=100, y=120)
temp_label = Label(root, text=curr_weather['current']['temp']+u'度',  font='-size 11')
temp_label.place(x=150, y=120)
Label(root, text='湿度', font='-size 11').place(x=100, y=140)
humi_label = Label(root, text=curr_weather['current']['humidity']+'%',  font='-size 11')
humi_label.place(x=150, y=140)
Label(root, text='风力', font='-size 11').place(x=100, y=160)
wind_label = Label(root, text=curr_weather['current']['wind_dir']+\
                            curr_weather['current']['wind_lev']+u'级',  font='-size 11')
wind_label.place(x=150, y=160)


# configure buttons
aqi_button = Button(root, text='空气质量', command=click_aqi)
aqi_button.place(x=30, y=210)
forecast_button = Button(root, text='未来三天预报', command=click_forecast)
forecast_button.place(x=130, y=210)
quit_button = Button(root, text=' 退出 ', command=root.quit)
quit_button.place(x=100, y=260)


# configure menu bar
m = Menu(root)
root.config(menu=m)
m.add_command(label='切换城市', command=lambda: change_city(current_city))
m.add_command(label='  刷新 ', command=update_weather)
m.add_command(label='  帮助 ', command=display_help)
m.add_command(label='联系作者', command=display_author)

root.mainloop()

