import data from './data.json' with { type: 'json' }

let url, headers, body

if (data['miHoYo_cookie'] != '' && data['miHoYo_uid'] != '') {
    console.log('开始米游社签到')
    url = 'https://api-takumi.mihoyo.com/event/luna/hk4e/sign'
    headers = {
        "User-Agent": `Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBS/${data['miHoYo_ua_version']}`,
        "x-rpc-signgame": "hk4e",
        "Referer": "https://act.mihoyo.com/",
        "Cookie": data['miHoYo_cookie']
    }
    body = {
        "act_id": "e202311201442471",
        "region": "cn_gf01",
        "uid": data['miHoYo_uid'],
        "lang": "zh-cn"
    }
    await sign(url, headers, body)
}

if (data['HoYoLAB_cookie'] != '') {//,"lang":"zh-cn"
    console.log('开始HoYoLAB签到')
    url = 'https://sg-hk4e-api.hoyolab.com/event/sol/sign?lang=zh-cn'
    headers = {
        'User-Agent': `Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBSOversea/${data['HoYoLAB_ua_version']}`,
        'Referer': "https://act.hoyolab.com/",
        'Cookie': data['HoYoLAB_cookie']
    }
    body = { 'act_id': 'e202102251931481' }
    await sign(url, headers, body)
}

console.log('签到完成,3秒后退出程序')

setTimeout(() => {
    process.exit()
}, 3000)

async function sign(url, headers, body) {
    const abort = new AbortController()
    const timeID = setTimeout(() => {
        console.log('请求超时')
        abort.abort()
    }, 10000)
    let res = await fetch(url, {
        method: 'POST',
        headers: headers,
        body: JSON.stringify(body),
        signal: abort.signal
    }).then(res => {
        clearTimeout(timeID)
        return res.json()
    }).catch(err => {
        console.error(`请求错误${err}`)
        clearTimeout(timeID)
    })
    if (res) {
        console.log(res)
    }
}

import crypto from 'node:crypto'
const md5 = (str) => crypto.createHash('md5').update(str).digest('hex')

async function getDS(body) {
    const salt = 't0qEgfub6cvueAPgR5m9aQWWVciEer7v'
    const t = Math.floor(Date.now() / 1000)
    let r = Math.floor(Math.random() * 100001 + 100000)
    if (r == 100000) {
        r = 642367
    }
    let temp_body = JSON.stringify(body, Object.keys(body).sort())
    const main = `salt=${salt}&t=${t}&r=${r}&b=${temp_body}`
    const ds = md5(main)

    return `${t},${r},${ds}`
}