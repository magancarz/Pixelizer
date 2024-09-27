#pragma once

#include <filesystem>

class PathBuilder
{
public:
    explicit PathBuilder(std::filesystem::path starting_path = std::filesystem::current_path());

    template <typename T>
    PathBuilder& append(const T& path)
    {
        current_path /= std::filesystem::path(path);
        return *this;
    }

    PathBuilder& fileExtension(const std::string& extension);
    std::string build();

private:
    std::filesystem::path current_path;
};
