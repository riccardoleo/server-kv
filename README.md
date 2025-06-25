# server-kv

`server-kv` è un semplice **server key–value TCP in C++**, ispirato al modello di comunicazione di Redis, ma con una struttura personalizzata. Utilizza socket POSIX, parsing manuale e un backend SQLite per la persistenza.

## 🌟 Caratteristiche

- Supporto ai comandi: `SET:key:value`, `GET:key`, `DEL:key`
- Connessioni TCP multiple (blocking)
- Parsing del protocollo custom (basato su `\r`)
- Backend persistente con SQLite
- Gestione di buffer limitati e controllo degli errori
- Statistiche di messaggi corretti e scartati

## 📝 Requisiti

- Sistema UNIX-like (Linux o macOS)
- `g++` o `clang++`
- `sqlite3` (dev headers installati)

## 🛠️ Build

### Clona il repository

```bash
git clone https://github.com/riccardoleo/server-kv.git
cd server-kv
