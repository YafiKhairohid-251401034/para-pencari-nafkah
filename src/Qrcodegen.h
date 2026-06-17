#pragma once
//  qrcodegen.h — Minimal QR Code generator (public domain)
//  Based on the algorithm by Project Nayuki (https://www.nayuki.io/page/qr-code-generator-library)
//  Simplified single-header C++ port: supports Byte-mode encoding,
//  automatic version selection, and Error Correction Level LOW.
//  No external dependencies — used to render QRIS-style QR codes offline.

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <algorithm>

namespace qrcodegen {

class QrCode {
public:
    enum class Ecc { LOW = 0, MEDIUM, QUARTILE, HIGH };

    static QrCode encodeText(const std::string &text, Ecc ecl = Ecc::LOW) {
        std::vector<uint8_t> data(text.begin(), text.end());
        for (int version = 1; version <= 40; version++) {
            int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;

            int ccBits = (version <= 9) ? 8 : 16;
            int usedBits = 4 + ccBits + static_cast<int>(data.size()) * 8;
            if (usedBits <= dataCapacityBits) {
                return QrCode(version, ecl, data);
            }
        }
        throw std::length_error("Data too long for QR code");
    }

    int size() const { return sideLength; }

    bool getModule(int x, int y) const {
        if (x < 0 || x >= sideLength || y < 0 || y >= sideLength) return false;
        return modules[y * sideLength + x];
    }

private:
    int version;
    int sideLength;
    std::vector<bool> modules;
    std::vector<bool> isFunction;

    QrCode(int ver, Ecc ecl, const std::vector<uint8_t> &data)
        : version(ver), sideLength(ver * 4 + 17) {
        modules.assign(sideLength * sideLength, false);
        isFunction.assign(sideLength * sideLength, false);

        drawFunctionPatterns();
        std::vector<uint8_t> allCodewords = encodeDataCodewords(data, ecl);
        drawCodewords(allCodewords);
        applyBestMask(ecl);
    }

    std::vector<uint8_t> encodeDataCodewords(const std::vector<uint8_t> &data, Ecc ecl) {
        int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;
        int ccBits = (version <= 9) ? 8 : 16;


        std::vector<bool> bb;
        auto appendBits = [&bb](uint32_t val, int len) {
            for (int i = len - 1; i >= 0; i--)
                bb.push_back(((val >> i) & 1u) != 0);
        };

        appendBits(0b0100, 4);
        appendBits(static_cast<uint32_t>(data.size()), ccBits);
        for (uint8_t b : data) appendBits(b, 8);

        int padTerm = std::min(4, dataCapacityBits - static_cast<int>(bb.size()));
        for (int i = 0; i < padTerm; i++) bb.push_back(false);
        while (bb.size() % 8 != 0) bb.push_back(false);

        std::vector<uint8_t> dataCodewords;
        for (size_t i = 0; i < bb.size(); i += 8) {
            uint8_t byte = 0;
            for (int j = 0; j < 8; j++) byte = (byte << 1) | (bb[i + j] ? 1 : 0);
            dataCodewords.push_back(byte);
        }

        const uint8_t padBytes[2] = {0xEC, 0x11};
        int needed = getNumDataCodewords(version, ecl);
        for (int i = 0; dataCodewords.size() < static_cast<size_t>(needed); i++)
            dataCodewords.push_back(padBytes[i % 2]);

        return addEccAndInterleave(dataCodewords, ecl);
    }

