"""
PysOS System Initialization Script
Pythonic な OS 拡張の第一歩
"""
import os_bridge as os

def boot():
    os.print("PysOS v0.1.0 initialization...")
    
    # サービスの起動（リストで管理）
    services = ["vfs", "proc", "input"]
    for service in services:
        os.print(f"Starting {service} service... Done.")
    
    os.print("Type 'help' for a list of commands.")
    os.print("PysOS Ready.")

if __name__ == "__main__":
    boot()