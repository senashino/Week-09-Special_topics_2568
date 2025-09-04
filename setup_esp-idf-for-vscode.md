# การใช้งาน esp-idf ใน vscode
1. เปิด vs code
2. คลิกแท็บ Extension
3. ค้นหา IDF
4. เลือก ESP-IDF
5. กด Install
6. กด Advanced
7. คลิกเลือก Configure ESP-IDF Extension
8. เลือก ESPRESSIF
9. เลือกเวอร์ชั่นล่าสุด
10. กด Install
11. รอเวลาติดตั้ง (ที่นานมากกกก)

# วิธีการเรียกใช้
1. Ctrl+Shift+P หรือ F1
2. พิมพ์ idf terminal
3. เลือก ESP-IDF:Open ESP-IDF Terminal
4. โปรแกรมจะเรียก ESP-IDF Terminal
5. ใช้คำสั่ง idf.py
6. ต้องขึ้นส่วน Options และ Commands
7. พิมพ์คำสั่ง idf.py create-project HelloWorld
8. พิมพ์คำสั่ง dir
9. พิมพ์คำสั่ง cd HelloWorld
10. พิมพ์คำสั่ง idf.py build
11. พิมพ์คำสั่ง idf.py เพื่อดูขนาดไซส์
12. พิมพ์คำสั่ง idf.py flash
13. พิมพ์คำสั่ง idf.py monitor (กำหนดพอร์ตเองใช้คำสั่ง idf.py -p PORT flash)
