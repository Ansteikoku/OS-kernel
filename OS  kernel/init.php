<?php
/**
 * bubblesOS System Initialization Script
 * WebエンジニアがOSを拡張するためのエントリーポイント
 */

// C言語側で定義した OS::print() を呼び出す想定
OS::print("bubblesOS boot sequence started.");

// システム構成を連想配列で管理（PHPらしさ）
$system_check = [
    "Memory"  => "1024KB detected",
    "Display" => "VGA Text Mode initialized",
    "Drivers" => "Standard Keyboard loaded"
];

foreach ($system_check as $component => $status) {
    OS::print("[OK] $component: $status");
}

OS::print("Starting Bubble Shell...");
OS::print("Welcome to bubblesOS!");