# -*- coding: utf-8 -*-

import sys
import socket
import json

HOST = 'api.openweathermap.org'
#port pro HTTP a weby
PORT = 80

#jako prvni argument se očekává klic k api
API_KEY = sys.argv[1]
#jako druhý argument se očekává jméno obce
CITY = sys.argv[2]

#pokud neni zadany klic nebo obec, skript konci
if CITY == '' or API_KEY == '':
	sys.exit("ERROR: ARGUMENT MISSING")

#vytvoreni socketu pomocí knihovny
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.connect((HOST, PORT))
	#vytvoreni odchozi zpravy
	message = str.encode('GET /data/2.5/weather?q=' + CITY + '&APPID=' + API_KEY + '&units=metric HTTP/1.1\r\nHost: api.openweathermap.org\r\n\r\n')
	s.sendall(message)
	#prijmuti dat
	data = s.recv(1024)
	
#zmena kodovani na utf-8
data = str(data, "utf-8")
#rozdeleni prijmutych dat na hlavicku a data s pocasim
header, weather_data = data.split("\r\n\r\n")
#rozdeleni hlavicky do pole
header = header.split(" ")
#na druhem prvku pole se nachazi navratova hodnota, 200 je v poradku, cokoliv jineho je chyba
if header[1] != '200':
	#pokud prisla s chybou i zprava, tak ji vypiseme
	if weather_data.get('message'):	
		sys.exit(weather_data['message'])
	else:
		sys.exit("INTERNAL ERROR")

#nacteni pomoci knihovny json
weather_data = json.loads(weather_data)

#udaje nemusi prijit, kontrolujeme pres try except
try:
	#pomoci knihovny json vytahneme jmeno mesta a vypiseme
	name = weather_data['name']
	print(name)
except:
	#pokud by jmeno nedoslo, vypiseme jmeno zadane uzivatelem
	print(CITY)

try:
	#vytahneme aktualni stav pocasi a vypiseme
	description = weather_data['weather'][0]['description']
	print (description)
except:
	print ("description not available")

try:
	#vypis teploty
	temperature = weather_data['main']['temp']
	print("temp:%.2f°C" %temperature)
except:
	print("temp: not available")

try:
	#vlhkost vzduchu
	humidity = weather_data['main']['humidity']
	print("humidity:%d%%" %humidity)
except:
	print("humidity: not available")

try:
	#tlak
	pressure = weather_data['main']['pressure']
	print("pressure:%d hPA" %pressure)
except:
	print("pressure: not available")

try:
	#rychlost vetru
	speed = weather_data['wind']['speed']*3.6
	print("wind-speed: %.1fkm/h" %speed)
except:
	print("wind-speed: not available")

try:
	#smer vetru
	deg = weather_data['wind']['deg']
	print("wind-deg: %d" %deg)
except:
	print("wind-deg: not available") 
