public class PalindromicPrime {
    public static void main(String[] args) {
        for (int i = 2, cnt = 0; ; ++i) {
            if (isPalindrome(i) && isPrime(i)) {
                System.out.println(i);
                ++cnt;
                if (cnt == 100) break;
            }
        }
    }
    public static boolean isPalindrome(int x) {
        String s = x + "";
        int len = s.length();
        for (int i = 0; i <= len / 2; ++i) {
            if (s.charAt(i) != s.charAt(len - i - 1)) return false;
        }
        return true;
    }
    public static boolean isPrime(int x) {
        for (int divisor = 2; divisor * divisor <= x; ++divisor) {
            if (x % divisor == 0) return false;
        }
        return true;
    }
}
