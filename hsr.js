// 崩坏：星穹铁道 国际服(亚服)签到
import data from './data.json' with { type: 'json' }

const headers = new Headers()
headers.append('User-Agent', `Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBS/${data['miHoYo_ua_version']}`);
headers.append('x-rpc-signgame', 'hkrpg')
headers.append('Referer', 'https://act.hoyolab.com/')
headers.append('Cookie', data['HoYoLAB_cookie'])

fetch('https://sg-public-api.hoyolab.com/event/luna/hkrpg/os/sign', {
    method: 'POST',
    headers: headers,
    body: JSON.stringify({
        "act_id": "e202303301540311",
        "lang": "zh-cn"
    })
}).then(response => response.json())
    .then(result => {
        console.log(result)
        console.log('签到完成,3秒后退出程序')
    })

setTimeout(() => {
    void 1
}, 3000)