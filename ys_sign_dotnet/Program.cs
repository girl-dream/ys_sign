using System.Security.Cryptography;
using System.Text;
using System.Text.Json;

namespace ys_sign_dotnet
{
    public class Ys_sign
    {
        private static readonly HttpClient client = new HttpClient();
        private static readonly Random _random = new Random();

        public static async Task Main(String[] args)
        {
            DirectoryInfo tempPath = new DirectoryInfo(Directory.GetCurrentDirectory());
            string path = Path.Combine(tempPath?.Parent?.Parent?.Parent?.Parent?.FullName ?? throw new InvalidOperationException("路径为空"), "data.json");

            string json = File.ReadAllText(path);
            Dictionary<string, string> dict = JsonSerializer.Deserialize<Dictionary<string, string>>(json)!;
            dynamic data;
            string url;
            HttpContent body;
            Dictionary<string, string> headers = new Dictionary<string, string>();

            client.Timeout = TimeSpan.FromSeconds(3);

            if (dict!["miHoYo_uid"] != "" && dict!["miHoYo_uid"] != "")
            {
                Console.WriteLine("开始米游社签到");
                url = "https://api-takumi.mihoyo.com/event/luna/hk4e/sign";
                data = new
                {
                    act_id = "e202311201442471",
                    region = "cn_gf01",
                    uid = dict["miHoYo_uid"],
                    lang = "zh-cn"
                };
                body = new StringContent(JsonSerializer.Serialize(data), Encoding.UTF8, "application/json");

                headers.Add("User-Agent", $"Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBS/{dict!["miHoYo_ua_version"]}");
                headers.Add("x-rpc-signgame", "hk4e");
                headers.Add("Referer", "https://act.mihoyo.com");
                headers.Add("Cookie", dict["miHoYo_cookie"]);
                await Sign(url, body, headers);
            }

            if (dict!["HoYoLAB_cookie"] != "")
            {
                Console.WriteLine("开始HoYoLAB签到");
                url = "https://sg-hk4e-api.hoyolab.com/event/sol/sign?lang=zh-cn";
                data = new
                {
                    act_id = "e202102251931481"
                };
                body = new StringContent(JsonSerializer.Serialize(data), Encoding.UTF8, "application/json");

                headers.Clear();
                headers.Add("User-Agent", $"Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBSOversea/${dict["HoYoLAB_ua_version"]}");
                headers.Add("Referer", "https://act.hoyolab.com");
                headers.Add("Cookie", dict["HoYoLAB_cookie"]);

                await Sign(url, body, headers);
            }
            Console.WriteLine("签到完成");
            Thread.Sleep(2000);
        }

        private static async Task Sign(string url, HttpContent body, Dictionary<string, string> headers)
        {

            HttpRequestMessage request = new HttpRequestMessage
            {
                Method = HttpMethod.Post,
                RequestUri = new Uri(url),
                Content = body
            };

            foreach (var header in headers)
            {
                request.Headers.Add(header.Key, header.Value);
            }
            HttpResponseMessage response;
            try
            {
                response = await client.SendAsync(request);
            }
            catch (Exception error)
            {
                Console.WriteLine($"请求失败{error}");
                return;
            }
            if (response.IsSuccessStatusCode)
            {
                string res = await response.Content.ReadAsStringAsync();
                Console.WriteLine($"签到成功: {res}");
            }
            else
            {
                Console.WriteLine($"签到失败: {response.StatusCode}");
            }
        }


        private static string getDS(Dictionary<string, dynamic> body)
        {
            const string salt = "t0qEgfub6cvueAPgR5m9aQWWVciEer7v";
            long t = DateTimeOffset.Now.ToUnixTimeSeconds();
            int r = _random.Next(100000, 200001);
            if (r == 100000)
            {
                r = 642367;
            }
            var temp_body = new SortedDictionary<string, dynamic>(body);
            string result = JsonSerializer.Serialize(temp_body);
            string main = $"salt={salt}&t={t}&r={r}&b={result}";
            string ds = md5(main);
            return $"{t},{r},{ds}";
        }

        // 获取字符串的md5
        private static string md5(string input)
        {
            var inputBytes = Encoding.UTF8.GetBytes(input);
            var hashBytes = MD5.HashData(inputBytes);
            return Convert.ToHexString(hashBytes);
        }
    }
}