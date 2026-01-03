import 'dart:convert';
import 'dart:io';
import 'package:http/http.dart' as http;
import 'dart:math';
import 'package:crypto/crypto.dart';
import 'dart:io';
import 'package:path/path.dart' as path;

late String miHoYo_uid;
late String miHoYo_cookie;
late String HoYoLAB_cookie;

void main() async {
   String jsonPath = path.join(path.dirname(Directory.current.path), 'data.json');
  
  Map<String, dynamic> json_data = jsonDecode(
    await File(jsonPath).readAsString(),
  );

  miHoYo_cookie = json_data['miHoYo_cookie'];
  miHoYo_uid = json_data['miHoYo_uid'];
  HoYoLAB_cookie = json_data['HoYoLAB_cookie'];

  if (miHoYo_cookie != '' && miHoYo_uid != '') {
    print('开始米游社签到');
    final Uri url = Uri.https(r'api-takumi.mihoyo.com', 'event/luna/hk4e/sign');
    Map<String, String> header = {
      "User-Agent":
          "Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBS/${json_data['miHoYo_ua_version']}",
      //"DS": await getDS(),
      "x-rpc-signgame": "hk4e",
      "Referer": "https://act.mihoyo.com/",
      "Cookie": miHoYo_cookie,
    };
    Map<String, String> body = {
      "act_id": "e202311201442471",
      "region": "cn_gf01",
      "uid": miHoYo_uid,
      "lang": "zh-cn",
    };
    await sign(url, header, body);
  }

  if (HoYoLAB_cookie != '') {
    print('开始HoYoLAB签到');
    final Uri url = Uri.https(r'sg-hk4e-api.hoyolab.com', 'event/sol/sign', {
      'lang': 'zh-cn',
    });
    Map<String, String> header = {
      'User-Agent':
          "Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBSOversea/${json_data['HoYoLAB_ua_version']}",
      'Referer': "https://act.hoyolab.com/",
      'Cookie': HoYoLAB_cookie,
    };
    const body = {'act_id': 'e202102251931481'};
    await sign(url, header, body);
  }
  print('签到完成');
  sleep(Duration(seconds: 3));
}

Future<void> sign(Uri url, Map<String, String> header, Map<String,String> body) async {
  late http.Response response;
  try {
    response = await http
        .post(url, headers: header, body: jsonEncode(body))
        .timeout(
          Duration(seconds: 3),
          onTimeout: () => throw Exception('请求超时'),
        );
  } catch (e) {
    print('请求失败$e');
    return;
  }
  if (response.statusCode == 200) {
    var data = jsonDecode(response.body) as Map<String, dynamic>;
    if (data[r'retcode'] == 0) {
      print(data);
    } else {
      print('请求失败${response.body}');
    }
  } else {
    print('请求失败${response.statusCode}');
  }
}

/// 可查询米游社签到状态
Future<dynamic> checkSign(String cookie) async {
  final Uri uri = Uri.https(r'sg-hk4e-api.hoyolab.com', 'event/sol/info', {
    'lang': 'zh-cn',
    'act_id': 'e202102251931481',
  });
  Map<String, String>? head = {'cookie': cookie};
  http.Response response = await http.get(uri, headers: head);
  if (response.statusCode == 200) {
    var data = jsonDecode(response.body) as Map<String, dynamic>;
    if (data[r'retcode'] == 0) {
      print(data);
      return data['data']['is_sign'];
    } else {
      print('请求失败${response.body}');
    }
  } else {
    print('请求失败${response.body}');
  }
}

Random random = Random();
const String salt = r't0qEgfub6cvueAPgR5m9aQWWVciEer7v';

/// 获取DS算法
Future<String> getDS(Map body) async {
  final String t = (DateTime.now().millisecondsSinceEpoch / 1000)
      .toInt()
      .toString();
  final String r = (100000 + random.nextInt(100000)).toString();

  String temp_body = jsonEncode(_sortMapByKeys(body));
  String main = 'salt=$salt&t=$t&r=$r&b=$body';
  String ds = getMD5(main);
  return "$t,$r,$ds";
}

/// 进行md5加密
String getMD5(String str) {
  var content = Utf8Encoder().convert(str);
  return md5.convert(content).toString();
}

// Map根据键的字母顺序排序
Map<String, dynamic> _sortMapByKeys(Map<String, dynamic> map) {
  var sortedKeys = map.keys.toList()..sort();
  return Map.fromEntries(
    sortedKeys.map((key) => MapEntry(key, map[key]))
  );
}