    static std::vector<uint8_t> reedSolomonComputeDivisor(int degree) {
        std::vector<uint8_t> result(degree, 0);
        result[degree - 1] = 1;
        uint8_t root = 1;
        for (int i = 0; i < degree; i++) {
            for (size_t j = 0; j < result.size(); j++) {
                result[j] = reedSolomonMultiply(result[j], root);
                if (j + 1 < result.size()) result[j] ^= result[j + 1];
            }
            root = reedSolomonMultiply(root, 0x02);
        }
        return result;
    }
    static uint8_t reedSolomonMultiply(uint8_t x, uint8_t y) {
        int z = 0;
        for (int i = 7; i >= 0; i--) {
            z = (z << 1) ^ ((z >> 7) * 0x11D);
            z ^= ((y >> i) & 1) * x;
        }
        return static_cast<uint8_t>(z);
    }
    static std::vector<uint8_t> reedSolomonComputeRemainder(const std::vector<uint8_t> &data,
                                                            const std::vector<uint8_t> &divisor) {
        std::vector<uint8_t> result(divisor.size(), 0);
        for (uint8_t b : data) {
            uint8_t factor = b ^ result[0];
            result.erase(result.begin());
            result.push_back(0);
            for (size_t i = 0; i < result.size(); i++)
                result[i] ^= reedSolomonMultiply(divisor[i], factor);
        }
        return result;
    }

    std::vector<uint8_t> addEccAndInterleave(const std::vector<uint8_t> &data, Ecc ecl) {
        int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[static_cast<int>(ecl)][version];
        int blockEccLen = ECC_CODEWORDS_PER_BLOCK[static_cast<int>(ecl)][version];
        int rawCodewords = getNumRawDataModules(version) / 8;
        int numShortBlocks = numBlocks - rawCodewords % numBlocks;
        int shortBlockLen = rawCodewords / numBlocks;

        std::vector<std::vector<uint8_t>> blocks;
        std::vector<uint8_t> divisor = reedSolomonComputeDivisor(blockEccLen);
        size_t pos = 0;
        for (int i = 0; i < numBlocks; i++) {
            int len = shortBlockLen - blockEccLen + (i < numShortBlocks ? 0 : 1);
            std::vector<uint8_t> block(data.begin() + pos, data.begin() + pos + len);
            pos += len;
            std::vector<uint8_t> ecc = reedSolomonComputeRemainder(block, divisor);
            block.insert(block.end(), ecc.begin(), ecc.end());
            blocks.push_back(block);
        }

        std::vector<uint8_t> result;
        size_t maxLen = blocks.back().size();
        for (size_t i = 0; i < maxLen; i++) {
            for (size_t j = 0; j < blocks.size(); j++) {
                if (i != shortBlockLen - blockEccLen - 1 || j >= static_cast<size_t>(numShortBlocks))
                    result.push_back(blocks[j][i]);
                else if (i < blocks[j].size())
                    result.push_back(blocks[j][i]);
            }
        }
        return result;
    }

    // Module placement
    void setFunctionModule(int x, int y, bool dark) {
        modules[y * sideLength + x] = dark;
        isFunction[y * sideLength + x] = true;
    }

    void drawFunctionPatterns() {
        for (int i = 0; i < sideLength; i++) {
            setFunctionModule(6, i, i % 2 == 0);
            setFunctionModule(i, 6, i % 2 == 0);
        }
        drawFinderPattern(3, 3);
        drawFinderPattern(sideLength - 4, 3);
        drawFinderPattern(3, sideLength - 4);

        std::vector<int> alignPositions = getAlignmentPatternPositions();
        for (int y : alignPositions)
            for (int x : alignPositions) {
                if ((x == 6 && y == 6) || (x == 6 && y == sideLength - 7) ||
                    (x == sideLength - 7 && y == 6))
                    continue;
                drawAlignmentPattern(x, y);
            }
        drawFormatBits();
    }

    void drawFinderPattern(int cx, int cy) {
        for (int dy = -4; dy <= 4; dy++) {
            for (int dx = -4; dx <= 4; dx++) {
                int dist = std::max(std::abs(dx), std::abs(dy));
                int x = cx + dx, y = cy + dy;
                if (x >= 0 && x < sideLength && y >= 0 && y < sideLength)
                    setFunctionModule(x, y, dist != 2 && dist != 4);
            }
        }
    }

    void drawAlignmentPattern(int cx, int cy) {
        for (int dy = -2; dy <= 2; dy++)
            for (int dx = -2; dx <= 2; dx++)
                setFunctionModule(cx + dx, cy + dy, std::max(std::abs(dx), std::abs(dy)) != 1);
    }

