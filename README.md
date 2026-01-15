# Arduino 輪型機器人實作 (Arduino Wheeled Robot Workshop)

## 關於本課程 (Course Overview)
本課程旨在帶領學員認識「輪型機器人」的基礎原理與控制技術。透過 Arduino 開發板，我們將從最底層的馬達驅動開始，一步步賦予機器人感知環境（感測器）與決策思考（演算法）的能力。

## 什麼是機器人？ (What is a Robot?)
一個完整的機器人通常具備三個核心循環 (Sense-Think-Act)，本課程將逐一實作：
1.  **感測 (Sense)**：感知外界環境的能力。
    *   *本課程使用*：紅外線接收器 (接收指令)、紅外線感測器 (近距離偵測)、超音波感測器 (測量距離)。
2.  **運算 (Think)**：接收感測數據，依據演算法做出決策。
    *   *本課程使用*：Arduino 微控制器。學習核心邏輯如：狀態機 (State Machine)、邏輯判斷 (If-Else)、回授控制 (Feedback Control)。
3.  **動作 (Act)**：執行決策，對物理世界產生作用。
    *   *本課程使用*：L298N 馬達驅動模組 (控制輪子轉動)、蜂鳴器 (發出聲音訊號)。

## 學習目標 (Learning Outcomes)
完成本課程後，您將掌握：
*   **嵌入式系統基礎**：理解 GPIO、PWM、中斷 (Interrupt) 與硬體通訊原理。
*   **控制理論入門**：
    *   **開迴路控制 (Open-Loop)**：學習如何克服物理慣性與硬體誤差 (Hardware Bias)。
    *   **閉迴路控制 (Closed-Loop)**：學習利用感測器回授來修正行為 (如 P-Control)。
*   **軟體工程思維**：
    *   **模組化設計**：學習撰寫 `.h` 與 `.cpp` 函式庫，管理複雜程式碼。
    *   **非阻塞多工 (Non-blocking Multitasking)**：脫離 `delay()`，學習寫出能同時處理多項任務的專業程式。

---

## 程式列表與教學流程 (Teaching Flow)

本課程分為五大階段，建議依序學習。

### 第一階段：基礎控制 (Foundation: Hardware & Open-Loop)
*目標：熟悉硬體特性，掌握開迴路控制技巧。*

1.  **基礎測試**
    *   **檔案**：[`car/car.ino`](car/car.ino)
    *   **內容**：最基本的馬達控制測試，確保線路連接正確。
2.  **馬達控制 (Motor Control)**
    *   **檔案**：[`straight_line_calibration/straight_line_calibration.ino`](straight_line_calibration/straight_line_calibration.ino) - **[重要工具]** 直線校正。找出兩個修正左右輪誤差的參數。
    *   **檔案**：[`accelerating_demo/accelerating_demo.ino`](accelerating_demo/accelerating_demo.ino) - 線性加速（PWM、啟動死區、斜坡控制）。
    *   **檔案**：[`square_path_demo/square_path_demo.ino`](square_path_demo/square_path_demo.ino) - 畫正方形（基礎時序控制）。
    *   **檔案**：[`star_path_demo/star_path_demo.ino`](star_path_demo/star_path_demo.ino) - 畫五角星（幾何角度計算）。
    *   **檔案**：[`figure_8_demo/figure_8_demo.ino`](figure_8_demo/figure_8_demo.ino) - 8字型繞行（差速轉向 Differential Steering）。
    *   **檔案**：[`shuttle_run_challenge/shuttle_run_challenge.ino`](shuttle_run_challenge/shuttle_run_challenge.ino) - **[階段驗收]** 折返跑挑戰（綜合應用）。

### 第二階段：互動與資料結構 (Interaction & Data)
*目標：增加人機互動，並學習如何在 Arduino 中儲存與回放資料。*

3.  **音頻原理 (Audio Basics)**
    *   **檔案**：[`manual_tone_song/manual_tone_song.ino`](manual_tone_song/manual_tone_song.ino)
    *   **內容**：手寫頻率產生器，理解聲音震動原理，演奏簡單歌曲。
4.  **手動互動 (Manual Interaction)**
    *   **檔案**：[`manual_sound_car/manual_sound_car.ino`](manual_sound_car/manual_sound_car.ino)
    *   **內容**：全功能遙控車，結合按鍵判斷與音效回饋。
5.  **遙控電子琴 (Remote Piano)**
    *   **檔案**：[`remote_piano/remote_piano.ino`](remote_piano/remote_piano.ino)
    *   **內容**：將遙控器變身為電子琴。按數字鍵 0-8 彈奏不同音階，練習「小蜜蜂」等兒歌。
6.  **記憶電子琴 (Memory Piano)**
    *   **檔案**：[`memory_piano/memory_piano.ino`](memory_piano/memory_piano.ino)
    *   **內容**：**[過渡單元]** 在學習記錄複雜的車子動作之前，先練習記錄簡單的「一維陣列」音符。彈一段旋律，按下 OK 鍵，Arduino 就會自動重播。
