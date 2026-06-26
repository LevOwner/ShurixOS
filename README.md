# **ВНИМАНИЕ ПРОЕКТ НЕ ДОДЕЛАН ДО КОНЦА ПОЖАЙЛУСТА СЛЕДИТЕ ЗАКОМИТАМИ**
# ⚡ ShurixOS

> **Гипервизор, который запускает любую ОС без тормозов**

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GitHub release](https://img.shields.io/badge/release-v0.1-orange)](https://github.com/LevOwner/ShurixOS/releases)
[![GitHub stars](https://img.shields.io/github/stars/LevOwner/ShurixOS?style=social)](https://github.com/LevOwner/ShurixOS/stargazers)

---

## 🧠 О проекте

**ShurixOS** — это лёгкая 32-битная ОС-гипервизор, созданная с нуля.  
Она позволяет **запускать Windows, Linux и любые другие ОС в контейнерах** — без эмуляции железа, почти с нативной скоростью.

> 🎯 **Цель:** чтобы ты мог переключаться между ОС за **5 секунд**, не переустанавливая их и не теряя данные.

---

## ✨ Особенности

| Фича | Описание |
|------|----------|
| 🧩 **Свои форматы** | `.cosa` — сжатые образы, `.acos` — активные контейнеры |
| 🗄️ **Свои ФС** | CFS (настройки) + HSFS (образы) |
| 🔗 **Свои протоколы** | KAFOS, KCBUC, KCBUSP, KCBUOTM, KCBUMSA |
| 🖥️ **BIOS-интерфейс** | Как в старых BIOS — синий фон, клавиши, стрелки |
| 💾 **Экономия места** | Контейнеры занимают **до 90% меньше** места |
| ⚡ **Мгновенное переключение** | Между ОС за **5 секунд** |

---

## 🛠️ Как собрать

### 📦 Требования

- **NASM** (компилятор)
- **i686-elf-gcc** (кросс-компилятор)

### 🔧 Сборка

```bash
git clone https://github.com/LevOwner/ShurixOS.git
cd ShurixOS
./build.bat
```

---

## Если вам лень собирать:

- **То вам надо обратиться в раздел релизы [Releases] https://github.com/LevOwner/ShurixOS/releases** 
