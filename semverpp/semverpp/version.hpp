#pragma once
#include <stdexcept>
#include <string>
#include <ostream>
#include <charconv>
#include <cctype>

namespace semverpp
{
    //
    // @brief Specifies the pre-release version.
    //
    struct prerelease
    {
        std::string value;
    };

    //
    // @brief Specifies the build metadata.
    //
    struct build_metadata
    {
        std::string value;
    };

    //
    // @brief Thrown if the version info is invalid.
    //
    class invalid_version : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    struct version
    {
        static constexpr auto separator = '.';

        int major{0};
        int minor{0};
        int patch{0};

        std::string prerelease;
        std::string build_metadata;

        version(int major, int minor, int patch)
        : major{major}, minor{minor}, patch{patch}
        { verify(); }

        ////////////////////////////////////////////////////
        
        version(int major, int minor, int patch, semverpp::prerelease&& prerelease)
        : major{major}, minor{minor}, patch{patch}, prerelease{std::move(prerelease.value)}
        { verify(); }

        ////////////////////////////////////////////////////
        
        version(int major, int minor, int patch, semverpp::build_metadata&& build_metadata)
        : major{major}, minor{minor}, patch{patch}, build_metadata{std::move(build_metadata.value)}
        { verify(); }

        ////////////////////////////////////////////////////
        
        version(int major, int minor, int patch, semverpp::prerelease&& prerelease, semverpp::build_metadata&& build_metadata)
        : major{major}, minor{minor}, patch{patch}, prerelease{std::move(prerelease.value)}, build_metadata{std::move(build_metadata.value)}
        { verify(); }
        
        version(int major, int minor, int patch, semverpp::build_metadata&& build_metadata, semverpp::prerelease&& prerelease)
        : major{major}, minor{minor}, patch{patch}, prerelease{std::move(prerelease.value)}, build_metadata{std::move(build_metadata.value)}
        { verify(); }

        ////////////////////////////////////////////////////

        version(std::string_view string)
        {
            auto curr = string.data();

            // Skip any non-digit characters from the version string
            while (!std::isdigit(*curr))
                curr++;

            auto end = string.data() + string.size();

            std::from_chars_result result = std::from_chars(curr, end, major);

            if ((result = std::from_chars(curr, end, minor)).ptr == curr)
                throw invalid_version(std::string{"in version "} + string.data() + ": failed to parse major version number");

            if (*(curr + 1) && std::isdigit(*(curr + 2)))
            {
                if (*(++curr) != separator)
                    throw invalid_version(std::string{"in version "} + string.data() + ": invalid separator");

                if ((result = std::from_chars(++curr, end, minor)).ptr == curr)
                    throw invalid_version(std::string{"in version "} + string.data() + ": failed to parse minor version number");
                
                if (*(curr + 1) && std::isdigit(*(curr + 2)))
                {
                    if (*(++curr) != separator)
                        throw invalid_version(std::string{"in version "} + string.data() + ": invalid separator");

                    if ((result = std::from_chars(++curr, end, patch)).ptr == curr)
                        throw invalid_version(std::string{"in version "} + string.data() + ": failed to parse patch version number");
                }
            }

            curr++;

            if (*curr == '-')
            {
                curr++;
                auto prerel_end = curr;

                while(*prerel_end != '\0' && *prerel_end != '+')
                    prerel_end++;

                prerelease.assign(curr, prerel_end);
                curr = prerel_end;
            }
            
            if (*curr == '+')
            {
                curr++;
                auto meta_end = curr;

                while(*meta_end != '\0')
                    meta_end++;

                build_metadata.assign(curr, meta_end);
                curr = meta_end;
            }

            verify();
        }

        ////////////////////////////////////////////////////

        version(const version&) = default;
        version(version&&) = default;

        inline std::string string() const
        {
            std::string result;

            result.append(std::to_string(major));
            result.append(".");
            result.append(std::to_string(minor));
            result.append(".");
            result.append(std::to_string(patch));

            if (!prerelease.empty())
            {
                result.append("-");
                result.append(prerelease);
            }

            if (!build_metadata.empty())
            {
                result.append("+");
                result.append(build_metadata);
            }

            return result;
        }

        inline void verify() const
        {
            if (!major && !minor && !patch)
                throw invalid_version("in version " + string() + ": version cannot be null");

            if (major < 0)
                throw invalid_version("in version " + string() + ": major version must be >= 0");
                
            if (minor < 0)
                throw invalid_version("in version " + string() + ": minor version must be >= 0");
                
            if (patch < 0)
                throw invalid_version("in version " + string() + ": patch version must be >= 0");

            for (auto& c : prerelease)
                if (!std::isalnum(c) && c != '-' && c != '.')
                    throw invalid_version(std::string{"in version "} + string() + ": prerelease version contains invalid character '" + c + "'");
                
            for (auto& c : build_metadata)
                if (!std::isalnum(c) && c != '-' && c != '.')
                    throw invalid_version(std::string{"in version "} + string() + ": build metadata contains invalid character '" + c + "'");
        }
    };

    inline bool operator==(const version& lhs, const version& rhs)
    {
        return (
            lhs.major == rhs.major &&
            lhs.minor == rhs.minor &&
            lhs.patch == rhs.patch &&
            lhs.prerelease == rhs.prerelease
        );
    }
    
    inline bool operator!=(const version& lhs, const version& rhs)
    {
        return !(lhs == rhs);
    }

    template<template<class> class Comp>
    inline bool compare_versions_with(const version& lhs, const version& rhs)
    {
        auto comp_num = Comp<decltype(lhs.major)>{};
        auto comp_str = Comp<std::string_view>{};

        if (comp_num(lhs.major, rhs.major))
            return true;
        
        if (lhs.major == rhs.major &&
            comp_num(lhs.minor, rhs.minor)
        )
            return true;
            
        if (lhs.major == rhs.major &&
            lhs.minor == rhs.minor &&
            comp_num(lhs.patch, rhs.patch)
        )
            return true;

        if (comp_str(lhs.prerelease, rhs.prerelease))
            return true;

        return false;
    }
    
    inline bool operator>(const version& lhs, const version& rhs)
    {
        return compare_versions_with<std::greater>(lhs, rhs);
    }
    
    inline bool operator>=(const version& lhs, const version& rhs)
    {
        return compare_versions_with<std::greater_equal>(lhs, rhs);
    }
    
    inline bool operator<(const version& lhs, const version& rhs)
    {
        return compare_versions_with<std::less>(lhs, rhs);
    }
    
    inline bool operator<=(const version& lhs, const version& rhs)
    {
        return compare_versions_with<std::less_equal>(lhs, rhs);
    }

    inline std::ostream& operator<<(std::ostream& os, const version& version)
    {
        return os << version.string();
    }
    
    /*inline bool operator<(const version& lhs, const version& rhs)
    {
        return !(lhs == rhs);
    }*/
}
