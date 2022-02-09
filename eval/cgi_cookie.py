import os

data = input()

if data.len() > 0:
	username = data.split("&")[0].split("=")[1]
	password = data.split("&")[1].split("=")[1]
	if username == "admin" and password == "admin":
		print(
			'''
			Status: 302
			Location: http://localhost:3333/secret.html
			Set-Cokkies: secret
			'''
		)
	else:
		print(
			'''
			Status: 302
			Location: http://localhost:3333/login.html
			'''
		)
else:
	if os.environ["COOKIE"]:
		if os.environ["COOKIE"] == "secret":
			print('''
			Status:302
			Location: http://localhost:3333/secret.html
			'''
			)
		else:
			print(
			'''
			Status: 302
			Location: http://localhost:3333/login.html
			'''
			)
