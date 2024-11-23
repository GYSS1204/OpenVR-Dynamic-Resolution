#pragma once
#include <unordered_map>
#include <string>

class LanguageManager {
public:
    enum Language {
        ENGLISH,
        SIMPLIFIED_CHINESE
    };

    static LanguageManager& getInstance() {
        static LanguageManager instance;
        return instance;
    }

    void setLanguage(Language lang);
    std::string translate(const std::string& key);

private:
    LanguageManager();
    Language currentLanguage;
    std::unordered_map<std::string, std::unordered_map<Language, std::string>> translations;

    void initializeTranslations();
    
};


void InitImGuiWithChineseFonts();
void setLanguage(int language);


// LanguageManager.h
// #ifndef LANGUAGE_MANAGER_H
// #define LANGUAGE_MANAGER_H

// #include <unordered_map>
// #include <string>
// #include <imgui.h>
// #include <fmt/core.h>


// class LanguageManager {
// public:
//     enum Language {
//         ENGLISH,
//         SIMPLIFIED_CHINESE
//     };

//     // 单例模式获取实例
//     static LanguageManager& getInstance();

//     // 设置当前语言
//     void setLanguage(Language lang);

//     // 获取当前语言
//     int getCurrentLanguage() const;

//     // 翻译文本
//     std::string translate(const std::string& key);

//     // 重新加载字体
//     void reloadFont();

// private:
//     // 私有构造函数（单例模式）
//     LanguageManager();

//     // 禁止拷贝和赋值
//     LanguageManager(const LanguageManager&) = delete;
//     LanguageManager& operator=(const LanguageManager&) = delete;

//     // 当前语言
//     Language currentLanguage;

//     // 存储所有翻译的映射
//     std::unordered_map<std::string, std::unordered_map<Language, std::string>> translations;

//     // 初始化翻译内容
//     void initializeTranslations();
// };

// #endif // LANGUAGE_MANAGER_H