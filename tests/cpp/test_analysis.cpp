#include <gtest/gtest.h>
#include <bunseki/analysis.hpp>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>

TEST(AnalysisTest, RejectsEmptySamples){
    std::vector<float> vec;

    EXPECT_THROW(
        Analysis analysis(vec),
        std::invalid_argument
    );
}

TEST(AnalysisTest, CalculatesRMS){
    std::vector<float> zeros(4);
    Analysis a0(zeros);
    EXPECT_FLOAT_EQ(a0.rms(), 0.0f);

    std::vector<float> ones(4, 1.0f);
    Analysis a1(ones);
    EXPECT_FLOAT_EQ(a1.rms(), 1.0f);

    std::vector<float> minus_ones(4, -1.0f);
    Analysis a2(minus_ones);
    EXPECT_FLOAT_EQ(a2.rms(), 1.0f);

    std::vector<float> alternating = {1.0f, -1.0f, 1.0f, -1.0f};
    Analysis a3(alternating);
    EXPECT_FLOAT_EQ(a3.rms(), 1.0f);

    std::vector<float> twos(2, 2.0f);
    Analysis a4(twos);
    EXPECT_FLOAT_EQ(a4.rms(), 2.0f);
    EXPECT_FLOAT_EQ(a4.rms(), a1.rms() * 2);

    std::vector<float> values = {3.0f, 4.0f};
    Analysis a5(values);
    EXPECT_NEAR(a5.rms(), std::sqrt(12.5f), 0.00001f);
}

TEST(AnalysisTest, CalculatesP2PAmplitude){
    std::vector<float> v0(5);
    Analysis a0(v0);
    EXPECT_FLOAT_EQ(a0.p2p_amplitude(), 0.0f);

    std::vector<float> v1(5, 1.0f);
    Analysis a1(v1);
    EXPECT_FLOAT_EQ(a1.p2p_amplitude(), 0.0f);

    std::vector<float> v2 = {-3.0f, 0.0f, 1.0f, 3.0f, 2.4f};
    Analysis a2(v2);
    EXPECT_FLOAT_EQ(a2.p2p_amplitude(), 6.0f);

    std::vector<float> v3 = {-3.0f, -0.3f, -1.0f, -2.0f, -2.4f};
    Analysis a3(v3);
    EXPECT_FLOAT_EQ(a3.p2p_amplitude(), 2.7f);
}

TEST(AnalysisTest, CalculatesPeakAmplitude){
    std::vector<float> v0(5);
    Analysis a0(v0);
    EXPECT_FLOAT_EQ(a0.peak_amplitude(), 0.0f);

    std::vector<float> v1(5, 1.0f);
    Analysis a1(v1);
    EXPECT_FLOAT_EQ(a1.peak_amplitude(), 1.0f);

    std::vector<float> v2 = {-3.0f, 0.1f, 2.7f, -2.8f, 1.0f};
    Analysis a2(v2);
    EXPECT_FLOAT_EQ(a2.peak_amplitude(), 3.0f);

    std::vector<float> v3 = {-0.5f, -2.0f, -1.2f};
    Analysis a3(v3);
    EXPECT_FLOAT_EQ(a3.peak_amplitude(), 2.0f);
}

TEST(AnalysisTest, CalculatesZeroCrossingRate){
    std::vector<float> v0(5);
    Analysis a0(v0);
    EXPECT_FLOAT_EQ(a0.zero_crossing_rate(), 0.0f);

    std::vector<float> v1(5, 1.0f);
    Analysis a1(v1);
    EXPECT_FLOAT_EQ(a1.zero_crossing_rate(), 0.0f);

    std::vector<float> v2 = {0.3f, -1.0f, 1.2f, 3.5f, -0.1f};
    Analysis a2(v2);
    EXPECT_FLOAT_EQ(a2.zero_crossing_rate(), 0.75f);

    std::vector<float> v3 = {0.3f, 1.0f, 1.2f, 3.5f, 0.1f};
    Analysis a3(v3);
    EXPECT_FLOAT_EQ(a3.zero_crossing_rate(), 0.0f);

    std::vector<float> v4 = {-1.0f, 1.0f, 1.2f, 3.5f, 0.1f};
    Analysis a4(v4);
    EXPECT_FLOAT_EQ(a4.zero_crossing_rate(), 0.25f);
}

