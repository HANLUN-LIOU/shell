# A Simple Linux Shell

## 基本功能

1. **印出 prompt 並可讓使用者輸入 command**
    - prompt 需印出 `"<當前使用者>@<當前主機名稱>:<當前路徑>$"` 
    - 路徑初始值為 `~`，也就是 `/home/<當前使用者>/`
    - 範例如同 terminal:
      
      ```bash
      purplered@PR-UbuntuVM:~$
      ```

2. **接受並執行 command**
    - **內建命令**: `export`, `echo`, `pwd`, `cd`
    - **外部命令**: fork a child process，再用 exec-like 函式執行外部 command，最後輸出執行結果
    - 輸入 `exit` 結束程式

3. **command 執行結束後，再回到 prompt**

---

## 進階功能 

1. **支援背景執行 (background execution `&`) **

2. **支援輸出重定向 (output redirection `>`) **

3. **查詢歷史 command **
    - 按 `上/下` 鍵時，印出 prompt 及上/下一筆歷史 command
    - 歷史 command 印出後，按 `enter` 鍵可執行該歷史 command

