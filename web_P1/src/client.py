import requests as req

SERV_ADD='http://127.0.0.1:8000/index.html'

r = req.get(SERV_ADD)
print(r)