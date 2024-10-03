/*
 * Created by janis on 2024-01-04
 */

#include "error.h"

namespace hs {
    const char* Severity::as_str() const {
        switch (this->kind) {
            case SeverityKind::Error: {
                return "Unknown";
            }
            case SeverityKind::Warning: {
                return "Warning";
            }
            case SeverityKind::Info: {
                return "Info";
            }
            case SeverityKind::Debug: {
                return "Debug";
            }
        }
    }

    SeverityKind Severity::operator*() const {
        return this->kind;
    }

    Severity::Severity(VkDebugUtilsMessageSeverityFlagBitsEXT flags) {
        if (flags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            this->kind = SeverityKind::Error;
        } else if (flags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            this->kind = SeverityKind::Warning;
        } else if (flags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            this->kind = SeverityKind::Info;
        } else {
            this->kind = SeverityKind::Debug;
        }
    }

    std::string Error::as_str() const {
        const char* kind_string = "";
        switch (this->kind) {
            case ErrorKind::Unknown: {
                kind_string = "Unknown";
                break;
            }
        }

        return kind_string;
    }
}