TEST(AnalysisTest, DetectsSilence){
    std::vector<float> samples = {
        0.0f, 0.0f,
        0.5f,
        0.0f, 0.0f,
        0.5f,
        0.0f
    };

    Analysis analysis(samples);
    auto result = analysis.silence_detection();

    ASSERT_EQ(result.size(), 3);

    EXPECT_FLOAT_EQ(result[0].first, 0.0f);
    EXPECT_FLOAT_EQ(result[0].second, 2.0f / SAMPLE_RATE);

    EXPECT_FLOAT_EQ(result[1].first, 3.0f / SAMPLE_RATE);
    EXPECT_FLOAT_EQ(result[1].second, 5.0f / SAMPLE_RATE);

    EXPECT_FLOAT_EQ(result[2].first, 6.0f / SAMPLE_RATE);
    EXPECT_FLOAT_EQ(result[2].second, 7.0f / SAMPLE_RATE);

    std::vector<float> no_silence(7, 0.5f);
    Analysis a2(no_silence);
    EXPECT_TRUE(a2.silence_detection().empty());

    std::vector<float> silence(7);
    Analysis a3(silence);
    auto result2 = a3.silence_detection();
    ASSERT_EQ(result2.size(), 1);

    EXPECT_FLOAT_EQ(result2[0].first, 0.0f);
    EXPECT_FLOAT_EQ(result2[0].second, 7.0f / SAMPLE_RATE);

    Analysis a4(silence);

    EXPECT_THROW(
        a4.silence_detection(-1.0f),
        std::invalid_argument
    );
}

TEST(AnalysisTest, CalculatesRMSWindows){
    std::vector<float> samples = {
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f,
        -1.0f, -1.0f
    };
    Analysis a0(samples);
    std::vector<float> res = a0.rms_windows(4, 2);

    ASSERT_EQ(res.size(), 3);

    EXPECT_FLOAT_EQ(res[0], 1.0f);
    EXPECT_NEAR(res[1], std::sqrt(0.5f), 0.00001f);
    EXPECT_NEAR(res[2], std::sqrt(0.5f), 0.00001f);
}

TEST(AnalysisTest, DetectsOnsets){
    std::vector<float> samples(2347, 0.0f);
    std::fill(samples.begin() + 1024, samples.end(), 1.0f);

    Analysis a0(samples);

    EXPECT_THROW(
        std::vector<std::size_t> res = a0.onset_detection(-1.2f, 1),
        std::invalid_argument
    );

    EXPECT_THROW(
        std::vector<std::size_t> res = a0.onset_detection(1.0f, -1),
        std::invalid_argument
    );

    std::vector<std::size_t> result = a0.onset_detection(0.05f);

    ASSERT_EQ(result.size(), 2);

    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 3);

    std::vector<float> empty(3000, 0.0f);

    Analysis a1(empty);

    EXPECT_TRUE(a1.onset_detection(0.05f).empty());
}

TEST(AnalysisTest, CalculatesOnsetIntervals){
    std::vector<float> samples(35 * STANDARD_HOP_SIZE + STANDARD_WINDOW_SIZE, 0.0f);

    // Primeiro pulso
    std::fill(
        samples.begin() + 10 * STANDARD_HOP_SIZE,
        samples.begin() + 12 * STANDARD_HOP_SIZE,
        1.0f
    );

    // Segundo pulso
    std::fill(
        samples.begin() + 20 * STANDARD_HOP_SIZE,
        samples.begin() + 22 * STANDARD_HOP_SIZE,
        1.0f
    );

    // Terceiro pulso
    std::fill(
        samples.begin() + 35 * STANDARD_HOP_SIZE,
        samples.end(),
        1.0f
    );

    Analysis a0(samples);

    std::vector<std::size_t> expected = {10, 15};
    std::vector<std::size_t> result = a0.onset_intervals();

    EXPECT_EQ(result, expected);


    std::vector<float> empty(2000, 0.0f);
    Analysis a1(empty);
    EXPECT_TRUE(a1.onset_intervals().empty());
}

