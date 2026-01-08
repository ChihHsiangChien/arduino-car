# Arduino 小車教學 (Arduino Smart Car Tutorial)

本專案包含一系列循序漸進的 Arduino 程式碼，旨在引導學習者從基礎的硬體控制，進階到結合感測器、音效與自動避障邏輯的整合應用。

## 程式列表與教學流程 (Teaching Flow)

請依照以下順序學習與測試，掌握各個核心概念：

### 1. 基礎測試
*   **檔案**：`car.ino`
*   **內容**：最基本的馬達控制與紅外線接收測試，確保所有線路連接正確。

### 2. 馬達控制初階 (Basic Motor Control)
*   **檔案**：`square_path_demo.ino`
*   **內容**：嘗試讓車子走出完美的正方形。你需要手動調整轉彎的 delay 時間，體會沒有感測器時控制的困難。
*   **檔案**：`accelerating_demo.ino`
*   **內容**：車子平滑地加速起步與減速煞車，學習克服馬達的啟動死區 (Dead Zone)。

### 3. 手動互動與聲音 (Interaction & Sound)
*   **檔案**：`manual_sound_car.ino`
*   **內容**：
    *   **全手動控制**：使用紅外線遙控器操作車子。
    *   **按鍵音效**：每個動作（前進、轉彎、煞車）都有對應的提示音。
    *   **喇叭功能**：按 EQ 鍵可發出喇叭聲。

### 4. 資料視覺化 (Data Visualization)
*   **檔案**：`serial_plotter_dashboard.ino`
*   **內容**：將感測器數據轉化為即時波形圖。這是工程師除錯與調整參數的重要技巧。

### 5. 基礎避障 (Simple Avoidance)
*   **檔案**：`simple_avoidance.ino`
*   **目標**：學習自動控制最核心的邏輯：「感測 -> 判斷 -> 動作」。
*   **內容**：最單純的自駕車程式。利用超音波偵測距離，遇障礙則後退轉彎。使用 `delay()` (阻塞) 寫法，邏輯直觀好懂。

### 6. 跟隨邏輯 (Follow Me Logic)
*   **檔案**：`follow_me_car.ino`
*   **目標**：學習「回授控制」與 P-Control 概念。
*   **內容**：與避障相反的邏輯。車子會嘗試與前方物體保持固定距離（太近後退，太遠前進）。

### 7. 進階觀念：函式庫封裝 (C++ Library Creation)
*   **檔案**：`SimpleIR.h`, `SimpleIR.cpp`, `library_tutorial.ino`
*   **目標**：將IR的程式碼打包成 `.h` 與 `.cpp` 檔。
*   **內容**：將 `car.ino` 中複雜的手寫紅外線解碼邏輯抽離出來，讓主程式只剩下乾淨的 `ir.check()` 與 `ir.getResult()`。

### 8. 進階觀念：非阻塞程式設計 (Non-blocking Code)
*   **檔案**：`non_blocking_tone_tutorial.ino`
*   **目標**：**[核心教學]** 理解為什麼不能用 `delay()`。
*   **內容**：一個純教學範例。展示如何使用 `millis()` 和狀態機 (State Machine) 來同時播放背景音樂並閃爍 LED，證明 CPU 沒有被卡住。

### 9. 感測器與回饋 (Sensors & Feedback)
*   **檔案**：`radar_car.ino`
*   **目標**：應用非阻塞技術。
*   **內容**：
    *   **自動駕駛**：不需要遙控器，車子自動前進。
    *   **倒車雷達音效**：利用剛剛學到的 `millis()` 技術，實作距離越近、嗶聲越急促的效果。
    *   **隨機避障**：遇到障礙物時會隨機選擇左轉或右轉。
### 10. 綜合應用 (Integration)
*   **檔案**：`ir_auto_sound_car.ino`
*   **內容**：
    *   結合 **左右紅外線感測器** 進行避障。
    *   **同步音效**：利用上述的「非阻塞」技術，讓車子在自動避障的同時，發出代表不同狀態的音效（心跳聲、轉彎聲、倒車警報）。

### 11. 聲音實驗 (Audio Experiment)
*   **檔案**：`dual_buzzer_harmony.ino`
*   **內容**：使用兩個蜂鳴器（硬體 Tone + 軟體模擬 Tone）同時發聲，產生 C 大調和弦效果。


### 12. 模式切換概念 (Mode Switch Concept)
*   **檔案**：`mode_switch_concept.ino`
*   **內容**：簡化複雜的避障與音效，專注於「如何寫出可以切換模式的程式架構」。學會如何用變數 (State Variable) 來控制程式的流向。

### 13. 完整版與模式切換 (Final Project with Mode Switching)
*   **檔案**：`smart_car_dual_mode.ino`
*   **內容**：
    *   整合 **手動遙控** 與 **自動避障** 兩大功能。
    *   支援遙控器 **一鍵切換模式** (EQ鍵)。
    *   包含完整的避障邏輯（前方超音波 + 左右紅外線）。

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
2.  (選用) 若執行舊版程式需安裝 `IRremote` 函式庫，但基礎測試 `car.ino` 已改寫為免函式庫版本。
3.  從 **1. 基礎測試** 開始，將對應的 `.ino` 檔上傳至 Arduino。
4.  開啟 Serial Monitor (115200) 觀察輸出。
