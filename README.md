# smartIR

「スマートリモコンを作ろう」題材のCH552マイコンボードを使用した学習リモコンです。廉価な材料で電子工作やIoTを体験することができます。

## Web アプリ

遠隔制御を体験するためのWebアプリはこちらからアクセスしてください。

### [🌉smartir-webbridge](https://iceetrdi.github.io/smartir-webbridge/)

赤外線送受信器を、インターネット上から制御できるようにするためのアプリです。インターネット接続と赤外線送受信器をUSB接続しているPCで、このアプリを実行してください。

### [📡smartir-webclient](https://iceetrdi.github.io/smartir-webclient/)

インターネット経由で赤外線信号のやりとりをするためのアプリです。インターネット接続されたPCやスマホ等で実行してください。

## 使用材料

### マイコンボード

[WeActStudio.CH552CoreBoard](https://github.com/WeActStudio/WeActStudio.CH552CoreBoard)

## 開発環境

- [Arduino IDE](https://www.arduino.cc/en/software)
- [ch55xduino](https://github.com/DeqingSun/ch55xduino)

### 開発環境構築方法

1. WCH社Webサイトからドライバ [CH372DRV.EXE](https://www.wch-ic.com/downloads/CH372DRV_EXE.html) を入手し、インストールしてください（※）
2. Arduino IDE を入手し、インストールしてください
3. Arduino IDE を起動し、基本設定 ( Ctrl + , ) の「追加のボードマネージャのURL」にch55xduinoを入手するためのURL ( https://raw.githubusercontent.com/DeqingSun/ch55xduino/ch55xduino/package_ch55xduino_mcs51_index.json ) を入力してください
4. Arduino IDE のボードマネージャから`CH55xDuino MCS51 plain C core (non-C++)`をインストールしてください
5. ボードは、`CH55xDuino MCS51 plain C core (non-C++)` から `CH552 Board` を選択してください

※ このドライバがないとプログラム書き込みに失敗します。代わりに[WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html)をインストールしても動作します。

## ご注意

- このキットは、全てのリモコンをエミュレートできるわけではありません。リモコンの種類によっては正常に送受信できないケースがあります。
  - 技術的にはNECフォーマットとAEHAフォーマットの一部に対応しています。([参考: 赤外線リモコンの通信フォーマット](http://elm-chan.org/docs/ir_format.html))
- ネットワーク環境によっては、MQTT (MQTT over WebSocket Secure, TCP 8084) 接続ができない場合があります。
  - 特に、学校や公共施設等のネットワークでは使用できない場合があります。
- あくまで体験用キットですので、実使用に対するテストはしておりません。もし活用する場合はご自身の判断と責任の下ご使用ください。
- 本キットにおける故障やその他損害につきましては責任を負いかねますので、ご注意の上作業をしてください。

## 実施イベント

この教材を使用したイベントなど

2024.08 2024年 第6回ものづくりチャレンジ大作戦 ( 主催: [一般社団法人ものづくり文化振興協会](https://sites.google.com/view/monodukuri-bunka/) )
