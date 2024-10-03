/*
 * Created by janis on 2024-01-04
 */

#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace hs {
    enum class SeverityKind {
        Error,
        Warning,
        Info,
        Debug
    };

    class Severity {
    public:
        Severity(SeverityKind kind) : kind(kind) {}
        Severity(VkDebugUtilsMessageSeverityFlagBitsEXT flags);

        SeverityKind operator*() const;

        [[nodiscard]] const char* as_str() const;
    private:
        SeverityKind kind;
    };

    enum class ErrorKind {
        Unknown
    };

    class Error {
    public:
        Error(ErrorKind kind) : kind(kind) {}

        [[nodiscard]] std::string as_str() const;
    private:
        ErrorKind kind;
    };
}
