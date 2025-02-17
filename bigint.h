#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class bigint;  // Forward declaration

// used so that x + 1 and 1 + x are both valid, where x is a bigint
template<typename T1, typename T2>
using OneBigInt = std::enable_if_t< (std::is_same_v<bigint, std::decay_t<T1>>) ^ (std::is_same_v<bigint, std::decay_t<T2>>) >;

class bigint {
    private: 

        std::string str; // base-10 representation, excluding sign
        char sign = '+';

        void trim() {
            if(str == "0") return;
            int i = 0;
            for (i=0; i<str.length(); i++) {
              if (str[i] != '0') break;
            }
            str.erase(str.begin(), str.begin() + i);
            if (str.empty()) str = "0";
        }

    public: 

        bigint() {
            str = '0';  //default value
            sign = '+';
        }

        bigint(const bigint& other) = default; // Copy constructor
        bigint& operator=(const bigint& other) = default;  // Copy assignment
        
        bigint(bigint&& other) noexcept { // Move constructor
            str = std::move(other.str);
            sign = other.sign;
        }

        bigint& operator=(bigint&& other) noexcept {  // Move assignment
            str = std::move(other.str);
            sign = other.sign;
            return *this;
        }

        bigint(const std::string& s) {
            if (s[0] == '-' or s[0] == '+') {
                sign = s[0];
                str = s.substr(1);
            } else {
                str = s;
            }
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
        bigint(T n) {
            str = std::to_string(n);
            if (str[0] == '-' or str[0] == '+') {
                sign = str[0];
                str = str.erase(0, 1);
            }
        }

        explicit operator int64_t() const { // must use static_cast<int64_t>
            return stoll(str) * (sign == '-' ? -1 : 1); 
         }

        // STREAM OPERATORS
        friend std::ostream &operator << (std::ostream& stream, const bigint &n) {
            if (n.sign == '-') stream << n.sign;
            stream << n.str;
            return stream;
        }

        friend std::istream &operator >> (std::istream& stream, bigint &n) {
            stream >> n.str;
            if (n.str[0] == '-' or n.str[0] == '+') {
                n.sign = n.str[0];
                n.str = n.str.erase(0, 1);
            }
            return stream;
        }

        void neg() {
            if (sign == '+' and str != "0") sign = '-';
            else if (sign == '-' and str != "0") sign = '+';
        }

        void abs() {
            sign = '+';
        }

        // ARITHMETIC OPERATORS

        bigint abs_sum(const bigint &a, const bigint &b) const {
            std::string sum;
            sum.reserve( std::max(a.str.size(), b.str.size()) + 1 );
            int i, j, track_sum, carry = 0;
            for(i = a.str.size()-1, j = b.str.size()-1; i>=0 && j>=0; --i, --j) {
                track_sum = (a.str[i]-'0') + (b.str[j]-'0') + carry;
                carry = track_sum/10;
                sum.push_back('0' + (track_sum%10));
            }
            if(i >= 0 && j < 0) {
                for(; i >= 0; --i) {
                    track_sum = (a.str[i]-'0') + carry;
                    carry = track_sum/10;
                    sum.push_back('0' + (track_sum%10));
                }
            } else if(j >= 0 && i < 0) {
                for(; j >= 0; --j) {
                    track_sum = (b.str[j]-'0') + carry;
                    carry = track_sum/10;
                    sum.push_back('0' + (track_sum%10));
                }
            }

            if (carry) sum.push_back('0' + carry);

            std::reverse(sum.begin(), sum.end());
            bigint ans(sum);
            return ans;
        }

        bigint operator+(const bigint &b) const {
            const bigint &a = *this;

            if (a.str.size() == 0 && b.str.size() == 0) return bigint();
            if (a.str == "0") return b;
            if (b.str == "0") return a;

            if (a.sign != b.sign) {
                if (abs_lte(a,b)) {
                  bigint ans = abs_diff(b,a);
                  ans.sign = (ans.str == "0" ? '+' : b.sign);
                  return ans;
                }
                else {
                  bigint ans = abs_diff(a,b);
                  ans.sign = (ans.str == "0" ? '+' : a.sign);
                  return ans;
                }
            }

            bigint ans = abs_sum(a,b);
            if (a.sign == '-' and b.sign == '-') ans.sign = '-';
            return ans;
        }

        template<typename T1, typename T2, typename = OneBigInt<T1,T2>> 
        friend bigint operator+(const T1 &a, const T2 &b) { 
            return bigint(a) + bigint(b);
        }

        template<typename T> bigint& operator+=(const T &n) { 
            (*this) = (*this) + bigint(n); 
            return (*this);
        }
        

        // a must be greater than or equal to b
        bigint abs_diff(const bigint &a, const bigint &b) const {
            std::string sum;
            sum.reserve( std::max(a.str.size(), b.str.size()) );
            int i, j, track_sum, carry = 0;      
            int val1, val2;
            for(i = a.str.size()-1, j = b.str.size()-1; i >= 0 || j >= 0; --i, --j) {
                if(i >= 0) val1 = a.str[i] - '0';
                else val1 = 0;

                if(j >= 0) val2 = b.str[j] - '0';
                else val2 = 0;

                track_sum = val1 - val2 - carry;
                if(track_sum < 0) {
                    track_sum += 10;
                    carry = 1;
                } else {
                    carry = 0;
                }
                sum.push_back('0' + track_sum);
            }
            std::reverse(sum.begin(), sum.end());
            bigint ans(sum);
            ans.trim();
            return ans;
        }

        bigint operator-(const bigint &b) const {
            const bigint &a = *this;

            if (b.str == "0") return a;
            if (a.str == "0") {
                bigint ans(b);
                ans.neg();
                return ans;
            }

            if (a.sign != b.sign) {
                bigint ans = abs_sum(a,b);
                ans.sign = a.sign;
                return ans;
            }

            if (abs_lte(b,a)) {
                bigint ans = abs_diff(a,b);
                ans.sign = a.sign; 
                if (ans.str == "0") ans.sign = '+';
                return ans;
            } else {
                bigint ans = abs_diff(b,a);
                ans.sign = b.sign == '+' ? '-' : '+';
                return ans;
            }
        }

        template<typename T1, typename T2, typename = OneBigInt<T1, T1>>
        friend bigint operator-(const T1 &a, const T2 &b) { 
            return bigint(a) - bigint(b); 
        }

        template<typename T> bigint& operator-=(const T &n) { 
            (*this) = (*this) - bigint(n); 
            return (*this); 
        }

        bigint operator*(const bigint &b) const {
            const bigint &a = *this;
            if (a.str == "0" or b.str == "0") return bigint();
            bool is_neg = a.sign != b.sign;
            std::vector<int> result(a.str.size() + b.str.size(), 0);
            int ia = 0;
            int ib = 0;
            for (int i=a.str.size()-1; i>=0; i--) {
                int carry = 0;
                int n1 = a.str[i] - '0';
                ib = 0;
                for (int j=b.str.size()-1; j>=0; j--) {
                    int n2 = b.str[j] - '0';
                    int sum = n1*n2 + result[ia + ib] + carry;
                    carry = sum/10;
                    result[ia + ib] = sum % 10;
                    ib++;
                }
                if (carry > 0) result[ia + ib] += carry;
                ia++;
            }
            int sz = result.size() - 1;
            while (sz>=0 && result[sz] == 0) sz--;
            if (sz == -1) return bigint();                    
            
            bigint ans;
            ans.str = "";
            ans.str.reserve(sz);
            while (sz >= 0) ans.str.push_back( result[sz--] + '0' );
            if (is_neg) ans.sign = '-';
            return ans;
        }

        template<typename T1, typename T2, typename = OneBigInt<T1, T2>>
        friend bigint operator*(const T1 &a, const T2 &b) {
          return bigint(a) * bigint(b);
        }

        template<typename T> bigint& operator *= (const T &n) {
            (*this) = (*this) * bigint(n);
            return (*this);
        }

        bigint operator/(const bigint &b) const {
            const bigint &a = *this;
            if (a.str == "0") return bigint();
            if (b.str == "0") throw std::invalid_argument("Division by zero");
            bool is_neg = a.sign != b.sign;

            if (abs_lte(a,b)) {
                if (a.str == b.str) {
                    bigint ans("1");
                    if (is_neg) ans.sign = '-';
                    return ans;
                }
                else return bigint();
            }

            if (b.str == "1") {
                bigint ans = a;
                if (is_neg) ans.sign = '-';
                return ans;
            }


            // Handle small divisors more efficiently
            if(b.str.size() <= 17) {
                long long int b_int = stoll(b.str);
                bigint ans = a.shortDivide(b_int);
                if (is_neg) ans.sign = '-';
                return ans;
            }

            // Binary search for quotient
            bigint left("0");
            bigint right = a;
            bigint one("1");
            
            while (left <= right) {
                bigint mid = (left + right);
                mid = mid.shortDivide(2ULL);  // Divide by 2
                
                bigint product = mid * b; // This implementation is not efficient
                if (product == a) {
                    if (is_neg) mid.sign = '-';
                    return mid;
                }

                if (product < a) {
                    bigint next = mid + one;
                    bigint next_product = next * b;
                    if (next_product > a) {
                        if (is_neg) mid.sign = '-';
                        return mid;
                    }
                    left = next;
                } else {
                    right = mid - one;
                }
            }
            if (is_neg) left.sign = '-';
            return left;
        }

        template<typename T1, typename T2, typename = OneBigInt<T1, T2>>
        friend bigint operator/(const T1 &a, const T2 &b) {
            return bigint(a) / bigint(b);
        }

        template<typename T> bigint& operator/=(const T &n) {
            (*this) = (*this) / bigint(n);
            return (*this);
        }

        bigint shortDivide(long long int divisor) const {
            if (divisor == 0) throw std::invalid_argument("Division by zero");
            
            std::string result;
            result.reserve(str.size());
            
            unsigned long long int current = 0;
            for(char digit : str) {
                current = current * 10 + (digit - '0');
                result.push_back((current / divisor) + '0');
                current %= divisor;
            }
            
            bigint ans(result);
            ans.trim();
            ans.sign = sign;
            return ans;
        }

        bigint operator%(const bigint &b) const { // do not use const, since we do subtraction and that is non-const
            const bigint &a = *this;
            if (a.str == "0") return bigint();
            if (b.str == "0") throw std::invalid_argument("Modulo by zero");

            bigint quotient = a / b;
            bigint product = quotient * b;
            bigint result = a - product;

            // in c++, the rule is that (a/b)*b + a%b is equal to a
            if (result.str != "0") result.sign = sign;
            return result;
        }

        template<typename T1, typename T2, typename = OneBigInt<T1, T2>>
        friend bigint operator%(const T1 &a, const T2 &b) { 
            return bigint(a) % bigint(b);
        }

        template<typename T> bigint& operator%=(const T &n) {
            (*this) = (*this) % bigint(n);
            return (*this);
        }

        bigint& operator++() {
            *this = (*this) + 1; 
            return (*this);
        }
        bigint operator++(int) {
            *this = (*this) + 1; 
            return (*this);
        }
        bigint& operator--() {
            (*this) = (*this) - 1;
            return (*this);
        }
        bigint operator--(int) {
            (*this) = (*this) - 1; 
            return (*this);
        }

        // Add unary minus operator
        bigint operator-() const {
            bigint result = *this;
            result.neg();
            return result;
        }

        // RELATIONAL OPERATORS

        bool abs_lte(const bigint &a, const bigint &b) const {
            if (a.str.size() != b.str.size()) return a.str.size() < b.str.size();
            else return a.str <= b.str;
        }
        
        bool operator<=(const bigint &b) const {
            const bigint &a = *this;
            bool a_smaller = false;
            if(a.sign == '-' and b.sign == '+') return true;
            if(a.sign == '+' and b.sign == '-') return false;

            // a and b have same sign
            if (a.sign == '+') return abs_lte(a,b);
            else return abs_lte(b,a);
        }

        bool operator==(const bigint &b) const {
            const bigint &a = *this;
            return (a.sign == b.sign and a.str == b.str);
        }
        
        bool operator!=(const bigint &b) const { return !(*this == b); }
        bool operator<(const bigint &b) const { return *this <= b and *this != b; }
        bool operator>(const bigint &b) const { return !(*this <= b); }
        bool operator>=(const bigint &b) const { return *this > b or *this == b; }


        template<typename T1, typename T2, typename = OneBigInt<T1, T2>> friend bool operator==(const T1 &a, const T2 &b) { return bigint(a) == bigint(b); }
        template<typename T1, typename T2, typename = OneBigInt<T1, T2>> friend bool operator!=(const T1 &a, const T2 &b) { return bigint(a) != bigint(b); }
        template<typename T1, typename T2, typename = OneBigInt<T1, T2>> friend bool operator<(const T1 &a, const T2 &b)  { return bigint(a) < bigint(b); }
        template<typename T1, typename T2, typename = OneBigInt<T1, T2>> friend bool operator>(const T1 &a, const T2 &b)  { return bigint(a) > bigint(b); }
        template<typename T1, typename T2, typename = OneBigInt<T1, T2>> friend bool operator<=(const T1 &a, const T2 &b) { return bigint(a) <= bigint(b); }
        template<typename T1, typename T2, typename = OneBigInt<T1, T2>> friend bool operator>=(const T1 &a, const T2 &b) { return bigint(a) >= bigint(b); }

};

