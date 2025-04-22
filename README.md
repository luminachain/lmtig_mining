

# 🛠️ lmtig - LuminaChain CPU Miner

**lmtig** is the official CPU-only miner for the [LuminaChain](https://luminachain.pro) blockchain.  
It is a minimalistic, command-line-only mining tool forked and streamlined from the Monero mining software — redesigned exclusively for **rx/lmn**, the mining algorithm used by LuminaChain.

---

## 🔧 What's Special About lmtig?

**lmtig** is a pure CPU miner optimized for **fair, transparent mining** on LuminaChain.

### ✅ Key Modifications

- ❌ Removed all legacy mining algorithms  
- ❌ Removed all configuration file support  
- ❌ Removed GPU mining support  
- ✅ Only supports **`rx/lmn`** (Lumina RandomX variant)  
- ✅ Only runs via **command-line interface**  



## 🚀 Quick Start

Download the latest release for your system from the [Releases](./releases) page.

### Run the miner:

```bash
./lmtig -o <pool_url> -a rx/lmn -u <your_lumina_address> -p x --coin LMT
```

### Example:

```bash
./lmtig -o 127.0.0.1:8888 -a rx/lmn -u LMN1y...xyz -p x --coin LMT
```

> 💡 `lmtig` does **not** support config files. All settings must be passed through CLI options.

---

## ⚙️ Command-Line Options

| Option | Description |
|--------|-------------|
| `-v`, `--version`        | Output version number |
| `-o`, `--url <char>`     | URL of mining server or pool |
| `-a`, `--algo <char>`    | Mining algorithm (must be `rx/lmn`) |
| `--coin <char>`          | Specify coin (should be `LMT`) |
| `-u`, `--user <char>`    | Username or wallet address |
| `-p`, `--pass <char>`    | Password (use `x` if unsure) |
| `-h`, `--help`           | Display help information |

---

## 🧠 Algorithm: `rx/lmn`

`lmtig` runs exclusively on **`rx/lmn`**, a customized variant of RandomX tailored for LuminaChain:

- Privacy features like ring signatures and stealth addresses are removed  
- Preserves ASIC resistance and CPU-level mining fairness  
- Designed for transparency and regulatory friendliness  
- Cannot run on GPU — **CPU mining only**

---

## 🧱 Build from Source

```bash
git clone https://github.com/luminachain/lmtig_mining.git
cd lmtig
mkdir build && cd build
cmake ..
make -j$(nproc)
```

---

## 📜 License

This project is based on Monero and inherits its license (BSD 3-Clause).  
All original modifications are © 2025 LuminaChain Contributors.

---

## 🌐 Learn More

- 🌍 [LuminaChain Website](https://luminachain.pro)
- 📣 [Twitter](https://x.com/luminachainorg)

> Secure. Transparent. Fair for all. — **lmtig**