    std::vector<int> getAlignmentPatternPositions() const {
        if (version == 1) return {};
        int numAlign = version / 7 + 2;
        int step;
        if (version == 32) step = 26;
        else step = (version * 4 + numAlign * 2 + 1) / (numAlign * 2 - 2) * 2;
        std::vector<int> result;
        int pos = sideLength - 7;
        for (int i = numAlign - 1; i >= 1; i--, pos -= step)
            result.insert(result.begin(), pos);
        result.insert(result.begin(), 6);
        return result;
    }

    void drawFormatBits() {
        for (int i = 0; i <= 8; i++) {
            setFunctionModule(8, i, false);
            setFunctionModule(i, 8, false);
        }
        for (int i = sideLength - 8; i < sideLength; i++) {
            setFunctionModule(8, i, false);
            setFunctionModule(i, 8, false);
        }
        setFunctionModule(8, 8, false);

        if (version >= 7) {
            for (int i = 0; i < 6; i++) {
                for (int j = 0; j < 3; j++) {
                    setFunctionModule(5 - i, sideLength - 9 - j, false);
                    setFunctionModule(sideLength - 9 - j, 5 - i, false);
                }
            }
        }
    }

    void drawCodewords(const std::vector<uint8_t> &data) {
        size_t i = 0; // bit index
        size_t totalBits = data.size() * 8;
        for (int right = sideLength - 1; right >= 1; right -= 2) {
            if (right == 6) right = 5;
            for (int vert = 0; vert < sideLength; vert++) {
                for (int j = 0; j < 2; j++) {
                    int x = right - j;
                    bool upward = ((right + 1) & 2) == 0;
                    int y = upward ? sideLength - 1 - vert : vert;
                    if (isFunction[y * sideLength + x]) continue;
                    bool bit = false;
                    if (i < totalBits)
                        bit = ((data[i >> 3] >> (7 - (i & 7))) & 1) != 0;
                    modules[y * sideLength + x] = bit;
                    i++;
                }
            }
        }
    }

    static bool getMaskBit(int mask, int x, int y) {
        switch (mask) {
        case 0: return (x + y) % 2 == 0;
        case 1: return y % 2 == 0;
        case 2: return x % 3 == 0;
        case 3: return (x + y) % 3 == 0;
        case 4: return (x / 3 + y / 2) % 2 == 0;
        case 5: return x * y % 2 + x * y % 3 == 0;
        case 6: return (x * y % 2 + x * y % 3) % 2 == 0;
        case 7: return ((x + y) % 2 + x * y % 3) % 2 == 0;
        }
        return false;
    }

    void applyMask(int mask) {
        for (int y = 0; y < sideLength; y++)
            for (int x = 0; x < sideLength; x++)
                if (!isFunction[y * sideLength + x] && getMaskBit(mask, x, y))
                    modules[y * sideLength + x] = !modules[y * sideLength + x];
    }

    long penaltyScore() const {
        long result = 0;
        // Adjacent same-color in rows/cols
        for (int y = 0; y < sideLength; y++) {
            bool color = modules[y * sideLength];
            int runLen = 1;
            for (int x = 1; x < sideLength; x++) {
                if (modules[y * sideLength + x] == color) {
                    runLen++;
                    if (runLen == 5) result += 3;
                    else if (runLen > 5) result += 1;
                } else { color = modules[y * sideLength + x]; runLen = 1; }
            }
        }
        for (int x = 0; x < sideLength; x++) {
            bool color = modules[x];
            int runLen = 1;
            for (int y = 1; y < sideLength; y++) {
                if (modules[y * sideLength + x] == color) {
                    runLen++;
                    if (runLen == 5) result += 3;
                    else if (runLen > 5) result += 1;
                } else { color = modules[y * sideLength + x]; runLen = 1; }
            }
        }
        // 2x2 blocks
        for (int y = 0; y < sideLength - 1; y++)
            for (int x = 0; x < sideLength - 1; x++) {
                bool c = modules[y * sideLength + x];
                if (c == modules[y * sideLength + x + 1] &&
                    c == modules[(y + 1) * sideLength + x] &&
                    c == modules[(y + 1) * sideLength + x + 1])
                    result += 3;
            }

        int dark = 0;
        for (bool m : modules) if (m) dark++;
        int total = sideLength * sideLength;
        int k = static_cast<int>(std::abs(dark * 20 - total * 10)) / total;
        result += k * 10;
        return result;
    }

