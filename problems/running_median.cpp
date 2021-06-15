#include <bits/stdc++.h>

using namespace std;

string ltrim(const string&);
string rtrim(const string&);

/*
 * Complete the 'runningMedian' function below.
 *
 * The function is expected to return a DOUBLE_ARRAY.
 * The function accepts INTEGER_ARRAY a as parameter.
 */

vector<double> runningMedian(vector<int> a) {
    size_t length = a.size();
    if (length == 0) {
        return {};
    } else if (length == 1) {
        return {double(a[0])};
    }

    // length is at least 2
    vector<double> medians(length, 0.);

    const bool length_is_even = (length % 2 == 0);
    if (!length_is_even) {
        assert(length > 2);
        // if length is odd, the loop below would have buffer overflow,
        // so patch it up by reducing the length
        --length;
        medians[length] = a[(length + 1) / 2];
    }

    for (size_t i = 0; i < length; i += 2) {
        // compute median for odd length
        size_t midpoint_odd = (i + 1) / 2;
        medians[i] = double(a[midpoint_odd]);
        // compute median for even length
        medians[i + 1] = double(a[midpoint_odd] + a[midpoint_odd + 1]) / 2.0;
    }

    return medians;
}

#define fout cout

int main() {
    // ofstream fout(getenv("OUTPUT_PATH"));

    string a_count_temp;
    getline(cin, a_count_temp);

    int a_count = stoi(ltrim(rtrim(a_count_temp)));

    vector<int> a(a_count);

    for (int i = 0; i < a_count; i++) {
        string a_item_temp;
        getline(cin, a_item_temp);

        int a_item = stoi(ltrim(rtrim(a_item_temp)));

        a[i] = a_item;
    }

    vector<double> result = runningMedian(a);

    for (int i = 0; i < a_count; i++) {
        fout << result[i];

        if (i != a_count - 1) {
            fout << "\n";
        }
    }

    fout << "\n";

    // fout.close();

    return 0;
}

string ltrim(const string& str) {
    string s(str);

    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));

    return s;
}

string rtrim(const string& str) {
    string s(str);

    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());

    return s;
}
