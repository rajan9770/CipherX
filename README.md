\# 🔐 CipherX – Secure File Encryption



CipherX is a Windows desktop application for securely encrypting and decrypting files using \*\*AES-256 encryption\*\*.



The project is built in \*\*C++\*\* with a native Windows GUI and OpenSSL cryptographic libraries.



\## ✨ Features



\* 🔐 AES-256 file encryption

\* 🔓 File decryption with password protection

\* 📁 File creation

\* 🗑️ File deletion

\* 🔑 Password-based encryption and decryption

\* ⚠️ Wrong-password and corrupted-file detection

\* 🖥️ Native Windows GUI

\* 🎨 Custom CipherX application icon and background

\* ℹ️ About CipherX information dialog



\## 🛠️ Technologies



\* \*\*C++\*\*

\* \*\*Win32 API\*\*

\* \*\*GDI+\*\*

\* \*\*OpenSSL\*\*

\* \*\*AES-256\*\*

\* \*\*GCC 13.2.0\*\*



\## 📁 Project Structure



```text

CipherX\_GUI/

│

├── src/

│   ├── aes\_crypto.cpp

│   ├── aes\_crypto.h

│   ├── gui\_main.cpp

│   └── main.cpp

│

├── CipherX.ico

├── CipherX\_background.png

├── resource.rc

├── libcrypto-4-x64.dll

├── libssl-4-x64.dll

├── .gitignore

└── README.md

```



\## ⚙️ Requirements



\* Windows 10/11

\* MinGW / GCC 13.2.0 or compatible compiler

\* OpenSSL

\* Windows SDK / Win32 development environment



\## 🚀 Building



Clone the repository and open a terminal in the project directory.



The project uses the following main source files:



```text

src/gui\_main.cpp

src/aes\_crypto.cpp

src/aes\_crypto.h

```



The final build command depends on the local OpenSSL/MinGW library configuration.



\## ▶️ Running



After building the application, run:



```text

CipherX.exe

```



Make sure the required OpenSSL DLL files and the CipherX background image are available alongside the executable.



\## 🔒 Security



CipherX is a learning/project implementation of file encryption using AES-256 and OpenSSL.



Do not use CipherX as the sole protection for highly sensitive or irreplaceable data without independently reviewing and validating the implementation.



\## 📌 Project Status



\*\*Version:\*\* 1.0



CipherX currently provides a functional Windows GUI for file creation, encryption, decryption, and deletion.



\## 👨‍💻 Author



\*\*Rajan Chauhan\*\*



B.Tech – Computer Science \& Engineering



