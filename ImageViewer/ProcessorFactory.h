#ifndef PROCESSORFACTORY_H
#define PROCESSORFACTORY_H
#include <map>
#include <string>
#include <memory>
#include "ImageProcessor.h"

class ProcessorFactory {
public:
    // 注册算法（名称 → 创建函数）
    static void registerProcessor(
        const std::string &name,
        std::function<std::unique_ptr<ImageProcessor>()> creator
        ) {
        registry()[name] = std::move(creator);
    }

    // 创建算法实例
    static std::unique_ptr<ImageProcessor> create(const std::string &name) {
        auto it = registry().find(name);
        if (it != registry().end()) {
            return it->second();
        }
        return nullptr;
    }

private:
    // 算法注册表（线程安全）
    static std::map<std::string, std::function<std::unique_ptr<ImageProcessor>()>>& registry() {
        static std::map<std::string, std::function<std::unique_ptr<ImageProcessor>()>> instance;
        return instance;
    }
};
#endif // PROCESSORFACTORY_H
