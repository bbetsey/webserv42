#!/usr/local/bin/python3
import os
import datetime

if os.environ['PATH_INFO'] == "eval/cookieroot/secret.html" and os.environ["COOKIE"] != "secret":
	print("Status: 403")
elif os.environ['PATH_INFO'] == "eval/cookieroot/secret.html" and os.environ["COOKIE"] == "secret":
	print('Status: 200')
else:
	data = os.environ["DATA"]

	if len(data) > 14:
		username = data.split("&")[0].split("=")[1]
		password = data.split("&")[1].split("=")[1]
		
		if username == "admin" and password == "a":
			expires = datetime.datetime.utcnow() + datetime.timedelta(seconds=30)
			print('Status: 302\nLocation: http://localhost:3333/secret.html\nSet-Cookie: secret; Expires=' + expires.strftime("%a, %d %b %Y %H:%M:%S GMT"))
		else:
			print('Status: 302\nLocation: http://localhost:3333/login.html')
	else:
		if os.environ["COOKIE"] == "secret":
			print('Status: 302\nLocation: http://localhost:3333/secret.html')
		else:
			print('Status: 200')