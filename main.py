import json
import time
import os
import urllib.request
import urllib.error

path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data.json")
data = json.loads(open(path, "r").read())


import random
from hashlib import md5


def getDS(body):
    salt = "t0qEgfub6cvueAPgR5m9aQWWVciEer7v"
    t = int(time.time())
    r = random.randint(100000, 200000)
    if r == 100000:
        r = 642367
    temp_body = json.dumps(body, sort_keys=True)
    main = f"salt={salt}&t={t}&r={r}&b={temp_body}"
    ds = md5(main.encode(encoding="UTF-8")).hexdigest()
    return f"{t},{r},{ds}"


def sign(url, header, data_json):
    try:
        data = json.dumps(data_json).encode("utf-8")
        req = urllib.request.Request(url, headers=header, data=data, method="POST")
        res = urllib.request.urlopen(req, timeout=10)
        result = res.read().decode("utf-8")
        print(result)
        res.close()
    except urllib.error.URLError as e:
        print(f"网络请求错误: {e}")
    except Exception as e:
        print(f"其他错误: {e}")


if data["miHoYo_cookie"] != "" and data["miHoYo_uid"] != "":
    print("开始米游社签到")
    url = "https://api-takumi.mihoyo.com/event/luna/hk4e/sign"
    header = {
        "User-Agent": f"Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBS/{data['miHoYo_ua_version']}",
        "x-rpc-signgame": "hk4e",
        "Referer": "https://act.mihoyo.com/",
        "Cookie": data["miHoYo_cookie"],
    }
    json_data = {
        "act_id": "e202311201442471",
        "region": "cn_gf01",
        "uid": data["miHoYo_uid"],
        "lang": "zh-cn",
    }

    sign(url, header, json_data)

if data["HoYoLAB_cookie"] != "":
    print("开始HoYoLAB签到")
    url = "https://sg-hk4e-api.hoyolab.com/event/sol/sign?lang=zh-cn"
    header = {
        "User-Agent": f"Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBSOversea/{data['HoYoLAB_ua_version']}",
        "Referer": "https://act.hoyolab.com/",
        "Cookie": data["HoYoLAB_cookie"],
    }
    json_data = {"act_id": "e202102251931481"}

    sign(url, header, json_data)

print("签到完成")
time.sleep(2)