    void applyBestMask(Ecc ecl) {

        long bestScore = -1;
        int bestMask = 0;
        std::vector<bool> bestModules;
        for (int mask = 0; mask < 8; mask++) {
            applyMask(mask);
            drawFormatBitsForMask(mask, ecl);
            long score = penaltyScore();
            if (bestScore == -1 || score < bestScore) {
                bestScore = score;
                bestMask = mask;
                bestModules = modules;
            }
            applyMask(mask);
            clearFormatBits();
        }
        modules = bestModules;
        (void)bestMask;
    }

    void clearFormatBits() {
        for (int i = 0; i <= 8; i++) {
            modules[i * sideLength + 8] = false;
            modules[8 * sideLength + i] = false;
        }
        for (int i = sideLength - 8; i < sideLength; i++) {
            modules[i * sideLength + 8] = false;
            modules[8 * sideLength + i] = false;
        }
        modules[8 * sideLength + 8] = false;
    }

    void drawFormatBitsForMask(int mask, Ecc ecl) {
        int eccBits;
        switch (ecl) {
        case Ecc::LOW: eccBits = 1; break;
        case Ecc::MEDIUM: eccBits = 0; break;
        case Ecc::QUARTILE: eccBits = 3; break;
        default: eccBits = 2; break;
        }
        int data = eccBits << 3 | mask;
        int rem = data;
        for (int i = 0; i < 10; i++)
            rem = (rem << 1) ^ ((rem >> 9) * 0x537);
        int bits = (data << 10 | rem) ^ 0x5412;

        for (int i = 0; i <= 5; i++) setBit(8, i, bits, i);
        setBit(8, 7, bits, 6);
        setBit(8, 8, bits, 7);
        setBit(7, 8, bits, 8);
        for (int i = 9; i < 15; i++) setBit(14 - i, 8, bits, i);

        for (int i = 0; i <= 7; i++) setBit(sideLength - 1 - i, 8, bits, i);
        for (int i = 8; i < 15; i++) setBit(8, sideLength - 15 + i, bits, i);
        modules[(sideLength - 8) * sideLength + 8] = true; // dark module
    }

    void setBit(int x, int y, int bits, int i) {
        bool b = ((bits >> i) & 1) != 0;
        modules[y * sideLength + x] = b;
    }

    static int getNumRawDataModules(int ver) {
        int result = (16 * ver + 128) * ver + 64;
        if (ver >= 2) {
            int numAlign = ver / 7 + 2;
            result -= (25 * numAlign - 10) * numAlign - 55;
            if (ver >= 7) result -= 36;
        }
        return result;
    }

    static int getNumDataCodewords(int ver, Ecc ecl) {
        int raw = getNumRawDataModules(ver) / 8;
        int blocks = NUM_ERROR_CORRECTION_BLOCKS[static_cast<int>(ecl)][ver];
        int eccPer = ECC_CODEWORDS_PER_BLOCK[static_cast<int>(ecl)][ver];
        return raw - eccPer * blocks;
    }

    static const int8_t ECC_CODEWORDS_PER_BLOCK[4][41];
    static const int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41];
};

// LOW, MEDIUM, QUARTILE, HIGH
inline const int8_t QrCode::ECC_CODEWORDS_PER_BLOCK[4][41] = {
    {-1, 7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
    {-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},
    {-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
    {-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30}
};
inline const int8_t QrCode::NUM_ERROR_CORRECTION_BLOCKS[4][41] = {
    {-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},
    {-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5, 5, 8, 9, 9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},
    {-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8, 8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},
    {-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81}
};

}