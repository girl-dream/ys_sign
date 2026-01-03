package main

import (
	"bytes"
	"crypto/md5"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"math/rand"
	"net/http"
	"os"
	"path/filepath"
	"time"
)

type Config struct {
	MiHoYoUID        string `json:"miHoYo_uid"`
	MiHoYoCookie     string `json:"miHoYo_cookie"`
	MiHoYoUAVersion  string `json:"miHoYo_ua_version"`
	HoYoLABCookie    string `json:"HoYoLAB_cookie"`
	HoYoLABUAVersion string `json:"HoYoLAB_ua_version"`
}

func main() {
	tempPath, _ := os.Getwd()
	path := filepath.Dir(tempPath)
	file, _ := os.Open(filepath.Join(path, "data.json"))
	var config Config
	if err := json.NewDecoder(file).Decode(&config); err != nil {
		log.Fatal("解析 JSON 失败:", err)
	}
	defer file.Close()
	if config.MiHoYoCookie != "" && config.MiHoYoUID != "" {
		fmt.Println("开始米游社签到")
		url := "https://api-takumi.mihoyo.com/event/luna/hk4e/sign"
		headers := map[string]string{
			"User-Agent":     fmt.Sprintf("Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBS/%s", config.MiHoYoUAVersion),
			"x-rpc-signgame": "hk4e",
			"Referer":        "https://act.mihoyo.com/",
			"Cookie":         config.MiHoYoCookie,
		}
		body := map[string]string{
			"act_id": "e202311201442471",
			"region": "cn_gf01",
			"uid":    config.MiHoYoUID,
			"lang":   "zh-cn",
		}
		sign(url, headers, body)
	}
	if config.HoYoLABCookie != "" {
		fmt.Println("开始HoYoLAB签到")
		url := "https://sg-hk4e-api.hoyolab.com/event/sol/sign?lang=zh-cn"
		headers := map[string]string{
			"User-Agent": fmt.Sprintf("Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBSOversea/%s", config.HoYoLABUAVersion),
			"Referer":    "https://act.hoyolab.com",
			"Cookie":     config.HoYoLABCookie,
		}
		body := map[string]string{"act_id": "e202102251931481"}
		sign(url, headers, body)
	}
	fmt.Println("签到完成,3秒后退出程序")
	time.Sleep(3 * time.Second)
}

func sign(url string, header map[string]string, body map[string]string) {
	jsonData, _ := json.Marshal(body)
	req, _ := http.NewRequest("POST", url, bytes.NewBuffer(jsonData))
	for key, value := range header {
		req.Header.Set(key, value)
	}
	client := &http.Client{Timeout: 5 * time.Second}
	res, err := client.Do(req)
	if err != nil {
		fmt.Printf("请求执行失败: %v\n", err)
		return
	}
	defer res.Body.Close()
	resBody, err := io.ReadAll(res.Body)
	if err != nil {
		fmt.Printf("读取响应失败: %v\n", err)
		return
	}
	fmt.Println("签到成功")
	fmt.Println(string(resBody))
}

func getDS(body map[string]string) string {
	const salt = "t0qEgfub6cvueAPgR5m9aQWWVciEer7v"
	t := time.Now().Unix()

	random := rand.New(rand.NewSource(time.Now().UnixNano()))
	r := random.Intn(100000) + 100000
	if r == 100000 {
		r = 642367
	}

	jsonBytes, err := json.Marshal(body)
	if err != nil {
		panic(err)
	}

	main := fmt.Sprintf("salt=%s&t=%d&r=%d&b=%s", salt, t, r, string(jsonBytes))
	ds := md5.Sum([]byte(main))
	return fmt.Sprintf("%d,%d,%s", t, r, ds)
}
