/*
 * Created by janis on 2023-10-01
 */

#include "netpbm.h"

#include <memory>
#include <fstream>
#include <vector>
#include <iostream>
#include <optional>
#include <charconv>

namespace hs::f {
    std::tuple<std::vector<uint8_t>, uint32_t, uint32_t> load_ppm(const std::string& file) {
        std::ifstream stream = std::ifstream(file, std::ios::in | std::ios::binary | std::ios::ate);
        if (stream.is_open()) {
            size_t stream_len = stream.tellg();
            std::vector<uint8_t> buf = std::vector<uint8_t>(stream_len);
            stream.seekg(0);
            stream.read(reinterpret_cast<char*>(buf.data()), stream_len);

            auto magic = std::string_view(reinterpret_cast<char*>(buf.data()), 2);
            if (magic == "P6") {
                std::optional<std::string_view> width_str;
                std::optional<std::string_view> height_str;
                std::optional<std::string_view> max_str;

                size_t cursor = 2;
                bool comment = false;
                std::optional<size_t> start_width;
                std::optional<size_t> start_height;
                std::optional<size_t> start_max;

                while (!(width_str && height_str && max_str)) {
                    if (comment) {
                        if (buf[cursor] == '\n') {
                            comment = false;
                        }
                    } else if (!start_width) {
                        if (buf[cursor] == '#') {
                            comment = true;
                        } else if (std::isdigit(buf[cursor])) {
                            start_width = cursor;
                        }
                    } else if (start_width && !width_str) {
                        if (!std::isdigit(buf[cursor])) {
                            width_str = std::string_view(reinterpret_cast<char*>(buf.data() + start_width.value()), cursor - start_width.value());
                        }
                    } else if (!start_height) {
                        if (buf[cursor] == '#') {
                            comment = true;
                        } else if (std::isdigit(buf[cursor])) {
                            start_height = cursor;
                        }
                    } else if (start_height && !height_str) {
                        if (!std::isdigit(buf[cursor])) {
                            height_str = std::string_view(reinterpret_cast<char*>(buf.data() + start_height.value()), cursor - start_height.value());
                        }
                    } else if (!start_max) {
                        if (buf[cursor] == '#') {
                            comment = true;
                        } else if (std::isdigit(buf[cursor])) {
                            start_max = cursor;
                        }
                    } else if (start_max && !max_str) {
                        if (!std::isdigit(buf[cursor])) {
                            max_str = std::string_view(reinterpret_cast<char*>(buf.data() + start_max.value()), cursor - start_max.value());
                        }
                    }

                    cursor++;
                }

                uint32_t width;
                std::from_chars(width_str.value().data(), width_str.value().data() + width_str.value().size(), width);
                uint32_t height;
                std::from_chars(height_str.value().data(), height_str.value().data() + height_str.value().size(), height);
                uint32_t max;
                std::from_chars(max_str.value().data(), max_str.value().data() + max_str.value().size(), max);

                auto img = std::vector<uint8_t>(width * height * 4);

                for (uint32_t y = 0; y < height; y++) {
                    for (uint32_t x = 0; x < width; x++) {
                        img[(y * width + x) * 4] = buf[cursor];
                        img[(y * width + x) * 4 + 1] = buf[cursor + 1];
                        img[(y * width + x) * 4 + 2] = buf[cursor + 2];
                        img[(y * width + x) * 4 + 3] = 0;

                        cursor += 3;
                    }
                }

                return {img, width, height};
            } else {
                std::cerr << "[F] File " << file << " incompatible" << std::endl;
            }
        } else {
            std::cerr << "[F] File " << file << " not found" << std::endl;
        }

        return {std::vector<uint8_t>(), 0, 0};
    }
}
