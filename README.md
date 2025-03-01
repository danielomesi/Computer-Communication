# Non-Blocking Multi-Client HTTP Server

## Overview  
This project implements a lightweight **non-blocking HTTP server** that supports multiple clients using `select()`. It is designed to handle various HTTP methods efficiently without multi-threading.  

## Features  
- ✅ **Non-blocking I/O** – Uses `select()` to manage multiple connections efficiently.  
- ✅ **HTTP Method Support** – Implements `OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE`.  
- ✅ **Multi-Language Support** – Handles `lang` query string (`en`, `he`, `fr`) to serve localized pages.  
- ✅ **Request Handling** – `POST` requests log received data to the console.  
- ✅ **Persistent & Timeout Management** – Closes inactive connections after **2 minutes** of inactivity.  

## Implementation Highlights  
- 🔹 Uses **event-driven programming** instead of threads.  
- 🔹 Implements **HTTP parsing and response generation** from scratch.  
- 🔹 Designed to support **real-time debugging and monitoring** via Wireshark.  
- 🔹 Ensures **efficient resource management** by closing inactive connections.  
