# Spotřeba energie – bakalářský projekt

Tento projekt slouží k **měření, analýze a vizualizaci spotřeby elektrické energie v domácnosti**.  
Systém je založen na **ESP8266**, které komunikuje s elektroměrem přes **Modbus RTU**. Data se odesílají přes Wi-Fi na **Raspberry Pi**, kde jsou ukládána do databáze **MariaDB** a dále vizualizována přes webovou aplikaci postavenou na **Vue.js**.

## 📁 Struktura projektu

Projekt obsahuje následující části:

- `backend/` – REST API pro komunikaci s databází (např. Spring Boot)
- `src/` – Frontendová část aplikace postavená na Vue.js
- `package.json` – Konfigurace a seznam závislostí frontendové aplikace
- `vite.config.js` – Konfigurace vývojového prostředí
- `index.html` – Výchozí HTML soubor

## 🚀 Jak spustit frontend (Vue.js)

1. Otevři terminál ve složce projektu.
2. Instaluj závislosti:
   ```
   npm install
   ```
3. Spusť vývojový server:
   ```
   npm run dev
   ```
4. Aplikace poběží na adrese: `http://localhost:5173`

## 🖥️ Backend API

Backendová část slouží pro:

- ukládání a načítání dat z databáze (MariaDB),
- poskytování REST API endpointů pro frontend,
- případné napojení na další logiku zpracování.

> Backend je potřeba rozbalit ze souboru `backend.zip`.

🔌 Arduino (ESP8266) firmware
Ve složce arduino/ se nachází firmware pro mikrokontrolér ESP8266 (WeMos D1 R2), který komunikuje s elektroměrem pomocí RS485 a Modbus RTU.

Tento program každou minutu načte z elektroměru hodnoty:

napětí (V) – registr 0x0000

proud (A) – registr 0x0006

výkon (W) – registr 0x000C

spotřeba (kWh) – registr 0x0156

Naměřená data odešle jako JSON přes HTTP POST na REST API (/prijem.php nebo jiný endpoint na backendu).

🛠 Funkce:

komunikace přes knihovnu ModbusMaster

řízení směru přenosu přes pin D0 (DE/RE)

odesílání JSONu přes WiFi (ESP8266HTTPClient)

jednoduchá integrace s backendem nebo databází

## ⚙️ Použité technologie

- **ESP8266** – mikrokontrolér pro sběr dat z elektroměru
- **Modbus RTU** – komunikační protokol s elektroměrem
- **MariaDB** – databáze pro ukládání měření
- **Raspberry Pi** – běh backendu a databáze
- **Vue.js + Vite** – moderní frontend framework a dev server
- **Chart.js** – vizualizace dat v grafech
- **Node.js / Java Spring Boot** – backendové technologie

## 📈 Funkce systému

- Reálný sběr dat o napětí, proudu, výkonu a spotřebě
- Zobrazení aktuálních i historických údajů pomocí grafů
- Automatické notifikace (např. při vysokém výkonu nebo proudu)
- Export do CSV souboru
- Možnost zadávání uživatelských limitů
- Přehled za posledních 24 hodin

## 🧪 Testováno na

- ESP8266 (WeMos D1 R2)
- Elektroměr DTS353F-2 (Modbus RTU)
- Raspberry Pi 4
- MariaDB 10.x
- Node.js 18+
- npm 9+

## 🧾 Licence

Tento projekt byl vytvořen jako součást bakalářské práce a je určen pro výukové a nekomerční účely.
