import com.alibaba.fastjson2.JSON;
import com.alibaba.fastjson2.JSONObject;
import com.alibaba.fastjson2.TypeReference;

import java.io.File;
import java.io.IOException;
import java.math.BigInteger;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.time.Instant;
import java.util.Map;
import java.util.Objects;
import java.util.Random;
import java.util.TreeMap;
import java.util.concurrent.CompletableFuture;

public class Main {
    static HttpClient client = HttpClient.newHttpClient();

    public static void main(String [] args) throws IOException {
        URI url;
        Map<String,String> headers;
        String body;
        String temp = Files.readString(Paths.get(new File(System.getProperty("user.dir")).getParent()).resolve("data.json"));
        Map<String, Object> map = JSONObject.parseObject(temp, new TypeReference<Map<String, Object>>() {});
        getDS(map);
        if (!Objects.equals(map.get("miHoYo_uid"),"") && !Objects.equals(map.get("miHoYo_cookie"),"")){
            System.out.println("开始米游社签到");
            url = URI.create("https://api-takumi.mihoyo.com/event/luna/hk4e/sign");
            headers = Map.of(
                    "User-Agent", "Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBS/" + map.get("miHoYo_ua_version"),
                    "x-rpc-signgame", "hk4e",
                    "Referer", "https://act.mihoyo.com",
                    "Cookie", map.get("miHoYo_cookie").toString()
            );
            body = String.format("""
            {
                "act_id": "e202311201442471",
                "region": "cn_gf01",
                "uid": "%s",
                "lang": "zh-cn"
            }
            """, map.get("miHoYo_uid"));
            sign(url,headers,body);
        }

        if (!Objects.equals(map.get("HoYoLAB_cookie"),"")){
            System.out.println("开始HoYoLAB签到");
            url = URI.create("https://sg-hk4e-api.hoyolab.com/event/sol/sign?lang=zh-cn");
            headers = Map.of(
                    "User-Agent","Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBSOversea/" + map.get("HoYoLAB_ua_version"),
                    "Referer","https://act.hoyolab.com",
                    "Cookie",map.get("HoYoLAB_cookie").toString()
            );
            body = "{\"act_id\": \"e202102251931481\"}";
            sign(url,headers,body);
        }

        System.out.println("签到完成");
    }

    private static void sign(URI url,Map<String,String> headers,String body)  {
        try {
            HttpRequest.Builder requestBuilder = HttpRequest.newBuilder().uri(url);

            headers.forEach(requestBuilder::header);

            HttpRequest request = requestBuilder
                    .POST(HttpRequest.BodyPublishers.ofString(body))
                    .build();
        // 发送异步请求
        CompletableFuture<HttpResponse<String>> future =
                client.sendAsync(request, HttpResponse.BodyHandlers.ofString());

        // 处理响应
            future.thenApply(HttpResponse::body)
                    .thenAccept(response -> {
                        System.out.println("响应结果: " + response);
                    })
                    .join();

        } catch (Exception e){
            e.printStackTrace();
        }
    }

    private static String getDS(Map<String, Object> body){
        final String salt = "t0qEgfub6cvueAPgR5m9aQWWVciEer7v";
        long t = Instant.now().getEpochSecond();

        Random random = new Random();
        int r = random.nextInt(100001) + 100000;
        if (r == 100000){
            r = 642367;
        }
        String temp_body = JSON.toJSONString(new TreeMap<>(body));
        String main = String.format("salt=%s&t=%d&r=%d&b=%s",salt,t,r,temp_body);
        String ds = md5(main);
        return String.format("%d,%d,%s",t,r,ds);
    }

    // 将字符串转MD5加密
    private static String md5(String str) {
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            md.update(str.getBytes());// 计算md5函数
            String hashedPwd = new BigInteger(1, md.digest()).toString(16);// 16是表示转换为16进制数
            return hashedPwd;
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return null;
    }
}
