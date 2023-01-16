#include <semverpp/version.hpp>

#include <gtest/gtest.h>

TEST(VersionTest, ConstructFromRawValues)
{
    constexpr auto major = 1, minor = 2, patch = 3;

    semverpp::version version{major, minor, patch};
    
    EXPECT_EQ(version.major, major);
    EXPECT_EQ(version.minor, minor);
    EXPECT_EQ(version.patch, patch);
}

TEST(VersionTest, ConstructFromString)
{
    semverpp::version version{"1.2.3"};

    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
}

TEST(VersionTest, ConstructFromStringWithPrefix)
{
    semverpp::version version{"v1.2.3"};

    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
}

TEST(VersionTest, VersionNumberValueRange)
{
    EXPECT_NO_THROW((semverpp::version{0, 0, 1}));
    EXPECT_NO_THROW((semverpp::version{0, 1, 0}));
    EXPECT_NO_THROW((semverpp::version{1, 0, 0}));

    ////////////////////////////////////////////////////

    EXPECT_THROW((semverpp::version{-1, 0, 1}), semverpp::invalid_version);
    EXPECT_THROW((semverpp::version{0, -1, 1}), semverpp::invalid_version);
    EXPECT_THROW((semverpp::version{0, 1, -1}), semverpp::invalid_version);

    ////////////////////////////////////////////////////

    EXPECT_THROW((semverpp::version{0, 0, 0}), semverpp::invalid_version);
}

TEST(VersionTest, SupportsPreReleaseVersionFromRawValues)
{
    semverpp::version version{1, 2, 3, semverpp::prerelease{"alpha.1"}};

    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
    
    EXPECT_EQ(version.prerelease, "alpha.1");
}

TEST(VersionTest, SupportsPreReleaseVersionFromPrefixedString)
{
    semverpp::version version{"v1.2.3-alpha.1"};

    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
    
    EXPECT_EQ(version.prerelease, "alpha.1");
}

TEST(VersionTest, SupportsBuildMetadataRawValues)
{
    semverpp::version version{1, 2, 3, semverpp::build_metadata{"20230113000000"}};

    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
    
    EXPECT_EQ(version.build_metadata, "20230113000000");
}

TEST(VersionTest, SupportsBuildMetadataFromPrefixedString)
{
    semverpp::version version{"v1.2.3-alpha.1+20230113000000"};

    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
    
    EXPECT_EQ(version.build_metadata, "20230113000000");
}

TEST(VersionTest, EqualityComparisons)
{
    constexpr auto major = 1, minor = 2, patch = 3;

    ////////////////////////////////////////////////////

    EXPECT_EQ(
        (semverpp::version{major, minor, patch}),
        (semverpp::version{major, minor, patch})
    );

    ////////////////////////////////////////////////////

    EXPECT_NE(
        (semverpp::version{major, minor, patch, semverpp::prerelease{"alpha"}}),
        (semverpp::version{major, minor, patch, semverpp::prerelease{"beta"}})
    );

    ////////////////////////////////////////////////////

    EXPECT_EQ(
        (semverpp::version{major, minor, patch, semverpp::build_metadata{"20230113000000"}}),
        (semverpp::version{major, minor, patch, semverpp::build_metadata{"20230114235959"}})
    );
}

TEST(VersionTest, PrecedenceComparisons)
{
    constexpr auto major = 1, minor = 2, patch = 3;

    ////////////////////////////////////////////////////

    EXPECT_GT(
        (semverpp::version{major + 1, minor, patch}),
        (semverpp::version{major, minor, patch})
    );
    
    EXPECT_GT(
        (semverpp::version{major, minor + 1, patch}),
        (semverpp::version{major, minor, patch})
    );
    
    EXPECT_GT(
        (semverpp::version{major, minor, patch + 1}),
        (semverpp::version{major, minor, patch})
    );

    ////////////////////////////////////////////////////

    EXPECT_LT(
        (semverpp::version{major - 1, minor, patch}),
        (semverpp::version{major, minor, patch})
    );
    
    EXPECT_LT(
        (semverpp::version{major, minor - 1, patch}),
        (semverpp::version{major, minor, patch})
    );
    
    EXPECT_LT(
        (semverpp::version{major, minor, patch - 1}),
        (semverpp::version{major, minor, patch})
    );

    ////////////////////////////////////////////////////

    EXPECT_GE(
        (semverpp::version{major + 1, minor, patch}),
        (semverpp::version{major, minor, patch})
    );
    
    EXPECT_GE(
        (semverpp::version{major, minor + 1, patch}),
        (semverpp::version{major, minor, patch})
    );
    
    EXPECT_GE(
        (semverpp::version{major, minor, patch + 1}),
        (semverpp::version{major, minor, patch})
    );

    ////////////////////////////////////////////////////

    EXPECT_GT(
        (semverpp::version{major, minor, patch, semverpp::prerelease{"beta"}}),
        (semverpp::version{major, minor, patch, semverpp::prerelease{"alpha"}})
    );
}

TEST(VersionTest, ConvertsToString)
{
    semverpp::version version{1, 2, 3, semverpp::prerelease{"beta"}, semverpp::build_metadata{"20230113000000"}};
    EXPECT_EQ(version.string(), "1.2.3-beta+20230113000000");

    semverpp::version reconstructed{version.string()};
    EXPECT_EQ(version, reconstructed);
}

TEST(VersionTest, ThrowsOnInvalidVersionSeparators)
{
    EXPECT_THROW((semverpp::version{"1,2,3"}), semverpp::invalid_version);
}

TEST(VersionTest, ThrowsOnIllegalCharactersInPrereleaseAndBuildMetadata)
{
    EXPECT_THROW((semverpp::version{"1.2.3-prelease.with.illegal.character.$"}), semverpp::invalid_version);
    EXPECT_THROW((semverpp::version{"1.2.3+build.meta.with.illegal.character.$"}), semverpp::invalid_version);
}

TEST(VersionTest, OverallBehavior)
{
    EXPECT_TRUE(semverpp::version{"1.2.3"} > semverpp::version{"1.2.0"});
    EXPECT_TRUE(semverpp::version{"1.1.0"} > semverpp::version{"0.2.5"});
    
    EXPECT_TRUE(semverpp::version{"1.7"} > semverpp::version{"1.6.3"});
    
    EXPECT_TRUE(semverpp::version{"1.7.4"} >= semverpp::version{"1.7"});
    
    EXPECT_TRUE(semverpp::version{"0.2-alpha"} < semverpp::version{"0.2-beta"});
    
    auto ver = semverpp::version{"release-1.12.1-alpha.3.foo+buildnum19483824028"};
    EXPECT_TRUE(ver.patch == 1); 
}
