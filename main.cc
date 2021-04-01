   // 承载类型(PackType)， 实际位数N
    template <typename P, uint16_t N>
    struct BitWrapper {
        static constexpr P max = (P(1) << (N - std::is_signed_v<P>)) - 1;
        //防止负无符号整型编译器报错，所以手写补码
        static constexpr P min = P(((P(1) << (sizeof(P) * 8 - N + 1)) - 1) << (N - 1)) * std::is_signed_v<P>;

        // 显式转换为承载类型，主要是兼容C语言的printf系列函数和三目运算符
        inline constexpr P get() const {
            return value;
        }
        // 隐式转换为承载类型
        inline constexpr operator P() const {
            return value;
        }

        // 允许显式转换为枚举
        template<typename T, std::enable_if_t<std::is_enum_v<T>> * = nullptr>
        inline constexpr explicit operator T() const {
            return static_cast<T>(value);
        }

        // 允许整型、弱枚举赋值给本类
        inline BitWrapper &operator=(const P num) {
            value = num;
            return *this;
        }
        // 允许其它BitWrapper类型赋值给本类
        template<typename T, uint16_t U>
        inline BitWrapper &operator=(const BitWrapper<T, U>& in) {
            value = in;
            return *this;
        }

        // 负号运算符
        inline constexpr BitWrapper operator-() const {
            return -value;
        }

        // 重载所有的赋值运算符
        template <typename T>
        inline BitWrapper &operator*=(const T num) {
            value *= num;
            return *this;
        }
        template <typename T>
        inline BitWrapper &operator/=(const T num) {
            value /= num;
            return *this;
        }
        template <typename T>
        inline BitWrapper &operator+=(const T num) {
            value += num;
            return *this;
        }
        template <typename T>
        inline BitWrapper &operator-=(const T num) {
            value -= num;
            return *this;
        }
        template <typename T>
        inline BitWrapper &operator<<=(const T num) {
            value <<= num;
            return *this;
        }
        template <typename T>
        inline BitWrapper &operator>>=(const T num) {
            value >>= num;
            return *this;
        }
        template <typename T>
        inline BitWrapper &operator&=(const T num) {
            value &= num;
            return *this;
        }
        template <typename T>
        inline BitWrapper &operator|=(const T num) {
            value |= num;
            return *this;
        }
        template <typename T>
        inline BitWrapper &operator^=(const T num) {
            value ^= num;
            return *this;
        }
        template <typename T>
        inline BitWrapper &operator%=(const T num) {
            value %= num;
            return *this;
        }

        // 重载自加自减类运算符
        inline BitWrapper &operator++() {
            ++value;
            return *this;
        }
        inline BitWrapper operator++(int) {
            BitWrapper ret = value;
            ++value;
            return ret;
        }
        inline BitWrapper &operator--() {
            --value;
            return *this;
        }
        inline BitWrapper operator--(int) {
            BitWrapper ret = value;
            --value;
            return ret;
        }
        // 弱枚举、内建数值类型
        template<typename T, std::enable_if_t<std::is_arithmetic_v<T> || (std::is_enum_v<T> && std::is_convertible_v<T, P>)> * = nullptr>
        constexpr BitWrapper(const T in) :value(in), dummy(0) {}

        // 强枚举类型
        template<typename T, std::enable_if_t<std::is_enum_v<T> && !std::is_convertible_v<T, P>> * = nullptr>
        constexpr explicit BitWrapper(const T in) : value(P(in)), dummy(0) {}

        // 高精度BitWrapper往低精度构造，必须显式
        template<typename T, uint16_t U, std::enable_if_t< N<U > * = nullptr>
            constexpr explicit BitWrapper(const BitWrapper<T, U> & in) : value(P(in)), dummy(0) {}

        // 低精度往高精度构造，可以隐式
        template<typename T, uint16_t U, std::enable_if_t< N >= U > * = nullptr>
        constexpr BitWrapper(const BitWrapper<T, U> & in) : value(in), dummy(0) {}

        // 平凡默认构造函数
        constexpr BitWrapper() = default;

    private:
        P value : N;
        P dummy : sizeof(P) * 8 - N;  // 非有效位永远为0
    };

#define REG_BIT_WRAPPER(pack_type, bits,...)\
using __VA_ARGS__##int##bits##_t = BitWrapper<pack_type, bits>;\
inline constexpr __VA_ARGS__##int##bits##_t operator "" _##__VA_ARGS__##i##bits(const unsigned long long in){\
    return static_cast<pack_type>(in);\
}

    REG_BIT_WRAPPER(uint8_t, 4, u);
    REG_BIT_WRAPPER(int8_t, 4);
    REG_BIT_WRAPPER(uint16_t, 9, u);
    REG_BIT_WRAPPER(int16_t, 9);
    REG_BIT_WRAPPER(int32_t, 24);
    REG_BIT_WRAPPER(uint32_t, 24, u);
    REG_BIT_WRAPPER(int64_t, 33);
    REG_BIT_WRAPPER(uint64_t, 33, u);
   int main() {
        // 丝滑使用
        constexpr uint4_t a{ 4 };
        int9_t b{ 258 };
        constexpr uint9_t c{ 258 };
        uint4_t d{ 1 };
        constexpr std::array <int9_t, a> arr{ 257, 256, 255, 254 };
        constexpr std::array arrl{ -1_i9, -2_i9, 3_i9 };
        constexpr std::array<int9_t, 3_i9> arr2{ -1_i9, -2_i9, 3_i9 };

        for (auto & n : arr) {
            cout << n << endl;
        }
        cout << int9_t::min << ',' << int9_t::max << endl;
        cout << uint9_t::min << ',' << uint9_t::max << endl;
        cout << int4_t::min + 0 << ',' << int4_t::max + 0 << endl;
        cout << (b >>= a) << endl;
        cout << a + b + c << ','<<int(d <<= 1) << endl;
        auto e = b * c;
        auto f = b > 0 ? c : a;
        uint4_t g = One;
        constexpr int33_t h{ 0xfffffffff };
        uint9_t i{ uint9_t(~0u) };
        int24_t j{ int24_t(~0) };
        constexpr auto k = arr[0];
        constexpr int l = arr[0];
        //constexpr uint4_t m = 33.14;
        //constexpr float n = m;
        b = c;

        printf("%d", b.get());
        {
            // 类型不一致
            auto x = b > 0 ? a.get(): 1;
            auto x1 = b > 0 ? a : 1_ui4;
        }

        constexpr Num p = static_cast<Num>(a);  // BitWrapper转成弱枚举
        constexpr Day q = static_cast<Day>(a);  // BitWrapper转成强枚举
        constexpr int9_t r = static_cast<int9_t>(Day::Mon);  // 强枚举转成BitWrapper
        auto t = -a;
        auto u = -r;
        b = One; // 弱枚举直接赋值
        int9_t{ One };  // 弱枚举初始化BitWrapper
        int33_t{ One };
        uint4_t{ One };
        b = static_cast<int9_t>(Day::Mon);  // 强枚举转成BitWrapper赋值

        int9_t{ Day::Mon };  // 强枚举初始化BitWrapper
        switch (uint4_t{1})
        {
        case One:
            break;
        case Two:
            break;
        default:
            break;
        }


        {
            uint9_t u{ 2 };
            int24_t v{ u };
            uint9_t w = u;
            int9_t y = u;
            int4_t x = static_cast<int4_t>(u);
            constexpr int4_t ccc{};
            int4_t cc{};
        }
    }
