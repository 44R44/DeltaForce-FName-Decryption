# DeltaForce FName Decryption

This repository provides an implementation of **FName decryption for Delta Force (Unreal Engine)**.  
It allows you to resolve and decode Unreal Engine `FName` entries from the in-memory NamePool, which is required when building external SDKs, debuggers, or tools.

---

## Features
- Works with **Delta Force (Unreal Engine-based)** games.
- Decrypts both **wide** and **ANSI** names.
- Caches results with an `unordered_map` for fast lookups.
- Provides helper to get an Actor's class name.

---

## 📂 Code Overview

- `DecryptFNameEntry` → Handles XOR-based decryption of name entries.
- `Get_name` → Retrieves and decrypts the string for a given `ComparisonIndex`.
- `getClassName` → Gets the class name of an Actor pointer.

---

## Usage Example

```cpp
// Example: Resolve and print Actor class name

uintptr_t someActor = mem->read<uintptr_t>(actorList + 0x0); 
if (someActor) {
    std::string className = getClassName(someActor);
    std::cout << "Actor Class: " << className << std::endl;
}
