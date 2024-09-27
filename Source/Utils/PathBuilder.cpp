#include "PathBuilder.h"

#include <utility>

PathBuilder::PathBuilder(std::filesystem::path starting_path)
    : current_path{std::move(starting_path)} {}

PathBuilder& PathBuilder::fileExtension(const std::string& extension)
{
    if (!current_path.generic_string().ends_with(extension))
    {
        current_path += std::filesystem::path(extension);
    }

    return *this;
}

std::string PathBuilder::build()
{
    return current_path.generic_string();
}
