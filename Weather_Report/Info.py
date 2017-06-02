# -*- coding: utf-8 -*-
# Name: Info.py
# Weather: class the stores infomation about current weather
# Author: Hu Hu
# Email: mihawkhu@gmail.com
# Function: Get weather information from API of heweather.com

import urllib
import urllib2
import json
import sys
import socket


class Weather():
    def __init__(self):
        # initialize city id infomation
        # city info: city_id, en_name, county, city, province
        city_file = open("city_info.txt", "r")
        self.city_list = [i.split() for i in city_file.readlines()]
        city_file.close()
        # initialize key and url of this API
        self.KEY = 'd11591eba81f48329224324a43e8313f'
        self.BASE_URL = 'https://api.heweather.com/x3/weather?'
    
    # get_city_id: returns the info about the required city
    def get_city_info(self, city_name):
        for city in self.city_list:
            if city[2].decode('utf8') == city_name:
                return city
        raise Exception("invalid city")
    
    # get_json_data: fetch data in json format via weather API
    def get_json_data(self, city_name):
        city_info = self.get_city_info(city_name)
        url = self.BASE_URL + 'cityid='+city_info[0] + '&key='+self.KEY
        html = urllib2.urlopen(url).read()
        return json.loads(html)['HeWeather data service 3.0'][0]
        
    # parse_json_data: return weather info in python dictionary
    def parse_json_data(self, json_data):
        if json_data['status'] != 'ok':
            raise Exception("fetch data error")
        weather_info = {}
        # basic information
        weather_info['basic'] = {}
        weather_info['basic']['city_name'] = json_data['basic']['city']
        weather_info['basic']['city_id'] = json_data['basic']['id']
        weather_info['basic']['update_time'] = json_data['basic']['update']['loc']
        # air quality index
        weather_info['aqi'] = {}
        weather_info['aqi']['aqi'] = json_data['aqi']['city']['aqi']
        weather_info['aqi']['co'] = json_data['aqi']['city']['co']
        weather_info['aqi']['no2'] = json_data['aqi']['city']['no2']
        weather_info['aqi']['o3'] = json_data['aqi']['city']['o3']
        weather_info['aqi']['pm10'] = json_data['aqi']['city']['pm10']
        weather_info['aqi']['pm25'] = json_data['aqi']['city']['pm25']
        weather_info['aqi']['quality'] = json_data['aqi']['city']['qlty']
        weather_info['aqi']['so2'] = json_data['aqi']['city']['so2']
        # current weather condition
        weather_info['current'] = {}
        weather_info['current']['weather'] = json_data['now']['cond']['code']
        weather_info['current']['weather_txt'] = json_data['now']['cond']['txt']
        weather_info['current']['humidity'] = json_data['now']['hum']
        weather_info['current']['temp'] = json_data['now']['tmp']
        weather_info['current']['wind_dir'] = json_data['now']['wind']['dir']
        weather_info['current']['wind_lev'] = json_data['now']['wind']['sc']
        # daily weather forecast
        weather_info['forecast'] = {}
        for i in range(3):
            index = str(i+1)
            this_forecast = json_data['daily_forecast'][i]
            weather_info['forecast'][index] = {}
            weather_info['forecast'][index]['weather'] = this_forecast['cond']['code_d']
            weather_info['forecast'][index]['max_temp'] = this_forecast['tmp']['max']
            weather_info['forecast'][index]['min_temp'] = this_forecast['tmp']['min']
        return weather_info
        
    # query_weather: returns weather infomation of the required city
    def query_city(self, city_name):
        json_data = self.get_json_data(city_name)
        weather_info = self.parse_json_data(json_data)
        return weather_info

