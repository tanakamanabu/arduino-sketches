# 概要
　PCを監視し、フリーズしていると判断したらリセットを行う

# ハードウエア構成

 1. M5 Atom Liteを使用
 2. M5Stack用ミニリレーユニット [U023]をGroveケーブルでM5と接続
 3. リレーの「NO」と「COM」をPCのリセットスイッチとGNDへ繋ぐ

# ステータス

 - 赤 停止中。ボタンを押すと緑になる
 - 緑 Wi-Fi検索中。発見したら消灯する
 - 消灯 監視を行っている。条件をみたしたら白になる
 - 白 PCがフリーズしている可能性が高いのでリセットスイッチ操作中。完了後青になる
 - 青 リセット操作後の再起動待ち。一定時間経過後に消灯になる
 
# ボタン
　赤状態で押下するとWifiの検索をはじめる。それ以外の状態で押下すると監視を中断し、赤へ遷移する。