7.  **機器人記憶 (Robot Memory)**
    *   **檔案**：[`memory_path_recorder/memory_path_recorder.ino`](memory_path_recorder/memory_path_recorder.ino)
    *   **內容**：**[核心教學]** 實作「教導模式」。學習使用 `struct` 與 `Array` 記錄動作序列並重播。

### 第三階段：感測與邏輯 (Sensing & Logic)
*目標：讓機器人學會「看」環境，並做出簡單決策。*

8.  **資料視覺化 (Visualization)**
    *   **檔案**：[`serial_plotter_dashboard/serial_plotter_dashboard.ino`](serial_plotter_dashboard/serial_plotter_dashboard.ino)
    *   **內容**：使用 Serial Plotter 觀察感測器數值，這是除錯的神器。
9.  **基礎避障 (Simple Avoidance)**
    *   **檔案**：[`simple_avoidance/simple_avoidance.ino`](simple_avoidance/simple_avoidance.ino)
    *   **內容**：最單純的「感測 -> 判斷 -> 動作」邏輯 (Blocking)。
10. **跟隨邏輯 (Follow Me)**
    *   **檔案**：[`follow_me_car/follow_me_car.ino`](follow_me_car/follow_me_car.ino)
    *   **內容**：學習 P-Control 概念，保持固定距離。
11. **循線自駕 (Line Following)**
    *   **檔案**：[`line_following_car/line_following_car.ino`](line_following_car/line_following_car.ino)
    *   **內容**：利用紅外線循線模組，讓車子沿著黑線行駛。

### 第四階段：軟體架構 (Software Architecture)
*目標：寫出專業、高效、可擴充的程式碼。*

12. **函式庫封裝 (Refactoring)**
    *   **檔案**：`SimpleIR.h`, `SimpleIR.cpp`, [`library_tutorial/library_tutorial.ino`](library_tutorial/library_tutorial.ino)
    *   **內容**：學習如何將雜亂的程式碼打包成 Library (`.h`/`.cpp`)。
13. **非阻塞多工 (Multitasking)**
    *   **檔案**：[`non_blocking_tone_tutorial/non_blocking_tone_tutorial.ino`](non_blocking_tone_tutorial/non_blocking_tone_tutorial.ino)
    *   **內容**：**[核心教學]** 拋棄 `delay()`。使用 `millis()` 與狀態機 (State Machine) 讓車子同時做多件事。
14. **非阻塞感測 (Non-blocking Sensing)**
    *   **檔案**：[`radar_car/radar_car.ino`](radar_car/radar_car.ino)
    *   **內容**：應用非阻塞技術，實作倒車雷達音效。
15. **狀態機應用 (State Machines)**
    *   **檔案**：[`ir_auto_sound_car/ir_auto_sound_car.ino`](ir_auto_sound_car/ir_auto_sound_car.ino)
    *   **內容**：多感測器融合 (Sensor Fusion) 與複雜狀態管理。

### 第五階段：專案整合 (Final Integration)
*目標：打造最終的智慧車系統。*

16. **模式切換架構 (Mode Switching)**
    *   **檔案**：[`mode_switch_concept/mode_switch_concept.ino`](mode_switch_concept/mode_switch_concept.ino)
    *   **內容**：學習利用「狀態變數」在不同功能模式間切換。
17. **最終專案 (Final Project)**
    *   **檔案**：[`smart_car_dual_mode/smart_car_dual_mode.ino`](smart_car_dual_mode/smart_car_dual_mode.ino)
    *   **內容**：整合遙控、避障、音效與模式切換的完整系統。

### 附錄：延伸實驗
*   **檔案**：`dual_buzzer_harmony.ino` - 雙蜂鳴器和弦實驗（硬體極限挑戰）。

---

## 硬體接腳定義 (Pin Definitions)

| 模組 | 功能 | Arduino 接腳 |
| :--- | :--- | :--- |
| **馬達驅動** | 左輪控制 | D2, D3 |
| | 右輪控制 | D4, D5 |
| **感測器** | 左側紅外線 (IR Left) | D6 |
| | 右側紅外線 (IR Right) | D7 |
| | 超音波 Trig | D8 |
| | 超音波 Echo | D9 |
| **人機介面** | 紅外線接收器 (IR Recv) | D10 |
| | 主蜂鳴器 (Buzzer) | D11 |
| | (選配) 副蜂鳴器 | D12 (僅用於 dual_buzzer_harmony) |

## 如何開始

1.  確認硬體接線無誤。  
2.  從 **1. 基礎測試** 開始，將對應的 `.ino` 檔上傳至 Arduino。
3.  開啟 Serial Monitor (115200) 觀察輸出。
