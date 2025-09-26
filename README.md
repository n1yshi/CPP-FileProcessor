# Multi-threaded File Processing System

This is a project I made for my final class.  
It is written in C++ and shows how to use some advanced features like multi-threading and design patterns.  
⚠️ The project is still in development and not fully working yet.

## **Features**

- Processes files using multiple threads (thread pool)
- Plugin system to support different file types
- Shows progress using the Observer pattern
- Uses Factory pattern to create file processors
- Uses templates for generic data structures
- Manages memory with smart pointers
- Handles errors and logs them
- Loads settings from config file
- Shows performance stats

## **Architecture**

The project uses several design patterns:

- **Factory Pattern** – creates different file processors  
- **Observer Pattern** – updates progress info  
- **Strategy Pattern** – switches between processing methods  
- **Singleton Pattern** – for config and logging  
- **Template Pattern** – for reusable data structures

## **License**

MIT License – see the LICENSE file for more info.
